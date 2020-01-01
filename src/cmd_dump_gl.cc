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
#include "gl.hh"
#include "gui.hh"

namespace drnsf {
namespace core {

// FIXME explain
int cmd_dump_gl(cmdenv e)
{
    argparser o;
    o.add_opt("help", [&]{ e.help_requested = true; });
    o.alias_opt('h', "help");
    o.begin(e.argv);
    o.end();

    if (e.help_requested) {
        std::cout << R"(Usage:

    drnsf :dump-gl

Attempts to create an OpenGL context and dumps any relevant information
to standard output.

No options are available.
)"
            << std::endl;
        return EXIT_SUCCESS;
    }

    bool ok = false;
    try {
        int dummy_argc = 0;
        char *dummy_args[] = { nullptr };
        char **dummy_argv = dummy_args;
        gui::init(dummy_argc, dummy_argv);

        gl::init();
        DRNSF_ON_EXIT { gl::shutdown(); };

        ok = true;
        throw gl::error("test error");
    } catch (gl::error &ex) {
        ex.dump(std::cout);
        std::cout
            << "OpenGL initialization was "
            << (ok ? "OK." : "BAD!")
            << std::endl;
        if (!ok) {
            std::cout << "Error message was: " << ex.what();
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

}
}
