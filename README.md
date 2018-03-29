# DRNSF #
__Copyright (C) 2017-2018  DRNSF contributors__

DRNSF is an unofficial level editor for the original Crash Bandicoot video game
trilogy.

## Pre-Release Details ##
__This is a prerelease version of DRNSF.__

This software is not "release-ready" so-to-speak, but is being made available
so that it can be read, observed, critiqued, contributed to, etc.

Changes which must be made before the first versioned release:

 * Implement a reasonable property editor (see `edit::asset_propview`)
based on prototype code seen in `src/edit_mode_classic.cc`.
 * Remove the following:
   * Class `edit::main_view`
   * Class `edit::old_editor`
   * Class `edit::pane`
   * Class `edit::mode`
   * Class `edit::modedef`
   * Class `edit::modedef_of<T>`
   * File `src/edit_mode_classic.cc`
 * Add project-control menu options to new UI:
   * Close Project
 * Remove the following:
   * Class `edit::core`
   * File `gui_im.cc`
   * Namespace `gui::im`
   * Class `edit::im_canvas`
 * Fix unused variable warnings in `nsf::wgeo_v2::import_entry`. This will
require adding more fields to various asset types to store this data so it can
be saved out later, as well as modified by the user.
 * Add menu options:
   * Export NSF
   * Export NSF As...

Note that this is a general outline and could be missing some important steps.

Essentially, for the first release (v1), the following should be possible:

 * Open NSF file (crash 2)
 * Edit scenery colors and vertex positions
 * Save NSF file (crash 2)

After this point, development can focus on supporting all kinds of entry
formats. The sky is the limit (?).

## Author ##
See the `AUTHORS.md` file for a list of contributors.

## License ##
DRNSF is made available to you under the terms of the GNU General Public
License as published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

DRNSF is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See `LICENSE.txt` for more details.

Some software is included with this version of the DRNSF source code. This
software may be available under other licenses.

For GLM, see `glm/copying.txt`.

For imgui, see `imgui/LICENSE`.

## Building ##
You will need the following dependencies to build DRNSF:

 * CMake >= 3.8
 * PkgConfig
 * GTK+3
 * libepoxy
 * OpenGL

You will also need a C++17 compiler and standard library.

Building is done using cmake. This should be done out-of-tree. The `build/`
directory is reserved in `.gitignore` for this purpose so it will not be
included in git commits.

For now, you may need to run with something to limit the OpenGL function level
to 3.0. With mesa, you can use `MESA_GL_VERSION_OVERRIDE=3.0` as an environment
variable.

## Contributing ##

### Licensing and Attribution ###
Ensure any code you contribute is something you have the right to distribute
under the terms of the GNU General Public License version 3.

Add your name to the `AUTHORS.md` file both to provide attribution for yourself
and to certify that you grant access to and usage of your contribution(s) under
the terms of the GNU General Public License version 3 as stated above.

### Styling ###
Please follow these guidelines for code style when contributing to DRNSF:

 * Indent using 4 space characters; no tabs. Do not indent the contents of
namespaces.
 * Place opening braces on the same line for statements, type definitions,
namespaces, etc. Place closing braces on the same line as `while` for
`do...while` loops. Place both closing and opening braces on the same line for
`else` and `else if`, such as `} else if (foo) {`.
 * Place opening braces for functions (static, member, global, etc) on their
own line. This is vaguely similar to Linux formatting guidelines.
 * Avoid exceeding 80 columns in width. Hardwrap if necessary.
 * Use UNIX-style line-endings (LF, not CRLF or CR).
 * Ensure there is a blank line at the end of every source and header file.
 * When two comma-separated values exist on the same line, place a space after
the comma.

These are only guidelines. Following them keeps the code clean and consistent,
but try not to let them get in the way of contribution. Misformatted code can
always be changed afterwards.

### Submitting ###
You can submit your pull request using the GitHub repository's pull requests
page.
