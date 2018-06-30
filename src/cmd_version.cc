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
int cmd_version(cmdenv e)
{
    if (e.help_requested) {
        std::cout << R"(Usage:

    drnsf :version

Prints version and license information about the program.
)"
            << std::endl;
        return EXIT_SUCCESS;
    }

    // Check for any arguments. `version' does not take any arguments.
    if (!e.argv.empty()) {
        std::cerr
            << "drnsf version: Too many arguments given.\n\n"
            << "Try: drnsf :help version"
            << std::endl;
        return EXIT_FAILURE;
    }

    std::cout
        << APP_VERSION
        << R"(

DRNSF - An unofficial Crash Bandicoot level editor
Copyright (C) 2017-2018  DRNSF contributors

See the AUTHORS.md file for more details.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License"
along with this program.  If not, see <http://www.gnu.org/licenses/>."
)"
        << std::endl;

    return EXIT_SUCCESS;
}

}
}
