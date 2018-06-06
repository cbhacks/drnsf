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

extern int cmd_help(argv_t argv);
extern int cmd_version(argv_t argv);
extern int cmd_gui(argv_t argv);
extern int cmd_internal_test(argv_t argv);
extern int cmd_resave_test_crash2(argv_t argv);

// (s-func) cmd_default
// The default subcommand. A pointer to this function is treated differently
// from a pointer to `cmd_gui' by the command line parser in `main'.
static int cmd_default(argv_t argv)
{
    return cmd_gui(argv);
}

// (s-var) s_cmds
// The set of available program subcommands by name.
const static std::map<std::string, int (*)(argv_t)> s_cmds = {
    { "help", cmd_help },
    { "version", cmd_version },
    { "gui", cmd_gui },
    { "internal-test", cmd_internal_test },
    { "resave-test-crash2", cmd_resave_test_crash2 }
};

// declared in core.hh
int main(argv_t argv)
{
    bool ok = true;
    int (*cmd)(argv_t) = cmd_default;

    // Parse global command-line options which are not part of a subcommand.
    while (!argv.empty() && argv[0][0] == '-') {
        // Stop parsing when the special argument `--' is reached.
        if (argv[0] == "--") {
            break;
        }

        // Check for empty options, such as `drnsf - h' instead of `drnsf -h'.
        if (!argv[0][1]) {
            ok = false;
            std::cerr << "drnsf: Strange empty option `-'." << std::endl;
            argv.pop_front();
            continue;
        }

        // Handle single-character options. These can be separate, like
        // `drnsf -a -b -c' or they can be joined like `drnsf -abc'. This code
        // handles the first in the set and removes it from the set, such that
        // the next loop iteration will handle the second one, and so on.
        if (argv[0][1] != '-') {
            char c = argv[0][1];
            if (!argv[0][2]) {
                // If this is the last option in this single-char option set,
                // remove the set from the argument list. ...
                argv.pop_front();
            } else {
                // ... otherwise, remove this single-char option from the set.
                // The next loop iteration will encounter the next option in the
                // set.
                argv[0].erase(argv[0].begin() + 1);
            }

            switch (c) {
                case 'h':
                    argv.push_front("--help");
                    break;
                case 'v':
                    argv.push_front("--version");
                    break;
                default:
                    ok = false;
                    std::cerr
                        << "drnsf: Unrecognized option: `-"
                        << c
                        << "'."
                        << std::endl;
                    break;
            }

            continue;
        }

        if (argv[0] == "--help") {
            cmd = cmd_help;
            argv.pop_front();
            continue;
        }

        if (argv[0] == "--version") {
            if (cmd != cmd_help) {
                cmd = cmd_version;
            }
            argv.pop_front();
            continue;
        }

        ok = false;
        std::cerr
            << "drnsf: Unrecognized option: `"
            << argv[0]
            << "'."
            << std::endl;
        argv.pop_front();
    }

    // If the first argument begins with `:' and is not just a single lone
    // colon, take the text after the colon and use that as the drnsf subcommand
    // to run.
    if (!argv.empty() && argv[0].size() >= 2 && argv[0][0] == ':') {
        std::string cmdname = { argv[0].begin() + 1, argv[0].end() };
        argv.pop_front();

        if (cmd == cmd_help) {
            std::cerr
                << "Subcommand ignored because -h or --help was specified."
                << std::endl;
        } else if (cmd != cmd_default) {
            std::cerr
                << "A subcommand has already been specified by a previous "
                << "option.\n`:"
                << cmdname
                << "' should be the first argument if you intend to specify a "
                << "subcommand."
                << std::endl;
            ok = false;
        } else {
            try {
                cmd = s_cmds.at(cmdname);
            } catch (std::out_of_range) {
                ok = false;
                std::cerr
                    << "drnsf: Unknown subcommand: `"
                    << cmdname
                    << "'."
                    << std::endl;
            }
        }
    }

    if (!ok) {
        std::cerr << "\nTry: drnsf --help" << std::endl;
        return EXIT_FAILURE;
    }

    return cmd(argv);
}

}
}
