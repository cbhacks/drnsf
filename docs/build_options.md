# DRNSF Build Options #
The following build options may be configured using CMake tools such as
`cmake`, `ccmake`, or the CMake GUI.

## `FRONTEND` ##
This option determines the user interface frontend to use.

Valid choices are:

 * `X11` - libX11 with GLX
 * `WINAPI` - Windows API with WGL

The default is `WINAPI` for windows, or `X11` for other platforms.

## `FEATURE_INTERNAL_TEST` ##
This option determines if internal unit tests should be built.

If ON, Google Test will be a required dependency. This is included as a
Git submodule.

If OFF, the `internal-test` command will not be available.

The default is ON.
