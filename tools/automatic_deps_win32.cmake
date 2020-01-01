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

if (NOT DEFINED DRNSF_DEP_ARCH)
    set (DRNSF_SUPPORTED_ARCHES
        "i386"
        "x86_64"
    )

    foreach (DRNSF_ATTEMPTED_ARCH IN LISTS DRNSF_SUPPORTED_ARCHES)
        try_run (
            DRNSF_ARCH_RUN_RESULT
            DRNSF_ARCH_COMPILE_RESULT
            "${CMAKE_BINARY_DIR}"
            "${CMAKE_SOURCE_DIR}/tools/test_arch_${DRNSF_ATTEMPTED_ARCH}.c"
        )
        if ("${DRNSF_ARCH_COMPILE_RESULT}")
            set (DRNSF_DEP_ARCH "${DRNSF_ATTEMPTED_ARCH}")
            break ()
        endif ()
    endforeach()

    if (NOT DEFINED DRNSF_DEP_ARCH)
        message (FATAL_ERROR
            "Failed to autodetect DRNSF_DEP_ARCH; see docs/build_options.md"
        )
    endif ()
endif ()

set (DRNSF_DEP_VERSION "20191028")
set (DRNSF_DEP_CHECKSUM_i386   "1d142ee46cff151e3fb2c12de90cbb9c639c4e7c593d9c7de6fbc213d64cbdb9173efecf8942cf41659b0746d1ed6680cff456b98b77b63e59f19e09db4b9908")
set (DRNSF_DEP_CHECKSUM_x86_64 "de3199c5f7f09daa8dea0b90cc2829a0b1a9575315bfa28d7efcd37670f3d46dc3333fc55c7376e1ea366a2d261f1f0d4ec82c87ce6761ad758e27b2afe84bc7")

set (DRNSF_DEP_COPYFILES
    "libcairo-2.dll"
    "libepoxy-0.dll"
    "libpixman-1-0.dll"
    "libpng16-16.dll"
    "zlib1.dll"
)

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

if (CMAKE_GENERATOR MATCHES "^Visual Studio [0-9]+")
    foreach (DRNSF_DEP_CONFIG IN LISTS CMAKE_CONFIGURATION_TYPES)
        foreach (DRNSF_DEP_COPYFILE IN LISTS DRNSF_DEP_COPYFILES)
            file (MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/${DRNSF_DEP_CONFIG}")
            file (COPY
                "${DRNSF_DEP_PATH}/bin/${DRNSF_DEP_COPYFILE}"
                DESTINATION "${CMAKE_BINARY_DIR}/${DRNSF_DEP_CONFIG}/"
            )
        endforeach ()
    endforeach ()
endif ()
