//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2018  DRNSF contributors
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
 * core.hh
 *
 * FIXME explain
 */

#include <deque>

namespace drnsf {
namespace core {

/*
 * core::argv_t
 *
 * This container type is intended to replace the C-style argc/argv structure.
 * It is built using a deque to allow easier removal from the front and middle
 * of the argument list.
 */
using argv_t = std::deque<std::string>;

/*
 * core::cmdenv
 *
 * This structure defines the environment in which a subcommand runs, such as
 * the arguments passed to the command, the environment variables, whether or
 * not --help was specified, etc.
 */
struct cmdenv {
    // (var) argv
    // These are the arguments passed to the subcommand. This is similar to the
    // `argv' passed to `main' by the OS, however there are some differences.
    //
    // For example, if the user runs the program like so:
    //
    //   ./drnsf :foo --bar --etc=1234 -- test
    //
    // Then the value of argv will be { "--bar", "--etc=1234", "--", "test" }
    // when passed to the function for the "foo" subcommand.
    argv_t argv;
};

/*
 * core::main
 *
 * FIXME explain
 */
int main(cmdenv e);

}
}
