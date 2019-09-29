//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2019  DRNSF contributors
//
// See the AUTHORS.md file for more details.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

// Specify that we will be using the Py_ssize_t-enabled API for certain Python
// functions.
#define PY_SSIZE_T_CLEAN

// Specify that we will be using the "limited" Python API from PEP 384. This is
// important for ABI compatibility across minor revisions of the Python library
// on Windows.
#define Py_LIMITED_API

// According to Python documentation, the Python header should be included
// before any system and standard headers. This may preclude precompiled header
// support for 'common.hh' in this file on some C++ implementations.
#include <Python.h>

#include "common.hh"
#include "scripting.hh"
#include "res.hh"
#include <thread>

DRNSF_DECLARE_EMBED(drnsf_py);

namespace drnsf {
namespace scripting {

// (s-var) s_init_state
// The initialization state of the scripting engine.
//
// Possible values:
//   - none:      The engine has not been initialized yet.
//   - ready:     The engine initialized successfully and is ready for use.
//   - failed:    The engine is currently initializing or has failed to do so.
//   - finished:  The engine was initialized and then shutdown.
static enum class init_state {
    none,
    ready,
    failed,
    finished
} s_init_state = init_state::none;

// (s-var) s_lockcount
// The number of locks (see `lock' and `unlock') currently held on the engine
// by the main thread.
static int s_lockcount = 0;

// (s-var) s_main_threadstate
// The main python thread state.
static PyThreadState *s_main_threadstate;

// (s-var) s_module
// An owning pointer to the "drnsf" module.
static PyObject *s_module;

// (s-var) s_dict
// An owning pointer to the dict of the "drnsf" module.
static PyObject *s_dict;

namespace {

// (internal type) conversion_error
// Thrown by `from_python' when the object cannot be converted to the output
// native type.
class conversion_error : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

#define SLOT_FN(x) { (Py_##x), reinterpret_cast<void *>(x) }

// (internal type) scr_base
// Base type for all scripting types defined below.
struct scr_base : PyObject {
};

// (internal type) scr_project
// Scripting type for "Project".
struct scr_project : scr_base {
    using native_type = std::shared_ptr<res::project>;

    static inline PyTypeObject *type;

    native_type proj_p;

    static native_type from_python(PyObject *obj)
    {
        if (obj == Py_None)
            return nullptr;

        if (Py_TYPE(obj) != type)
            throw conversion_error("scr_project: incompatible type");

        return static_cast<scr_project *>(obj)->proj_p;
    }

    static PyObject *to_python(native_type value) noexcept
    {
        if (!value)
            Py_RETURN_NONE;

        if (value->m_scripthandle.p) {
            auto obj = static_cast<scr_project *>(value->m_scripthandle.p);
            Py_INCREF(obj);
            return obj;
        }

        try {
            auto obj = new scr_project();
            obj->ob_refcnt = 1;
            obj->ob_type = type;
            obj->proj_p = value;
            obj->proj_p->m_scripthandle.p = obj;
            return obj;
        } catch (std::bad_alloc &) {
            return PyErr_NoMemory();
        }
    }

    static PyObject *tp_new(
        PyObject *subtype,
        PyObject *args,
        PyObject *kwds) noexcept
    {
        if (PyTuple_Size(args) != 0) {
            PyErr_SetString(PyExc_TypeError, "drnsf.Project() takes no parameters");
            return nullptr;
        }
        if (kwds && PyMapping_Size(kwds) != 0) {
            PyErr_SetString(PyExc_TypeError, "drnsf.Project() takes no parameters");
            return nullptr;
        }
        try {
            return to_python(std::make_shared<res::project>());
        } catch (std::bad_alloc &) {
            return PyErr_NoMemory();
        }
    }

    static void tp_dealloc(scr_project *self) noexcept
    {
        self->proj_p->m_scripthandle = {};
        delete self;
    }

    static void install()
    {
        if (type)
            return;

        static PyType_Slot slots[] = {
            SLOT_FN(tp_new),
            SLOT_FN(tp_dealloc),
            {}
        };

        static PyType_Spec spec = {
            "drnsf.Project",
            sizeof(scr_project),
            0,
            0,
            slots
        };

        auto type_o = PyType_FromSpec(&spec);
        type = reinterpret_cast<PyTypeObject *>(type_o);
        if (!type) {
            PyErr_Print();
            std::abort();
        }

        if (PyDict_SetItemString(s_dict, "Project", type_o) == -1) {
            PyErr_Print();
            std::abort();
        }
    }
};

#undef SLOT_FN

}

// declared in scripting.hh
void init()
{
    if (s_init_state == init_state::ready)
        return; // no-op
    if (s_init_state == init_state::failed)
        throw std::runtime_error("scripting::init: init previously failed");
    if (s_init_state == init_state::finished)
        throw std::runtime_error("scripting::init: already shutdown");

    // Set the initialization state to 'failed'. If the function exits abruptly
    // such as from a thrown exception, this will be the resulting state.
    s_init_state = init_state::failed;

    // If this function fails, the entire process is killed unfortunately. This
    // is solved in PEP 587 which is not available in any stable builds at time
    // of writing.
    Py_Initialize();

    PyEval_InitThreads();
    s_main_threadstate = PyThreadState_Get();

    s_module = PyImport_AddModule("drnsf");
    if (!s_module) {
        PyErr_Print();
        std::abort();
    }
    Py_INCREF(s_module);

    s_dict = PyModule_GetDict(s_module);
    if (!s_dict) {
        PyErr_Print();
        std::abort();
    }
    Py_INCREF(s_dict);

    scr_project::install();

    auto code = Py_CompileString(
        reinterpret_cast<const char *>(embed::drnsf_py::data),
        "drnsf.py",
        Py_file_input
    );
    if (!code) {
        PyErr_Print();
        std::abort();
    }

    auto result = PyImport_ExecCodeModule("drnsf", code);
    if (!result) {
        PyErr_Print();
        std::abort();
    }

    Py_DECREF(code);
    Py_DECREF(result);
    PyEval_ReleaseThread(s_main_threadstate);

    s_init_state = init_state::ready;

    // FIXME temporary hack until scripting is thread-safe
    lock();
    // this hack lock is released during select()/WaitForMulitipleObjectsEx()
}

// declared in scripting.hh
void shutdown() noexcept
{
    if (s_init_state != init_state::ready)
        return;

    // One final lock. This is never released.
    lock();

    Py_Finalize();

    s_init_state = init_state::finished;
}

// declared in scripting.hh
bool is_init() noexcept
{
    return s_init_state == init_state::ready;
}

// declared in scripting.hh
void lock() noexcept
{
    if (s_init_state != init_state::ready)
        return;
    if (s_lockcount < 0)
        std::abort();

    if (s_lockcount == 0)
        PyEval_AcquireThread(s_main_threadstate);

    s_lockcount++;
}

// declared in scripting.hh
void unlock()
{
    if (s_init_state != init_state::ready)
        return;
    if (s_lockcount < 0)
        std::abort();
    if (s_lockcount == 0)
        throw std::runtime_error("scripting::unlock: not locked");

    if (s_lockcount == 1)
        PyEval_ReleaseThread(s_main_threadstate);

    s_lockcount--;
}

}
}
