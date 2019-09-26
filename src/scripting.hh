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

namespace drnsf {
namespace scripting {

/*
 * scripting::init
 *
 * Initializes the scripting engine. This prepares the Python 3 library for
 * usage and registers the 'drnsf' module along with its types and functions.
 *
 * TODO - explain possible errors and resulting engine state
 *
 * After the first successful initialization, further calls to this function
 * are silently ignored.
 */
void init();

/*
 * scripting::is_init
 *
 * Returns true if the scripting engine has been previously successfully
 * initialized by a call to `init'. Returns false otherwise.
 */
bool is_init() noexcept;

}
}
