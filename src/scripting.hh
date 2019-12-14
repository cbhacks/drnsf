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

#pragma once

/*
 * scripting.hh
 *
 * FIXME explain
 */

// defined in edit.hh
namespace drnsf::edit {
    class context;
}

namespace drnsf {
namespace scripting {

/*
 * scripting::handle
 *
 * Objects of this type are present as fields in some editor types to provide
 * for two-way associative relations between native objects (owning the field)
 * and scripting objects.
 */
struct handle {
    void *p = nullptr;
};

/*
 * scripting::init
 *
 * Initializes the scripting runtime. This prepares the Python 3 library for
 * usage and registers the 'drnsf' module along with its types and functions.
 *
 * TODO - explain possible errors and resulting runtime state
 *
 * Calling this function while the runtime is already initialized has no effect.
 * Calling this function after the runtime has failed to initialize or after it
 * has been shutdown (`scripting::shutdown' below) throws an exception.
 *
 * If provided, ctxp should be a pointer to an `edit::context' object whose
 * lifetime does not end until after the scripting runtime is shutdown.
 */
void init(edit::context *ctxp = nullptr);

/*
 * scripting::shutdown
 *
 * Shuts down the scripting runtime.
 *
 * Calls to this function are ignored unless the runtime was previously
 * successfully initialized. Once shutdown, the runtime is no longer usable.
 * This should only be called when the application is exiting.
 */
void shutdown() noexcept;

/*
 * scripting::is_init
 *
 * Returns true if the scripting runtime has been previously successfully
 * initialized by a call to `init' and has not been shutdown. Returns false
 * otherwise.
 */
bool is_init() noexcept;

/*
 * scripting::lock
 * scripting::unlock
 *
 * Called to lock or unlock scripting execution. While locked, any script which
 * attempts to access native application objects from another thread will block
 * until the lock is released.
 *
 * Lock and unlock are recursive. It is an error to call `unlock' when there
 * is no active lock.
 *
 * These functions should only be called from the main thread.
 *
 * Calls to these functions are silently ignored if the scripting runtime is not
 * currently initialized.
 */
void lock() noexcept;
void unlock();

/*
 * scripting::engine
 *
 * FIXME explain
 */
class engine : private util::nocopy {
private:
    // inner implementation type
    struct impl;

    // (var) M
    // Pointer to the internal implementation object. This is null if the engine
    // was not actually constructed (i.e. the runtime was not initialized).
    impl *M;

public:
    // (default ctor)
    // Constructs a new scripting engine. If the runtime is not initialized when
    // this object is constructed, no engine is allocated and the object is not
    // meaningfully usable.
    engine();

    // (dtor)
    // Destroys the scripting engine, if one was allocated at construction.
    ~engine();

    // (func) start_console
    // Launches a new Python REPL.
    void start_console();
};

}
}
