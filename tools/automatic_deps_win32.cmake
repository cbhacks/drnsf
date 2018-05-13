#
# DRNSF - An unofficial Crash Bandicoot level editor
# Copyright (C) 2017-2018  DRNSF contributors
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

if (NOT DEFINED DRNSF_DEP_ARCH)
    if ("${CMAKE_GENERATOR}" STREQUAL "Visual Studio 15 2017")
        set (DRNSF_DEP_ARCH "i386")
    elseif ("${CMAKE_GENERATOR}" STREQUAL "Visual Studio 15 2017 Win64")
        set (DRNSF_DEP_ARCH "x86_64")
    else ()
        message (FATAL_ERROR
            "Failed to autodetect DRNSF_DEP_ARCH; see docs/build_options.md"
        )
    endif ()
endif ()

set (DRNSF_DEP_VERSION "20180506")
set (DRNSF_DEP_CHECKSUM_i386   "f40d3d7dc3ff4c1bc2d585928cff3c5031911d398443327fc58ac15d0b81b68c9f033efa0dcee432ff9a4447fca80e9fcbf029f0d35ac4ed6da56659e472e121")
set (DRNSF_DEP_CHECKSUM_x86_64 "bdd541b32a43a1c7178b02b3a505da422f412db731327a69a4c2100540cda3607e31f3300d8338f23bea0758ee0fc52870f0ec2123982194f9c51b75ccb702d1")

file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/autodep")

set (DRNSF_DEP_NAME "drnsf-dep-${DRNSF_DEP_VERSION}-${DRNSF_DEP_ARCH}")
set (DRNSF_DEP_PATH "${CMAKE_BINARY_DIR}/autodep/${DRNSF_DEP_NAME}")
set (DRNSF_DEP_FILE "${DRNSF_DEP_PATH}.tar.xz")

set (DRNSF_DEP_URL "http://dep-files.drnsf.cbhacks.com/${DRNSF_DEP_NAME}.tar.xz")

if (NOT EXISTS "${DRNSF_DEP_PATH}")
    if (NOT EXISTS "${DRNSF_DEP_FILE}")
        if (NOT DEFINED "DRNSF_DEP_CHECKSUM_${DRNSF_DEP_ARCH}")
            message (FATAL_ERROR
                "There is no download available for ${DRNSF_DEP_ARCH}."
            )
        endif ()
        set (DRNSF_DEP_EXPECTED_CHECKSUM
            "${DRNSF_DEP_CHECKSUM_${DRNSF_DEP_ARCH}}"
        )

        set (DRNSF_DEP_TEMPFILE "${CMAKE_BINARY_DIR}/autodep/download_temp")

        file (DOWNLOAD
            "${DRNSF_DEP_URL}"
            "${DRNSF_DEP_TEMPFILE}"
            STATUS DRNSF_DEP_DLSTATUS
        )
        list (GET DRNSF_DEP_DLSTATUS 0 DRNSF_DEP_DLSTATUS_CODE)
        list (GET DRNSF_DEP_DLSTATUS 1 DRNSF_DEP_DLSTATUS_MSG)
        if (NOT "${DRNSF_DEP_DLSTATUS_CODE}" EQUAL 0)
            message (FATAL_ERROR
                "Downloading package from '${DRNSF_DEP_URL}' failed.\n\n"
                "  Message: ${DRNSF_DEP_DLSTATUS_MSG}"
            )
        endif ()

        file (SHA512 "${DRNSF_DEP_TEMPFILE}" DRNSF_DEP_RECEIVED_CHECKSUM)
        if (NOT      "${DRNSF_DEP_EXPECTED_CHECKSUM}"
            STREQUAL "${DRNSF_DEP_RECEIVED_CHECKSUM}")
            message (FATAL_ERROR
                "Downloaded package '${DRNSF_DEP_TEMPFILE}' from "
                "'${DRNSF_DEP_URL}' did not match expected checksum.\n\n"
                "  Expected: '${DRNSF_DEP_EXPECTED_CHECKSUM}'\n"
                "  Received: '${DRNSF_DEP_RECEIVED_CHECKSUM}'\n\n"
                "The download may have been corrupted."
            )
        else ()
            # We perform the file move here instead of after the if block. This
            # ensures that we don't end up unpacking and using a potentially
            # compromised archive if message(FATAL_ERROR) somehow continues or
            # falls through.
            file (RENAME "${DRNSF_DEP_TEMPFILE}" "${DRNSF_DEP_FILE}")
        endif ()
    endif ()

    execute_process (
        COMMAND "${CMAKE_COMMAND}" -E tar xJf "${DRNSF_DEP_FILE}"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/autodep"
    )

    if (NOT EXISTS "${DRNSF_DEP_PATH}")
        message (FATAL_ERROR
            "Unpacking archive '${DRNSF_DEP_FILE}' did not produce "
            "expected directory '${DRNSF_DEP_PATH}'."
        )
    endif ()
endif ()

list (APPEND CMAKE_PREFIX_PATH "${DRNSF_DEP_PATH}")
