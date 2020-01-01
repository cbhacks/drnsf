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

// FIXME explain
int cmd_internal_test(cmdenv e)
{
    if (e.help_requested) {
        std::cout << R"(Usage:

    drnsf :internal-test [<gtest options>]

Runs internal unit tests for DRNSF. This command is intended for use by
developers of DRNSF or for automated build systems.

DRNSF uses GoogleTest. Any options passed after the `:internal-test'
argument will be passed to GoogleTest. For more information about these
options, see the GoogleTest documentation. For a quick overview, run:

    drnsf :internal-test --gtest_help

This command is only available if FEATURE_INTERNAL_TEST was enabled in
the DRNSF build configuration.
)"
            << std::endl;
        return EXIT_SUCCESS;
    }

#if FEATURE_INTERNAL_TEST
    // Initialize google test with the given options.
    int C_argc = int(e.argv.size() + 1);
    auto C_argv = std::make_unique<const char *[]>(C_argc);
    C_argv[0] = "drnsf";
    for (int i = 1; i < C_argc; i++) {
        C_argv[i] = e.argv[i - 1].c_str();
    }
    ::testing::InitGoogleTest(&C_argc, const_cast<char **>(C_argv.get()));
    e.argv = argv_t(&C_argv[1], &C_argv[C_argc]);

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
