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
#include "edit.hh"
#include "gui.hh"
#include "gl.hh"

namespace drnsf {
namespace core {

// FIXME explain
int cmd_gui(cmdenv e)
{
    argparser o;
    o.add_opt("help", [&]{ e.help_requested = true; });
    o.alias_opt('h', "help");
    o.begin(e.argv);
    o.end();

    if (e.help_requested) {
        std::cout << R"(Usage:

    drnsf [:gui]

Runs the program in GUI mode, as most users would normally expect. This
is the default subcommand used if none is given.

At this time, no arguments are accepted by this subcommand. This may
change in the future.
)"
            << std::endl;
        return EXIT_SUCCESS;
    }

    int dummy_argc = 0;
    char *dummy_args[] = { nullptr };
    char **dummy_argv = dummy_args;
    gui::init(dummy_argc, dummy_argv);

    gl::init();
    DRNSF_ON_EXIT { gl::shutdown(); };

    // Create the editor.
    edit::context ctx;

    edit::main_window wnd(ctx);
    wnd.show();

    // Run the main application/game loop.
    gui::run();

    return EXIT_SUCCESS;
}

}
}
