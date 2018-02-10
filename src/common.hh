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
 * STRINGIFY
 *
 * FIXME explain
 */
#define STRINGIFY(x) STRINGIFY_(x)
#define STRINGIFY_(x) #x

// Include the application build configuration. This is partially generated
// by CMake when configuring and generating your build system.
#define DRNSF_BUILDCONFIG_INCLUDED_PROPERLY
#include "drnsf_buildconfig_out.hh"
#undef DRNSF_BUILDCONFIG_INCLUDED_PROPERLY

#include "util.hh"

/*
 * DRNSF_ON_EXIT
 *
 * This macro can be used within a code block to provide code which will run
 * when that code block exits, whether by a statement such as `return' or
 * `break', or by a thrown exception. Example usage:
 *
 *   FILE *f = fopen("config.txt", "r");
 *   if (!f) { exit(1); }
 *   DRNSF_ON_EXIT { fclose(f); };
 *   }
 *
 *   ...
 *
 *   return 0;
 *
 * These error handlers can also be stacked, for example:
 *
 *   int err = SDL_Init(SDL_INIT_VIDEO);
 *   if (err < 0) { ... }
 *   DRNSF_ON_EXIT { SDL_Quit(); };
 *
 *   auto wnd = SDL_CreateWindow(...);
 *   if (!wnd) { ... }
 *   DRNSF_ON_EXIT { SDL_DestroyWindow(wnd); };
 *
 *   ...
 *
 *   return 0;
 *
 * Using DRNSF_ON_EXIT is often more reliable than placing cleanup code after a
 * code block because DRNSF_ON_EXIT blocks will execute even if the block exit
 * is due to an exception being thrown. For example:
 *
 *   CURL *c = curl_easy_init();
 *   if (!c) { ... }
 *
 *   ... some code which may throw an exception ...
 *
 *   curl_easy_cleanup(c);
 *
 * In this case, if the intervening code throws an exception, curl_easy_cleanup
 * is not called, which results in a resource leak. Using DRNSF_ON_EXIT:
 *
 *   CURL *c = curl_easy_init();
 *   if (!c) { ... }
 *   DRNSF_ON_EXIT { curl_easy_cleanup(c); };
 *
 *   ... some code which may throw an exception ...
 *
 * In this case, even if the code throws an exception, the curl object will be
 * released properly.
 *
 * IMPORTANT: Because the body of a DRNSF_ON_EXIT statement may execute as part
 * of exception unwinding, the body code MUST NOT throw an exception itself. It
 * is acceptable for an exception to be thrown within execution of the body,
 * however it must also be caught within the body and not allowed to escape.
 */
#define DRNSF_ON_EXIT \
    auto DRNSF_ON_EXIT__##__LINE__ = ::drnsf::util::on_exit_helper % [&]

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
inline auto operator "" _fmt(const char *s, std::size_t sz)
{
    return util::fmt(s);
}

}
