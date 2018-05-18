# DRNSF Automatic Dependencies for Windows #
The DRNSF code relies on several external libraries. Some of these
libraries are included with Windows or the Windows SDK (included with
Visual Studio usually), however some are not. The libraries which are
not can be fairly difficult to install manually on Windows systems.
These include:

 * zlib
 * libpng
 * pixman
 * cairo
 * epoxy

Compounding the issue further, the DRNSF build system locates and
imports (most of) these libraries by using the PkgConfig tool, a
program which is similarly difficult to set up for Windows systems.

To solve this issue, the DRNSF build system includes a mechanism for
automatically downloading and managing these dependencies for you. To
enable this feature, set the CMake cache setting `AUTOMATIC_DEPS` to
`ON` (this appears as a checkbox in cmake-gui).

The automatic dep system will download an archive containing the above
stated libraries and PkgConfig for your target architecture. This
archive is versioned (by date) against your current version of DRNSF,
and verified by SHA512 checksum (matched against a hardcoded value).
The archive's contents are extracted into an architecture- and version-
specific subdirectory of the `autodep` directory in your build folder.
Internally, this is then appended to `CMAKE_PREFIX_PATH`, which will
allow CMake to locate the extracted PkgConfig and thus the extracted
PkgConfig-enabled libraries as well.

You can examine the exact behavior of the automatic dep system in the
`tools/automatic_deps_win32.cmake` build script.

## Handling autodep package version updates ##
During the development of DRNSF, additional libraries may be added as
dependencies. When this happens, a new autodep package will be released
and the auto dependency system will be updated to acquire the new
package. For the most part, this transition should be seamless.

Due to the manner in which CMake caches information about the existing
libraries from the previous autodep package (such as their location),
it is possible that CMake could continue to use libraries from an
older package. Usually this should not be an issue, however if you do
encounter problems with this, you can correct them by clearing your
CMake cache.

Also because of the above issue, if you decide to delete older versions
of the autodep packages or directories, you may also need to clear your
CMake cache as it will continue to attempt to use the (now deleted)
files.

## Disabling the autodep system ##
You can disable the automatic download and extraction of autodep
archives by setting the `AUTOMATIC_DEPS` setting to `OFF` in your CMake
cache. However, the libraries previously located within the archives
will continue to be used as CMake has internally cached their location.
To cease using the autodep libraries, you may need to completely clear
your CMake cache.

## Handling "failed to autodetect" errors ##
Depending on your system or your CMake configuration, you may receive
the following error message when configuring or generating with CMake:

```
Failed to autodetect DRNSF_DEP_ARCH; see docs/build_options.md
```

In this case, the automatic dep system has failed to recognize your
target architecture and is unable to determine which archive to
download or which directory to provide to CMake. You can specify your
architecture manually by adding a `DRNSF_DEP_ARCH` setting to your
CMake cache with one of the following values:

 * `i386` - 32-bit x86 _(not necessarily fully compatible with Intel
 80386)_
 * `x86_64` - 64-bit x86 _(a.k.a. x64, amd64, intel64, IA-32e, etc.)_

Other architectures are not officially supported at this time.

## Manual usage of autodep archives ##
It is possible to utilize the autodep archives yourself. Simply extract
the contents of such an archive into a directory and add the path of
that directory to the `CMAKE_PREFIX_PATH` setting in your CMake cache.
Enabling `AUTOMATIC_DEPS` is unnecessary in this case.

## Building your own autodep archives ##
The autodep archives are built using a MinGW environment with the bash
script provided at `extra/build_win32_deps.sh`. This script is designed
primarily for usage on Debian GNU/Linux systems, however it is likely
usable on a wider range of system setups. Usage information is provided
in comments near the top of the script.

Every tool or library within the archive is built from source retrieved
from the official website or a major mirror thereof and verified by
SHA512 checksum.

Note that, due to differences in toolchain versions, configurations,
build timestamps, etc, any archive produced by your usage of the script
is unlikely to match the hardcoded checksums used for verification.
However, this verification is only applied to downloaded archives. If
you place your own archives into the `autodep` directory with the
proper filename, they will be used without verification.
