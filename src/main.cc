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

#include "common.hh"
#include "core.hh"

/*
 * main, wmain
 *
 * This is the true entry point of the program. This function simply converts
 * the program's arguments into an easier structure and passes execution off
 * to `core::main'.
 */
#ifdef _WIN32
int wmain(int w_argc, wchar_t *w_argv[])
#else
int main(int C_argc, char *C_argv[])
#endif
{
    using namespace drnsf;

    core::cmdenv e;

    // Convert the C-style argument list into a string vector.
#ifdef _WIN32
    for (int i = 0; i < w_argc; i++) {
        e.argv.push_back(util::wstr_to_u8str(w_argv[i]));
    }
#else
    e.argv = core::argv_t(C_argv, C_argv + C_argc);
#endif

    // Discard the first argument, if present. This argument is typically the
    // name of the executable itself.
    if (!e.argv.empty()) {
        e.argv.pop_front();
    }

    return core::main(std::move(e));
}
