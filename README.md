# DRNSF #
__Copyright (C) 2017-2018  DRNSF contributors__

DRNSF is an unofficial level editor for the original Crash Bandicoot
video game trilogy.

## Pre-Release Details ##
__This is a prerelease version of DRNSF.__

This software is not "release-ready" so-to-speak, but is being made
available so that it can be read, observed, critiqued, contributed to,
etc.

## Author ##
See the `AUTHORS.md` file for a list of contributors.

## License ##
DRNSF is made available to you under the terms of the GNU General Public
License as published by the Free Software Foundation, either version 3
of the License, or (at your option) any later version.

DRNSF is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See `LICENSE.txt` for more details.

Some software is included with this version of the DRNSF source code.
This software may be available under other licenses.

For GLM, see `glm/copying.txt`.

For Dear ImGui, see `imgui/LICENSE`.

For Google Test, see `googletest/LICENSE`.

## Building ##
You will need the following dependencies to build DRNSF:

 * CMake >= 3.8
 * PkgConfig
 * Python 3
 * libepoxy _(recommended >= 1.3.1)_
 * Cairo _(recommended >= 1.14.12)_
 * C++17 compiler and toolset; specifically supported are:
   * GCC >= 6.4.0
   * _more to come soon_

Depending on your selected frontend and features, you may need more
dependencies. See `docs/build_options.md` for more details.

Building is done using cmake. This should be done out-of-tree. The
`build/` directory is reserved in `.gitignore` for this purpose so it
will not be included in git commits.

Retrieiving and building drnsf can be done like so:

```sh
/$ git clone https://github.com/cbhacks/drnsf
/$ cd drnsf
/drnsf$ mkdir build
/drnsf$ cd build
/drnsf/build$ cmake ..
/drnsf/build$ make
/drnsf/build$ make test #if desired
```

At this time, DRNSF is not intended to be installed to any system
directories.
