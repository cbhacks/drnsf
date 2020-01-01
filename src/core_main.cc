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
#include "gl.hh"

namespace drnsf {
namespace core {

extern int cmd_help(cmdenv e);
extern int cmd_version(cmdenv e);
extern int cmd_gui(cmdenv e);
extern int cmd_internal_test(cmdenv e);
extern int cmd_resave_test(cmdenv e);
extern int cmd_cdxa_imprint(cmdenv e);
extern int cmd_dump_gl(cmdenv e);

// (s-func) cmd_default
// The default subcommand. A pointer to this function is treated differently
// from a pointer to `cmd_gui' by the command line parser in `main'.
static int cmd_default(cmdenv e)
{
    return cmd_gui(std::move(e));
}

// (s-var) g_cmds
// The set of available program subcommands by name.
extern const std::map<std::string, int (*)(cmdenv)> g_cmds = {
    { "help", cmd_help },
    { "version", cmd_version },
    { "gui", cmd_gui },
    { "internal-test", cmd_internal_test },
    { "resave-test", cmd_resave_test },
    { "cdxa-imprint", cmd_cdxa_imprint },
    { "dump-gl", cmd_dump_gl }
};

// declared in core.hh
int main(cmdenv e)
{
    bool ok = true;
    int (*cmd)(cmdenv) = cmd_default;

    argparser o;
    o.add_opt('h', [&]{ cmd = cmd_help; });
    o.add_opt('v', [&]{ if (cmd == cmd_default) cmd = cmd_version; });
    o.alias_opt("help", 'h');
    o.alias_opt("version", 'v');

    argv_t main_argv;

    // Find the subcommand argument (if present). This begins with a colon
    // character.
    auto subcmd_it = std::find_if(
        e.argv.begin(),
        e.argv.end(),
        [](const std::string &s) -> bool {
            return s.size() >= 2 && s[0] == ':';
        }
    );

    // Separate the global program arguments from the subcommand ones.
    if (subcmd_it != e.argv.end()) {
        main_argv = {e.argv.begin(), subcmd_it};
        e.argv.erase(e.argv.begin(), subcmd_it);
    } else {
        main_argv = std::move(e.argv);
        e.argv.clear();
    }

    // Parse the global program arguments.
    try {
        o.begin(main_argv);
        o.end();
    } catch (arg_error ex) {
        std::cerr << "drnsf: " << ex.what() << std::endl;
        ok = false;
    }

    // If there was a subcommand, it is the first element in the remaining argv
    // structure.
    std::string cmdname = "gui";
    if (!e.argv.empty()) {
        cmdname = std::move(e.argv.front());
        e.argv.pop_front();

        // Trim the leading colon character.
        cmdname.erase(0, 1);

        // Find and use the specified subcommand.
        try {
            // If the previously selected command was "help", i.e. because the
            // user specified "-h" or "--help", request help information for
            // the selected subcommand.
            if (cmd == cmd_help) {
                e.help_requested = true;
            }

            cmd = g_cmds.at(cmdname);
        } catch (std::out_of_range) {
            ok = false;
            std::cerr
                << "drnsf: Unknown subcommand: `"
                << cmdname
                << "'."
                << std::endl;
        }
    }

    if (!ok) {
        std::cerr << "\nTry: drnsf --help" << std::endl;
        return EXIT_FAILURE;
    }

    // Execute the subcommand.
    try {
        return cmd(std::move(e));
    } catch (arg_error ex) {
        std::cerr << "drnsf " << cmdname << ": " << ex.what() << std::endl;
        std::cerr << "\nTry: drnsf :help " << cmdname << std::endl;
        return EXIT_FAILURE;
    } catch (gl::error &ex) {
        ex.dump(std::cerr);
        std::cerr << "drnsf " << cmdname << ": " << ex.what() << std::endl;
        std::cerr << "\nAn error occurred related to OpenGL." << std::endl;
        return EXIT_FAILURE;
    }
}

}
}
