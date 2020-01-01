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
 * common.hh
 *
 * This header file is the first #include in every source file, even before any
 * system headers.
 *
 * `common.hh' provides a common set of definitions and includes which are
 * likely to be needed across the entire project.
 */

// Disable certain pervasive MSVC warnings.
#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#pragma warning( disable : 4244 )
#pragma warning( disable : 4267 )
#endif

#include <cstddef>
#include <cstdlib>
#include <cstdint>
#include <climits>
#include <cinttypes>
#include <memory>
#include <utility>
#include <string>
#include <functional>
#include <type_traits>
#include <mutex>

/*
 * DRNSF_STRINGIFY
 *
 * FIXME explain
 */
#define DRNSF_STRINGIFY_(x) #x
#define DRNSF_STRINGIFY(x) DRNSF_STRINGIFY_(x)

/*
 * DRNSF_CONCAT
 *
 * Concatenates two symbols into a single symbol. This macro is necessary
 * because directly using FOO##BAR would simply return FOOBAR instead of the
 * defined values for FOO and BAR concatenated.
 */
#define DRNSF_CONCAT_(a, b) a##b
#define DRNSF_CONCAT(a, b) DRNSF_CONCAT_(a, b)

/*
 * DRNSF_ANON_VAR
 *
 * Returns a name for an "anonymous variable". These don't actually exist in
 * C++, so this macro creates a name based on the given symbol and the current
 * line number where this appears in the file.
 */
#define DRNSF_ANON_VAR(name) DRNSF_CONCAT(name, __LINE__)

// Include the application build configuration. This is partially generated
// by CMake when configuring and generating your build system.
#define DRNSF_BUILDCONFIG_INCLUDED_PROPERLY
#include "drnsf_buildconfig_out.hh"
#undef DRNSF_BUILDCONFIG_INCLUDED_PROPERLY

// Include googletest headers if internal testing was enabled for this build.
#if FEATURE_INTERNAL_TEST
#include "gtest/gtest.h"
#endif

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
    auto DRNSF_ANON_VAR(DRNSF_ON_EXIT__) = ::drnsf::util::on_exit_helper % [&]

/*
 * DRNSF_DECLARE_EMBED
 *
 * FIXME explain
 */
#define DRNSF_DECLARE_EMBED(name) \
    namespace drnsf { \
    namespace embed { \
    namespace name { \
        extern const unsigned char data[]; \
        extern const size_t size; \
        const std::string_view str = { \
            reinterpret_cast<const char *>(data), \
            size \
        }; \
    } \
    } \
    }

namespace drnsf {

/*
 * (various type aliases)
 *
 * The following types are imported from the std namespace for the entire
 * program:
 *
 * size_t:    The type resulting from a sizeof operator.
 *
 * ptrdiff_t: The type resulting from subtracting two pointers.
 *
 * nullptr_t: The type of nullptr. This is used for function overloading with
 *            nullptr.
 *
 * intN_t:    An N-bit signed integer. N can be 8, 16, 32, or 64.
 *
 * uintN_t:   An N-bit unsigned integer, with the same possible values for N.
 *
 * Some or all of these types often end up in the global namespace when using
 * many compilers or libraries, but they are imported here in case they are
 * not.
 */
using std::size_t;
using std::ptrdiff_t;
using std::nullptr_t;
using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;
using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

}

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
inline auto operator "" _fmt(const char *s, size_t sz)
{
    return util::fmt(s);
}

}
