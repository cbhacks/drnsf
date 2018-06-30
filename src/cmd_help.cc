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

#include "common.hh"
#include "core.hh"
#include <iostream>

namespace drnsf {
namespace core {

// FIXME explain
int cmd_help(cmdenv e)
{
    std::cout << R"(Usage:

    drnsf [:<subcommand>] [--] [<args>]

The following global options are available. They must appear before any
specified subcommand.

    -h, --help
        Display this message. This is identical to specifying the
        subcommand `help'.

    -v, --version
        Display version and license information. This is identical to
        specifying the subcommand `version'.

The following subcommands are available:

    gui
        Run the application in graphical mode (default).

    help
        Display this message.

    version
        Display version and license information.

    internal-test
        Run internal unit tests.

    resave-test-crash2
        Run resave consistency tests against C2 NSF files.

Example usage:

    drnsf :resave-test-crash2 S000000E.NSF S0000010.NSF

The default subcommand is `gui', which will be used if no subcommand was
specified.
)"
        << std::endl;

    return EXIT_SUCCESS;
}

}
}
