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
#include <iostream>

namespace drnsf {
namespace core {

// defined in core.cc
extern const std::map<std::string, int (*)(cmdenv)> g_cmds;

// FIXME explain
int cmd_help(cmdenv e)
{
    argparser o;
    o.add_opt("help", [&]{ e.help_requested = true; });
    o.alias_opt('h', "help");
    o.begin(e.argv);

    if (e.help_requested) {
        std::cout << R"(Usage:

    drnsf :help [<subcommand> | :<subcommand>]

This subcommand prints usage information about the program, or usage
information about a particular subcommand (if specified).

Example usage:

    # Print information about DRNSF.
    drnsf :help

    # Print information about the DRNSF internal-test subcommand.
    drnsf :help internal-test

    # The option --help is available globally as well as within each
    # subcommand. This has the same effect as the help subcommand.
    drnsf --help
    drnsf --help :internal-test
    drnsf :internal-test --help
)"
            << std::endl;
        return EXIT_SUCCESS;
    }

    // Check to see if an argument is present. If so, find it as a subcommmand
    // and display help for that command.
    if (!o.pump_eof()) {
        std::string cmdname;
        o >> cmdname;
        o.end();

        // Check if the argument is empty or is a nameless subcommand.
        if (cmdname.empty() || cmdname == ":") {
            std::cerr
                << "drnsf help: Invalid argument.\n\n"
                << "Try: drnsf :help help"
                << std::endl;
            return EXIT_FAILURE;
        }

        // Remove the leading colon if it exists. The subcommand table does not
        // include the colon in the subcommand name.
        if (cmdname[0] == ':') {
            cmdname.erase(0, 1);
        }

        // Lookup the subcommand. If not found, fail with an error message.
        auto it = g_cmds.find(cmdname);
        if (it == g_cmds.end()) {
            std::cerr
                << "drnsf help: Unknown subcommand: `"
                << cmdname
                << "'.\n\n"
                << "Try: drnsf :help"
                << std::endl;
            return EXIT_FAILURE;
        }

        // Call the given subcommand to have it print help information.
        e.help_requested = true;
        return it->second(std::move(e));
    }

    o.end();

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
        Display this message, or information about a given subcommand.

    version
        Display version and license information.

    internal-test
        Run internal unit tests.

    resave-test
        Run resave consistency tests against NSF files.

    cdxa-imprint
        Overwrite the system info section of CD-XA BIN disc images.

    dump-gl
        Dump information about your OpenGL implementation, if possible.

Example usage:

    drnsf :resave-test --c2 S000000E.NSF S0000010.NSF

The default subcommand is `gui', which will be used if no subcommand was
specified.

The `help' subcommand can also show information about a specific
subcommand. See `drnsf :help help' for more details.
)"
        << std::endl;

    return EXIT_SUCCESS;
}

}
}
