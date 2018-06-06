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
int cmd_help(argv_t argv)
{
    std::cout << R"(Usage:

# To view this message:
    drnsf { -h | --help | :help }

# To display version and license information:
    drnsf { -v | --version | :version }

# Normal usage:
    drnsf [ :subcommand ] [ subcommand options and arguments ]


Available subcommands:

  gui                 Run the application in graphical mode (default)
  help                Display this message
  version             Display version and license information
  internal-test       Runs internal unit tests
  resave-test-crash2  Runs resave consistency tests against C2 NSF files

The default subcommand is `gui', which will be used if no subcommand was
specified.
)"
        << std::endl;

    return EXIT_SUCCESS;
}

}
}
