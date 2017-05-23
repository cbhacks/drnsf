# DRNSF #
__Copyright (C) 2017  DRNSF contributors__

DRNSF is an unofficial level editor for the original Crash Bandicoot video game
trilogy.

## Pre-Release Details ##
__This is a prerelease version of DRNSF.__

This software is not "release-ready" so-to-speak, but is being made available
so that it can be read, observed, critiqued, contributed to, etc.

Changes which must be made before the first versioned release:

 * Implement a reasonable property editor (see `edit::asset_propview`)
based on prototype code seen in `src/edit_mode_classic.cc`.
 * Move undo/redo menu options from `src/edit_main_view` into the proper
user interface.
 * Remove the following:
  * Class `edit::main_view`
  * Class `edit::old_editor`
  * Class `edit::pane`
  * Class `edit::mode`
  * Class `edit::modedef`
  * Class `edit::modedef_of<T>`
  * File `src/edit_mode_classic.cc`
 * Implement model/mesh/anim/frame rendering in the new "render" system,
in the class `render::meshframe_fig`. The older rendering code, seen in
`src/module_testbox.cc` uses immediate mode rendering (as CrashEdit did). The
new rendering code should be at or near OpenGL "core" level functionality.
 * Add project-control menu options to new UI:
  * New Project -> Blank
  * New Project -> From NSF (C2)
  * Close Project
  * Undo { name of transaction to be undone }
  * Redo { name of transaction to be redone }
 * Remove the following:
  * Class `edit::core`
  * File `module.hh`
  * File `module_testbox.cc`
  * File `gui_im.cc`
  * Namespace `gui::im`
  * Usage of `gl::old::machine` in `gui::gl_canvas`
  * Namespace `gl::old`
 * Remove imgui.
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

## Contributing ##
Please see `CONTRIBUTING.md` for details on how to style code for contribution.
