//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2020  DRNSF contributors
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

#pragma once

/*
 * gl.hh
 *
 * FIXME explain
 *
 * For compile speed reasons, this file intentionally does not include any
 * frontend-specific headers (especially "windows.h"). If you are including
 * this file with the intention of implementing frontend-specific behavior, you
 * may wish to #define DRNSF_FRONTEND_IMPLEMENTATION before including this
 * file.
 */

// Windows system header must be included before "gl.h" except in cases where it
// is never included. This fixes MSVC C4005 "'APIENTRY': macro redefinition".
#if defined(DRNSF_FRONTEND_IMPLEMENTATION) && USE_WINAPI
#include <windows.h>
#endif

#include <epoxy/gl.h>
#include <glm/glm.hpp>
#include <unordered_set>

#ifdef DRNSF_FRONTEND_IMPLEMENTATION
#if USE_X11
#include <epoxy/glx.h>
#elif USE_WINAPI
#include <epoxy/wgl.h>
#endif
#endif

namespace drnsf {
namespace gl {

/*
 * gl::init
 *
 * FIXME explain
 *
 * If there is already an active GL context, this function returns immediately
 * with no error.
 *
 * Initialization is recursive; each call to init should be matched to a call
 * to shutdown. Only the outermost init/shutdown call pair actually performs
 * initialization and shutdown. For example:
 *
 *   ... //no gl context (init count = 0)
 *   gl::init(); //may throw
 *   ... //gl context OK (init count = 1)
 *   gl::init(); //never throws
 *   ... //gl context OK (init count = 2)
 *   gl::shutdown(); //no change to gl objects
 *   ... //gl context OK (init count = 1)
 *   gl::shutdown(); //all gl objects get reset here
 *   ... //no gl context (init count = 0)
 */
void init();

/*
 * gl::shutdown
 *
 * Resets all of the gl::object handles (see below), releases the GL context,
 * and closes any background windows or other resources which were in use by
 * gl::init.
 *
 * This function must not be called unless gl::init has been called previously
 * with no error (no exceptions thrown).
 *
 * Initialization is recursive; each call to init should be matched by a call
 * to shutdown. Only the outermost init/shutdown call pair actually performs
 * initialization and shutdown. For an example, see the comment for `init'.
 */
void shutdown() noexcept;

/*
 * gl::is_init
 *
 * Returns true if there is an active GL context.
 */
bool is_init() noexcept;

/*
 * gl::error
 *
 * This is an exception type based on the standard C++ runtime_error. It is
 * intended for errors related to OpenGL usage, such as shader compile/link
 * errors or GL initialization errors.
 *
 * If possible, information about the OpenGL implementation is collected when
 * the object is constructed, and can be retrieved from the object later, even
 * if the GL context has already been closed by then.
 */
class error : public std::runtime_error {
private:
    // (var) m_has_details
    // True if the error object has details for the GL implementation, such as
    // vendor, version, etc.
    bool m_has_details = false;

    // (var) m_vendor
    // The value of GL_VENDOR, if available.
    std::string m_vendor;

    // (var) m_renderer
    // The value of GL_RENDERER, if available.
    std::string m_renderer;

    // (var) m_version
    // The value of GL_VERSION, if available.
    std::string m_version;

    // (var) m_glsl_version
    // The value of GL_SHADING_LANGUAGE_VERSION, if available.
    std::string m_glsl_version;

    // (var) m_has_extensions
    // True if the error object has the list of GL extensions for the GL
    // implementation. This must be false if `m_has_details' is false.
    bool m_has_extensions = false;

    // (var) m_extensions
    // The list of extension strings retrieved from GL_EXTENSIONS, if
    // available.
    std::vector<std::string> m_extensions;

    // (func) collect
    // Populates the above GL information variables.
    void collect();

public:
    // (explicit ctor)
    // Constructs the error with the given string as its message. This is
    // passed verbatim to the `runtime_error' error constructor.
    //
    // If there is an active GL context, information about its implementation
    // will be stored for future retrieval.
    explicit error(const std::string &what_arg);

    // (explicit ctor)
    // Identical to the other constructor, however the string is given by
    // `const char *'.
    explicit error(const char *what_arg);

    // (func) has_details
    // Returns true if the error object has collected details from the GL
    // implementation. Otherwise, returns false.
    bool has_details() noexcept;

    // (func) gl_vendor
    // Returns the value of GL_VENDOR from the collected GL details. Throws if
    // no details were collected.
    const std::string &gl_vendor();

    // (func) gl_renderer
    // Returns the value of GL_RENDERER from the collected GL details. Throws
    // if no details were collected.
    const std::string &gl_renderer();

    // (func) gl_version
    // Returns the value of GL_VERSION from the collected GL details. Throws if
    // no details were collected.
    const std::string &gl_version();

    // (func) gl_glsl_version
    // Returns the value of GL_SHADING_LANGUAGE_VERSION from the collected GL
    // details. Throws if no details were collected.
    const std::string &gl_glsl_version();

    // (func) has_extensions
    // Returns true if the error object has collected a list of GL extensions
    // from the GL implementation. Otherwise, returns false.
    //
    // This function always returns false if `has_details' returns false.
    bool has_extensions() noexcept;

    // (func) gl_extensions
    // Returns the list of extensions retrieved from GL_EXTENSIONS from the
    // collected GL details. Throws if no extension list or details were
    // collected.
    const std::vector<std::string> &gl_extensions();

    // (func) dump
    // Dumps all of the collected information, if any, to the given stream. The
    // output is intended for human consumption, not for any kind of parsing or
    // similar automated work.
    void dump(std::ostream &out);
};

/*
 * gl::any_object
 *
 * FIXME explain
 */
class any_object : private util::nocopy {
private:
    // (s-func) get_all_objects
    // FIXME explain
    static std::unordered_set<any_object *> &get_all_objects();

public:
    // (s-func) reset_all
    // FIXME explain
    static void reset_all();

    // (default ctor)
    // FIXME explain
    any_object()
    {
        get_all_objects().insert(this);
    }

    // (dtor)
    // FIXME explain
    virtual ~any_object()
    {
        get_all_objects().erase(this);
    }

    // (pure func) reset
    // FIXME explain
    virtual void reset() = 0;
};

/*
 * gl::object
 *
 * FIXME explain
 */
template <typename traits>
class object : any_object {
private:
    // (var) m_id
    // FIXME explain
    typename traits::type m_id = traits::null_value;

public:
    // (var) ok
    // This flag and its value are intended for the users of the class to use
    // in whatever way they wish. The initial value is false, and it is set to
    // false again whenever the `reset' method is called, such as when the GL
    // context is closed.
    //
    // This class does not access or use this flag in any way except to set it
    // false when a reset occurs. Users of this class may choose to ignore the
    // flag entirely.
    bool ok = false;

    // (default ctor)
    // FIXME explain
    object() = default;

    // (move ctor)
    // FIXME explain
    object(object &&src)
    {
        using std::swap;
        swap(m_id, src.m_id);
        swap(ok, src.ok);
    }

    // (dtor)
    // FIXME explain
    ~object()
    {
        if (m_id != traits::null_value) {
            traits::destroy(m_id);
        }
    }

    // (func) reset
    // FIXME explain
    void reset() final override
    {
        if (m_id != traits::null_value) {
            traits::destroy(m_id);
            m_id = traits::null_value;
        }
        ok = false;
    }

    // (conversion operator)
    // FIXME explain
    operator typename traits::type() &
    {
        if (m_id == traits::null_value) {
            traits::create(m_id);
        }
        return m_id;
    }
};

/*
 * gl::base_traits
 *
 * FIXME explain
 */
struct base_traits {
    using type = GLuint;
    constexpr static type null_value = 0;
};

/*
 * gl::texture
 * gl::texture_traits
 *
 * FIXME explain
 */
struct texture_traits : base_traits {
    static void create(type &v)
    {
        glGenTextures(1, &v);
    }
    static void destroy(type v)
    {
        glDeleteTextures(1, &v);
    }
};
using texture = object<texture_traits>;

/*
 * gl::buffer
 * gl::buffer_traits
 *
 * FIXME explain
 */
struct buffer_traits : base_traits {
    static void create(type &v)
    {
        glGenBuffers(1, &v);
    }
    static void destroy(type v)
    {
        glDeleteBuffers(1, &v);
    }
};
using buffer = object<buffer_traits>;

/*
 * gl::vert_array
 * gl::vert_array_traits
 *
 * FIXME explain
 */
struct vert_array_traits : base_traits {
    static void create(type &v)
    {
        glGenVertexArrays(1, &v);
    }
    static void destroy(type v)
    {
        glDeleteVertexArrays(1, &v);
    }
};
using vert_array = object<vert_array_traits>;

/*
 * gl::renderbuffer
 * gl::renderbuffer_traits
 *
 * FIXME explain
 */
struct renderbuffer_traits : base_traits {
    static void create(type &v)
    {
        glGenRenderbuffers(1, &v);
    }
    static void destroy(type v)
    {
        glDeleteRenderbuffers(1, &v);
    }
};
using renderbuffer = object<renderbuffer_traits>;

/*
 * gl::framebuffer
 * gl::framebuffer_traits
 *
 * FIXME explain
 */
struct framebuffer_traits : base_traits {
    static void create(type &v)
    {
        glGenFramebuffers(1, &v);
    }
    static void destroy(type v)
    {
        glDeleteFramebuffers(1, &v);
    }
};
using framebuffer = object<framebuffer_traits>;

/*
 * gl::program
 * gl::program_traits
 *
 * FIXME explain
 */
struct program_traits : base_traits {
    static void create(type &v)
    {
        v = glCreateProgram();
    }
    static void destroy(type v)
    {
        glDeleteProgram(v);
    }
};
using program = object<program_traits>;

/*
 * gl::link_program
 *
 * Attempts to link the specified GL program. If an error occurs, the program
 * linker info log is output to the console and a `gl::error' object is thrown.
 */
void link_program(unsigned int prog);

/*
 * gl::shader
 * gl::shader_traits
 * gl::vert_shader
 * gl::frag_shader
 *
 * FIXME explain
 */
template <int ShaderType>
struct shader_traits : base_traits {
    static void create(type &v)
    {
        v = glCreateShader(ShaderType);
    }
    static void destroy(type v)
    {
        glDeleteShader(v);
    }
};
template <int ShaderType>
using shader = object<shader_traits<ShaderType>>;
using vert_shader = shader<GL_VERTEX_SHADER>;
using frag_shader = shader<GL_FRAGMENT_SHADER>;

/*
 * gl::shader_source
 *
 * Calls glShaderSource on the specified shader with the given source strings.
 */
void shader_source(
    unsigned int sh,
    std::initializer_list<std::string_view> sources);

/*
 * gl::compile_shader
 *
 * FIXME explain
 */
void compile_shader(unsigned int sh);

#ifdef DRNSF_FRONTEND_IMPLEMENTATION
#if USE_X11
// (var) g_wnd
// The background window which the GL context is bound to.
extern Window g_wnd;

// (var) g_ctx
// The GL context.
extern GLXContext g_ctx;

// (var) g_vi
// XVisualInfo pointer used for creating GLX-capable windows.
extern XVisualInfo *g_vi;
#elif USE_WINAPI
// (var) g_hwnd
// The background window which the GL context is bound to.
extern HWND g_hwnd;

// (var) g_hdc
// The device context for the background window which the GL context is bound
// to.
extern HDC g_hdc;

// (var) g_hglrc
// The GL context.
extern HGLRC g_hglrc;

// (var) g_pfd, g_pfid
// The pixel format used for OpenGL-compatible windows.
extern PIXELFORMATDESCRIPTOR g_pfd;
extern int g_pfid;
#endif
#endif

}
}
