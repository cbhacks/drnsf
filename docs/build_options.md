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

## `FEATURE_SCRIPTING` ##
This option determines if the scripting engine will be built.

If ON, Python 3 will be a required runtime dependency. The Limited API
is used to reduce ABI compatibility issues between different versions
of the library on Windows.

If OFF, all scripting capability will be unavailable.

The default is ON.

Regardless of setting, Python 3 is still a required build dependency
to run some Python-based build tools.

## `FEATURE_NATIVE_MENU` ##
This option determines if native menus should be used.

If ON, native menus will be used for the following frontends:

 * WINAPI

For other frontends, a non-native generic fallback will be used.

If OFF, non-native generic menus will be used on all platforms.

The default is ON.

## `FEATURE_NATIVE_FILE_DIALOG` ##
This option determines if native file dialogs should be used.

If ON, native file dialogs will be used for the following frontends:

 * WINAPI

For other frontends, a non-native generic fallback will be used.

If OFF, non-native generic file dialogs will be used on all platforms.

The default is ON.

## `AUTOMATIC_DEPS` ##
_This option is only present on Windows._

This option determines if the CMake build script should automatically
download and manage various dependencies needed for DRNSF.

If ON, the automatic dependency system will be enabled. See
`docs/automatic_deps.md` for more details.

If OFF, the automatic dependency system will not be used.

The default is OFF.

Regardless of setting, you will still need Python 3, as this is not
currently provided in the archive.

### `DRNSF_DEP_ARCH` ###
_This option is only present on Windows._

This option determines which architecture package to retrieve when the
automatic dependency system is enabled. This option is ignored if
`AUTOMATIC_DEPS` is OFF.

Recommended choices are:

 * `i386`
 * `x86_64`

You may also choose another name if desired, however only the above
options are available for automatic download in this version.

There is no default. If this option has not been set, the build system
will attempt to detect the architecture based on the name of the CMake
generator you have selected. If this detection fails, you must provide
your own value.

## `STATIC_MSVC_RUNTIME` ##
_This option is only present for builds using an MSVC generator, such as
Visual Studio 15 2017._

This option determines if the Visual Studio or MSBUILD projects should
link against the static MSVC runtime libraries.

If ON, the project files will link against the "Multi-threaded (/MT)" or
"Multi-threaded Debug (/MTd)" runtime libraries.

If OFF, the project files will link against the default runtime library,
often "Multi-threaded DLL (/MD)" or "Multi-threaded Debug DLL (/MDd)".

The default is OFF.
