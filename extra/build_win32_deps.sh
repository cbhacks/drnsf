#!/bin/bash

#
# DRNSF - An unofficial Crash Bandicoot level editor
# Copyright (C) 2017-2020  DRNSF contributors
#
# See the AUTHORS.md file for more details.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

#
# This script uses MinGW to build the win32 dependency package.
#
# When running this script, you must specify some information via
# environment variables.
#
# ARCH - This must match the DRNSF_DEP_ARCH setting used by the CMake
# scripts for DRNSF. This is a user-specified value, however it may be
# automatically detected as "x86_64" or "i386".
#
# HOST - This is used by the build script to specify your MinGW cross
# compiler toolchain.
#
# Example usage:
#
#   ARCH=x86_64 HOST=x86_64-w64-mingw32 ./build_win32_deps.sh
#
#   ARCH=i386 HOST=i686-w64-mingw32 ./build_win32_deps.sh
#

set -e

# Uncomment a mirror, if desired.
#DRDEP_MIRROR="http://dep-files.drnsf.cbhacks.com/libs/"
#DRDEP_MIRROR="https://github.com/cbhacks/drnsf-dep-files/raw/master/libs/"

if [ -z "${ARCH}" ]; then
    echo 'You must set $ARCH when running this script!'
    echo 'More details can be found in the script comments.'
    exit 1
fi

if [ -z "${HOST+x}" ]; then
    echo 'You must set $HOST when running this script!'
    echo 'You may set it to an empty string if desired.'
    echo 'More details can be found in the script comments.'
    exit 1
fi

echo "Building for ${ARCH} using host: ${HOST}"

DRDEP_TEMP_DIR="$(mktemp -d)"
trap 'rm -rf "${DRDEP_TEMP_DIR}"' EXIT
echo "Operating in temporary directory: ${DRDEP_TEMP_DIR}"

DRDEP_VERSION="20191028"
echo "Building package version: ${DRDEP_VERSION}"

DRDEP_OUT_NAME="drnsf-dep-${DRDEP_VERSION}-${ARCH}"

DRDEP_BUILD_DIR="${DRDEP_TEMP_DIR}/build"
DRDEP_INSTALL_DIR="${DRDEP_TEMP_DIR}/out"
DRDEP_FINAL_DIR="${DRDEP_TEMP_DIR}/${DRDEP_OUT_NAME}"

mkdir -p "${DRDEP_BUILD_DIR}"
mkdir -p "${DRDEP_INSTALL_DIR}"
mkdir -p "${DRDEP_FINAL_DIR}"

export CC="${HOST}-gcc -static-libgcc"
export CXX="${HOST}-g++ -static-libgcc -static-libstdc++"
export CPPFLAGS="-I${DRDEP_INSTALL_DIR}/include"
export LDFLAGS="-L${DRDEP_INSTALL_DIR}/lib -static-libgcc"
export PKG_CONFIG_LIBDIR="${DRDEP_INSTALL_DIR}/lib/pkgconfig"
export PKG_CONFIG_SYSROOT_DIR="${DRDEP_INSTALL_DIR}"

pkg() {
    in="$1"
    out="${2-$1}"
    in_path="${DRDEP_INSTALL_DIR}/${in}"
    out_path="${DRDEP_FINAL_DIR}/${out}"
    mkdir -p "$(dirname "${out_path}")"
    cp "${in_path}" "${out_path}"
}

patch_lib() {
    mkdir patch_lib_dir
    (
        cd patch_lib_dir
        "${HOST}-ar" x "$2"
        for f in *.o
        do
            mv "$f" "$1_$f"
        done
    )
    rm "$2"
    "${HOST}-ar" r "$2" patch_lib_dir/*.o
    rm -rf patch_lib_dir
}

download() {
    upstream_name="$2"
    desired_name="${3-$2}"
    if [ -z "${DRDEP_MIRROR}" ]; then
        wget "$1${upstream_name}" -O "${desired_name}"
    else
        wget "${DRDEP_MIRROR}${desired_name}" -O "${desired_name}"
    fi
}

do_zlib()
{(
    echo "Downloading zlib source..."
    cd "${DRDEP_BUILD_DIR}"
    download "http://zlib.net/" zlib-1.2.11.tar.xz

    echo "Verifying downloaded file with SHA512..."
    echo "b7f50ada138c7f93eb7eb1631efccd1d9f03a5e77b6c13c8b757017b2d462e19d2d3e01c50fad60a4ae1bc86d431f6f94c72c11ff410c25121e571953017cb67  zlib-1.2.11.tar.xz" | sha512sum -c --strict

    echo "Extracting zlib source archive..."
    tar xJf zlib-1.2.11.tar.xz

    echo "Building zlib..."
    cd zlib-1.2.11
    sed "s/PREFIX =/PREFIX = ${HOST}-/" -i win32/Makefile.gcc
    sed "s/SHARED_MODE=0/SHARED_MODE=1/" -i win32/Makefile.gcc
    export prefix=""
    export DESTDIR="${DRDEP_INSTALL_DIR}"
    export BINARY_PATH=/bin
    export LIBRARY_PATH=/lib
    export INCLUDE_PATH=/include
    make -f win32/Makefile.gcc
    make -f win32/Makefile.gcc install
    sed \
        -e 's/^exec_prefix=$/exec_prefix=${prefix}/' \
        -e 's/^libdir=\/lib$/libdir=${exec_prefix}\/lib/' \
        -e 's/^sharedlibdir=\/lib$/sharedlibdir=${exec_prefix}\/lib/' \
        -e 's/^includedir=\/include/includedir=${prefix}\/include/' \
        -i "${DRDEP_INSTALL_DIR}/lib/pkgconfig/zlib.pc"
    patch_lib zlib "${DRDEP_INSTALL_DIR}/lib/libz.dll.a"
    strip "${DRDEP_INSTALL_DIR}/bin/zlib1.dll"

    echo "Packaging zlib binaries..."
    pkg bin/zlib1.dll
    pkg lib/libz.dll.a lib/zlib.lib
    pkg lib/pkgconfig/zlib.pc
    pkg include/zlib.h
    pkg include/zconf.h
)}

do_libpng()
{(
    echo "Downloading libpng source..."
    cd "${DRDEP_BUILD_DIR}"
    download "ftp://ftp-osl.osuosl.org/pub/libpng/src/libpng16/" libpng-1.6.34.tar.xz

    echo "Verifying downloaded file with SHA512..."
    echo "89407c5abc1623faaa3992fc1e4a62def671d9a7401108dfceee895d5f16fe7030090bea89b34a36d377d8e6a5d40046886991f663ce075d1a2d31bf9eaf3c51  libpng-1.6.34.tar.xz" | sha512sum -c --strict

    echo "Extracting libpng source archive..."
    tar xJf libpng-1.6.34.tar.xz

    echo "Building libpng..."
    cd libpng-1.6.34
    ./configure --host="${HOST}" --prefix=""
    make
    DESTDIR="${DRDEP_INSTALL_DIR}" make install
    patch_lib libpng16 "${DRDEP_INSTALL_DIR}/lib/libpng16.dll.a"
    strip "${DRDEP_INSTALL_DIR}/bin/libpng16-16.dll"

    echo "Packaging libpng binaries..."
    pkg bin/libpng16-16.dll
    pkg lib/libpng16.dll.a lib/libpng16.lib
    pkg lib/pkgconfig/libpng16.pc
    pkg lib/pkgconfig/libpng16.pc lib/pkgconfig/libpng.pc
    pkg include/libpng16/png.h
    pkg include/libpng16/pngconf.h
    pkg include/libpng16/pnglibconf.h
)}

do_pixman()
{(
    echo "Downloading pixman source..."
    cd "${DRDEP_BUILD_DIR}"
    download "https://www.cairographics.org/releases/" pixman-0.34.0.tar.gz

    echo "Verifying downloaded file with SHA512..."
    echo "81caca5b71582b53aaac473bc37145bd66ba9acebb4773fa8cdb51f4ed7fbcb6954790d8633aad85b2826dd276bcce725e26e37997a517760e9edd72e2669a6d  pixman-0.34.0.tar.gz" | sha512sum -c --strict

    echo "Extracting pixman source archive..."
    tar xzf pixman-0.34.0.tar.gz

    echo "Building pixman..."
    cd pixman-0.34.0
    ./configure --host="${HOST}" --prefix=""
    make
    DESTDIR="${DRDEP_INSTALL_DIR}" make install
    patch_lib pixman-1 "${DRDEP_INSTALL_DIR}/lib/libpixman-1.dll.a"
    strip "${DRDEP_INSTALL_DIR}/bin/libpixman-1-0.dll"

    echo "Packaging pixman binaries..."
    pkg bin/libpixman-1-0.dll
    pkg lib/libpixman-1.dll.a lib/pixman-1.lib
    pkg lib/pkgconfig/pixman-1.pc
    pkg include/pixman-1/pixman.h
    pkg include/pixman-1/pixman-version.h
)}

do_cairo()
{(
    echo "Downloading cairo source..."
    cd "${DRDEP_BUILD_DIR}"
    download "https://www.cairographics.org/releases/" cairo-1.14.12.tar.xz

    echo "Verifying downloaded file with SHA512..."
    echo "ede6e75ae95605bf5c4b3859b1824cf125c4a2a35a67d38244cc70951ca44b1d4fd7cf622953d3a7cb868fb5b00856ef6d68ee615276b76eec437641290b01e1  cairo-1.14.12.tar.xz" | sha512sum -c --strict

    echo "Extracting cairo source archive..."
    tar xJf cairo-1.14.12.tar.xz

    echo "Building cairo..."
    cd cairo-1.14.12
    ./configure --host="${HOST}" --prefix="" --enable-pdf=no --enable-ps=no --enable-svg=no --enable-script=no --enable-interpreter=no
    make
    DESTDIR="${DRDEP_INSTALL_DIR}" make install
    patch_lib cairo "${DRDEP_INSTALL_DIR}/lib/libcairo.dll.a"
    strip "${DRDEP_INSTALL_DIR}/bin/libcairo-2.dll"

    echo "Packaging cairo binaries..."
    pkg bin/libcairo-2.dll
    pkg lib/libcairo.dll.a lib/cairo.lib
    pkg lib/pkgconfig/cairo.pc
    pkg lib/pkgconfig/cairo-win32.pc
    pkg lib/pkgconfig/cairo-win32-font.pc
    pkg lib/pkgconfig/cairo-png.pc
    pkg include/cairo/cairo.h
    pkg include/cairo/cairo-version.h
    pkg include/cairo/cairo-features.h
    pkg include/cairo/cairo-deprecated.h
    pkg include/cairo/cairo-win32.h
)}

do_epoxy()
{(
    # Using v1.3.1 as used by Debian stable. This version can be built without
    # meson.

    echo "Downloading epoxy sources..."
    cd "${DRDEP_BUILD_DIR}"
    download "https://github.com/anholt/libepoxy/archive/" v1.3.1.tar.gz libepoxy-1.3.1.tar.gz

    echo "Verifying downloaded file with SHA512..."
    echo "7d83f01bbc6d2387e4e2ec8a94b05d6e7aa6d0513821a794d60544381a18874fa75ec1dfd2585824076708e2848ff8d5dc20e184af65a7cb602e7dfd010aaf53  libepoxy-1.3.1.tar.gz" | sha512sum -c --strict

    echo "Extracting epoxy source archive..."
    tar xzf libepoxy-1.3.1.tar.gz

    echo "Building epoxy..."
    cd libepoxy-1.3.1
    ./autogen.sh --host="${HOST}" --prefix=""
    make
    DESTDIR="${DRDEP_INSTALL_DIR}" make install
    patch_lib epoxy "${DRDEP_INSTALL_DIR}/lib/libepoxy.dll.a"
    strip "${DRDEP_INSTALL_DIR}/bin/libepoxy-0.dll"

    echo "Packaging epoxy binaries..."
    pkg bin/libepoxy-0.dll
    pkg lib/libepoxy.dll.a lib/epoxy.lib
    pkg lib/pkgconfig/epoxy.pc
    pkg include/epoxy/gl.h
    pkg include/epoxy/gl_generated.h
    pkg include/epoxy/wgl.h
    pkg include/epoxy/wgl_generated.h
)}

do_pkgconfig()
{(
    echo "Downloading pkg-config sources..."
    cd "${DRDEP_BUILD_DIR}"
    download "https://pkg-config.freedesktop.org/releases/" pkg-config-0.29.2.tar.gz

    echo "Verifying downloaded file with SHA512..."
    echo "4861ec6428fead416f5cbbbb0bbad10b9152967e481d4b0ff2eb396a9f297f552984c9bb72f6864a37dcd8fca1d9ccceda3ef18d8f121938dbe4fdf2b870fe75  pkg-config-0.29.2.tar.gz" | sha512sum -c --strict

    echo "Extracting pkg-config source archive..."
    tar xzf pkg-config-0.29.2.tar.gz

    echo "Building pkg-config..."
    cd pkg-config-0.29.2
    ./configure --host="${HOST}" --prefix="" --with-internal-glib
    make
    DESTDIR="${DRDEP_INSTALL_DIR}" make install

    echo "Packaging pkg-config binaries..."
    pkg bin/pkg-config.exe
)}

do_zlib
do_libpng
do_pixman
do_cairo
do_epoxy
do_pkgconfig

echo "Packaging final tarball..."
tar cJf "${DRDEP_TEMP_DIR}/${DRDEP_OUT_NAME}.tar.xz" -C "${DRDEP_TEMP_DIR}" "${DRDEP_OUT_NAME}"

cp --interactive "${DRDEP_TEMP_DIR}/${DRDEP_OUT_NAME}.tar.xz" "${DRDEP_OUT_NAME}.tar.xz"
