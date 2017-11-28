//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017  DRNSF contributors
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
 * common.hh
 *
 * This header file is the first #include in every source file, even before any
 * system headers.
 *
 * `common.hh' provides a common set of definitions and includes which are
 * likely to be needed across the entire project.
 */

#include <cstddef>
#include <cstdlib>
#include <memory>
#include <utility>
#include <string>
#include <functional>
#include <type_traits>

/*
 * APP_NAME
 *
 * FIXME explain
 */
#define APP_NAME "DRNSF"

/*
 * APP_TITLE
 *
 * FIXME explain
 */
#define APP_TITLE "Dr. N. Essef"

/*
 * STRINGIFY
 *
 * FIXME explain
 */
#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

#include "util.hh"

namespace drnsf {

/*
 * (user-defined literal)
 *
 * This allows one to use a construct such as the following:
 *
 *  `auto s = "Size: $, $"_fmt(width, height);'
 *
 * to format strings. This code is equivalent to:
 *
 *  `auto s = util::fmt("Size: $, $")(width, height);'
 *
 * but is shorter and perhaps easier to read.
 */
inline auto operator "" _fmt(const char *s, long unsigned int sz)
{
    return util::fmt(s);
}

}
