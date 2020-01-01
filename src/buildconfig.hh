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

#ifndef DRNSF_BUILDCONFIG_INCLUDED_PROPERLY
#error "Do not include this file directly!"
#endif

/*
 * buildconfig.hh
 *
 * This file is consumed by cmake's configure_file directive, producing a
 * processed file filled with any CMake settings which need to be brought
 * into C++ code through preprocessor constants.
 *
 * For more details on this format, see the CMake documentation.
 * <https://cmake.org/cmake/help/v3.5/command/configure_file.html>
 *
 * Do not include this file directly, it must be included in a special manner
 * which is handled automatically for you by "common.hh".
 */

#define APP_NAME "DRNSF"
#define APP_FULLNAME "Dr. N. Essef"

#if ${DRNSF_VERSION_MAJOR} == 0
#define APP_VERSION "DEV Version"
#define APP_IS_DEV_VERSION 1
#define APP_TITLE APP_FULLNAME " DEV"
#else
#define APP_VERSION "Version ${DRNSF_VERSION}"
#define APP_VERSION_MAJOR ${DRNSF_VERSION_MAJOR}
#define APP_TITLE APP_FULLNAME " v${DRNSF_VERSION}"
#endif

#cmakedefine USE_GUI 1
#cmakedefine USE_X11 1
#cmakedefine USE_WINAPI 1

#cmakedefine USE_GL 1
#cmakedefine USE_NATIVE_MENU 1
#cmakedefine USE_NATIVE_FILE_DIALOG 1

#cmakedefine FEATURE_INTERNAL_TEST 1
