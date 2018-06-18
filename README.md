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

If you have received DRNSF in source code form, then it includes some
additional software which is available under other GPL-compatible
licenses.

 * GLM: see `glm/manual.md` section 0.
 * Dear ImGui: see `imgui/LICENSE`.
 * Google Test: see `googletest/LICENSE`.

If you are building the source code on Windows and choose to enable the
automatic dependency system (see `docs/automatic_deps.md`), additional
software will be automatically downloaded and utilized on your behalf.
Some of this software is available under other GPL-compatible licenses:

 * zlib: see `extra/autodep_licenses/zlib.txt`
 * libpng: see `extra/autodep_licenses/libpng.txt`
 * Pixman: see `extra/autodep_licenses/pixman.txt`
 * Cairo: see `extra/autodep_licenses/cairo.txt`
 * libepoxy: see `extra/autodep_licenses/libepoxy.txt`
 * PkgConfig: see `extra/autodep_licenses/pkg-config-with-glib.txt`

## Building ##
You will need the following dependencies to build DRNSF:

 * CMake >= 3.8
 * PkgConfig
 * Python 3
 * libepoxy _(recommended >= 1.3.1)_
 * Cairo _(recommended >= 1.14.12)_
 * C++17 compiler and toolset; specifically supported are:
   * GCC >= 6.3.0
   * Visual Studio 2017 _(requires "Desktop development with C++")_

Depending on your selected frontend and features, you may need more
dependencies. See `docs/build_options.md` for more details.

Building is done using cmake. This should be done out-of-tree. The
`build/` directory is reserved in `.gitignore` for this purpose so it
will not be included in git commits.

Retrieving and building drnsf can be done like so:

```sh
/$ git clone https://github.com/cbhacks/drnsf
/$ cd drnsf
/drnsf$ mkdir build
/drnsf$ cd build
/drnsf/build$ cmake ..
/drnsf/build$ make
/drnsf/build$ make test #if desired
```

If you have Python 2 installed, you may receive an error such as the
following:

> Could NOT find PythonInterp: Found unsuitable version "2.7.15", but
> required is at least "3" (found /usr/bin/python)

In that case, you can add `-DPYTHON_EXECUTABLE="$(which python3)"` to
the cmake command, or use ccmake or cmake-gui to set `PYTHON_EXECUTABLE`
to the path to your Python 3 executable.

At this time, DRNSF is not intended to be installed to any system
directories.
