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
#include <iostream>
#include <deque>
#include "edit.hh"
#include "gui.hh"
#include "gl.hh"

namespace drnsf {
namespace {

using argv_t = std::deque<std::string>;

static int cmd_help(argv_t argv)
{
    std::cout << R"(Usage:

# To view this message:
    drnsf { -h | --help | :help }

# To display version and license information:
    drnsf { -v | --version | :version }

# Normal usage:
    drnsf [ :subcommand ] [ subcommand options and arguments ]


Available subcommands:

  gui            Run the application in graphical mode (default)
  help           Display this message
  version        Display version and license information
  internal-test  Runs internal unit tests

The default subcommand is `gui', which will be used if no subcommand was
specified.
)"
        << std::endl;

    return EXIT_SUCCESS;
}

static int cmd_version(argv_t argv)
{
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

static int cmd_gui(argv_t argv)
{
    int dummy_argc = 0;
    char *dummy_args[] = { nullptr };
    char **dummy_argv = dummy_args;
    gui::init(dummy_argc, dummy_argv);

    gl::init();
    DRNSF_ON_EXIT { gl::shutdown(); };

    // Create the editor.
    auto proj = std::make_shared<res::project>();
    edit::context ctx(proj);
    edit::core edcore(*proj);

    edit::main_window wnd(ctx);
    wnd.show();
    wnd.set_project(*proj);

    // Run the main application/game loop.
    gui::run();

    return EXIT_SUCCESS;
}

static int cmd_default(argv_t argv)
{
    return cmd_gui(argv);
}

static int cmd_internal_test(argv_t argv)
{
#if FEATURE_INTERNAL_TEST
    // Initialize google test with the given options.
    int C_argc = int(argv.size() + 1);
    auto C_argv = std::make_unique<const char *[]>(C_argc);
    C_argv[0] = "drnsf";
    for (int i = 1; i < C_argc; i++) {
        C_argv[i] = argv[i - 1].c_str();
    }
    ::testing::InitGoogleTest(&C_argc, const_cast<char **>(C_argv.get()));
    argv = argv_t(&C_argv[1], &C_argv[C_argc]);

    // Check for any arguments which weren't consumed by gtest.
    // TODO

    return RUN_ALL_TESTS();
#else
    std::cerr
        << "This feature was disabled in the build configuration."
        << std::endl;

    return EXIT_FAILURE;
#endif
}

const static std::map<std::string, int (*)(argv_t)> s_cmds = {
    { "help", cmd_help },
    { "version", cmd_version },
    { "gui", cmd_gui },
    { "internal-test", cmd_internal_test }
};

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

#if FEATURE_INTERNAL_TEST
// (test) AlwaysPass
// This test should always pass. This is a simple test to see if the testing
// system is building and running, even if it is the only test defined.
TEST(main, AlwaysPass)
{
}
#endif

}
}

/*
 * main
 *
 * This is the actual entry point of the program. Because C++ forbids calling
 * this function or taking its address, most of the functionality is moved into
 * the `drnsf::main' function above.
 */
int main(int C_argc, char *C_argv[])
{
    // Convert the C-style argument list into a string vector.
    drnsf::argv_t argv(C_argv, C_argv + C_argc);

    // Discard the first argument, if present. This argument is typically the
    // name of the executable itself.
    if (!argv.empty()) {
        argv.pop_front();
    }

    return drnsf::main(argv);
}
