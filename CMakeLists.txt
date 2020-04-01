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

cmake_minimum_required (VERSION 3.12)
project (DRNSF VERSION 0.0)

# Option: FRONTEND
if (WIN32)
    set (DEFAULT_FRONTEND WINAPI)
else ()
    set (DEFAULT_FRONTEND X11)
endif ()
set (FRONTEND "${DEFAULT_FRONTEND}" CACHE STRING
    "User interface frontend"
)
set_property (CACHE FRONTEND PROPERTY STRINGS
    X11
    WINAPI
)
if (FRONTEND STREQUAL X11)
    set (USE_GUI ON)
    set (USE_X11 ON)
    set (USE_GL ON)
elseif (FRONTEND STREQUAL WINAPI)
    set (USE_GUI ON)
    set (USE_WINAPI ON)
    set (USE_GL ON)
    set (CAN_USE_NATIVE_MENU ON)
    set (CAN_USE_NATIVE_FILE_DIALOG ON)
else ()
    message (FATAL_ERROR "Unrecognized FRONTEND value: ${FRONTEND}")
endif ()

# Option: FEATURE_INTERNAL_TEST
set (FEATURE_INTERNAL_TEST ON CACHE BOOL
    "Enable the internal-test command. (requires googletest)"
)
if (FEATURE_INTERNAL_TEST)
    enable_testing ()
    add_test (NAME internal_test COMMAND drnsf ":internal-test")
endif ()

function (forward_native_feature name)
    if (FEATURE_NATIVE_${name} AND CAN_USE_NATIVE_${name})
        set (USE_NATIVE_${name} ON PARENT_SCOPE)
    endif ()
endfunction (forward_native_feature)

# Option: FEATURE_NATIVE_MENU
set (FEATURE_NATIVE_MENU ON CACHE BOOL
    "Use native menus (if available)."
)
forward_native_feature (MENU)

# Option: FEATURE_NATIVE_FILE_DIALOG
set (FEATURE_NATIVE_FILE_DIALOG ON CACHE BOOL
    "Use native file open and save dialogs (if available)."
)
forward_native_feature (FILE_DIALOG)

# Option: AUTOMATIC_DEPS
if (WIN32)
    set (AUTOMATIC_DEPS OFF CACHE BOOL
        "Automatically download and manage DRNSF dependencies."
    )
    if (AUTOMATIC_DEPS)
        include (tools/automatic_deps_win32.cmake)
    endif ()
endif ()

# Option: STATIC_MSVC_RUNTIME
if (MSVC)
    set (STATIC_MSVC_RUNTIME OFF CACHE BOOL
    "Build DRNSF and its dependencies against static MSVC runtimes."
    )
    if (STATIC_MSVC_RUNTIME)
        foreach (fvar IN ITEMS
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL
            CMAKE_CXX_FLAGS_RELWITHDEBINFO
        )
            string (REPLACE "/MD" "/MT" ${fvar} "${${fvar}}")
        endforeach ()
    endif ()
endif ()

add_library (imgui OBJECT
    imgui/imgui.h
    imgui/imconfig.h
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_widgets.cpp
)

add_executable (drnsf
    src/main.cc

    src/common.hh
    src/buildconfig.hh  # processed by configure_file below

    src/core.hh
    src/core.cc
    src/core_main.cc
    src/core_argparser.cc

    src/cmd_help.cc
    src/cmd_version.cc
    src/cmd_gui.cc
    src/cmd_internal_test.cc
    src/cmd_resave_test.cc
    src/cmd_cdxa_imprint.cc
    src/cmd_dump_gl.cc

    src/util.hh
    src/util.cc
    src/util_binreader.cc
    src/util_binwriter.cc
    src/util_file.cc
    src/util_stopwatch.cc

    src/fs.hh

    src/gui.hh
    src/gui.cc
    src/gui_widget.cc
    src/gui_composite.cc
    src/gui_window.cc
    src/gui_popup.cc
    src/gui_widget_gl.cc
    src/gui_widget_2d.cc
    src/gui_widget_im.cc
    src/gui_label.cc
    src/gui_treeview.cc
    src/gui_menu.cc
    src/gui_menubar.cc
    src/gui_file_dialog.cc
    $<$<BOOL:${USE_WINAPI}>:
        src/frontend/winapi/gui_command.cc
        src/frontend/winapi/gui_menu.cc
        src/frontend/winapi/gui_menubar.cc
        src/frontend/winapi/gui_file_dialog.cc
    >

    src/gl.hh
    src/gl.cc
    src/gl_error.cc
    $<$<BOOL:${USE_X11}>:
        src/frontend/x11/gl.cc
    >
    $<$<BOOL:${USE_WINAPI}>:
        src/frontend/winapi/gl.cc
    >

    src/render.hh
    src/render_scene.cc
    src/render_viewport.cc
    src/render_figure.cc
    src/render_reticle_fig.cc
    src/render_frameonly_fig.cc
    src/render_animonly_fig.cc
    src/render_meshframe_fig.cc
    src/render_meshanim_fig.cc
    src/render_model_fig.cc
    src/render_world_fig.cc

    src/edit.hh
    src/edit_context.cc
    src/edit_base_window.cc
    src/edit_mode_menuset.cc
    src/edit_menus.cc
    src/edit_field.cc
    src/edit_asset_metactl.cc
    src/edit_asset_viewctl.cc
    src/edit_asset_propctl.cc
    src/edit_asset_tree.cc
    src/edit_main_window.cc
    src/edit_mode_classic.hh
    src/edit_mode_classic_tree.cc
    src/edit_mode_classic_mainctl.cc
    src/edit_mode_assets.hh
    src/edit_mode_assets_mainctl.cc
    src/edit_mode_map.hh
    src/edit_mode_map_mainctl.cc
    src/edit_mode_window.cc

    src/transact.hh
    src/transact_teller.cc
    src/transact_transaction.cc
    src/transact_nexus.cc

    src/res.hh
    src/res_atom.cc
    src/res_asset.cc

    src/gfx.hh

    src/misc.hh

    src/nsf.hh
    src/nsf_eid.cc
    src/nsf_archive.cc
    src/nsf_spage.cc
    src/nsf_tpage.cc
    src/nsf_entry.cc
    src/nsf_raw_entry.cc
    src/nsf_wgeo_v1.cc
    src/nsf_wgeo_v2.cc

    $<TARGET_OBJECTS:imgui>

    src/embed_data.cc
)
target_include_directories (drnsf PRIVATE "${CMAKE_SOURCE_DIR}/src")
set_property (DIRECTORY PROPERTY VS_STARTUP_PROJECT drnsf)

# Enable C++17
set_target_properties (drnsf PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Dependency: pkg-config
find_package (PkgConfig REQUIRED)

# Dependency: cairo
if (USE_X11)
    pkg_search_module (CAIRO REQUIRED cairo-xlib IMPORTED_TARGET)
    target_link_libraries (drnsf PRIVATE PkgConfig::CAIRO)
elseif (USE_WINAPI)
    pkg_search_module (CAIRO REQUIRED cairo-win32 IMPORTED_TARGET)
    target_link_libraries (drnsf PRIVATE PkgConfig::CAIRO)
endif ()

# Dependency: X11
if (USE_X11)
    find_package (X11 REQUIRED)
    target_include_directories (drnsf PRIVATE ${X11_INCLUDE_DIRS})
    target_link_libraries (drnsf PRIVATE ${X11_LIBRARIES})
endif ()

# Dependency: OpenGL
find_package (OpenGL REQUIRED)
target_link_libraries (drnsf PRIVATE OpenGL::GL)

# Dependency: libepoxy
pkg_search_module (EPOXY REQUIRED epoxy IMPORTED_TARGET)
target_link_libraries (drnsf PRIVATE PkgConfig::EPOXY)

# Dependency: GLM
# (header-only, included as a git submodule)
target_include_directories (drnsf PRIVATE ${CMAKE_SOURCE_DIR}/glm)

# Dependency: Google Test
# (included as a git submodule)
if (FEATURE_INTERNAL_TEST)
    set (BUILD_GTEST ON CACHE INTERNAL
        "Use googletest."
        FORCE)
    set (BUILD_GMOCK OFF CACHE INTERNAL
        "Use googlemock."
        FORCE)
    set (INSTALL_GTEST OFF CACHE INTERNAL
        "Install googletest when installing drnsf."
        FORCE
    )
    set (INSTALL_GMOCK OFF CACHE INTERNAL
        "Install googlemock when installing drnsf."
        FORCE
    )
    if (MSVC)
        set (gtest_force_shared_crt ON CACHE INTERNAL
            "Use non-DLL runtime for googletest."
            FORCE
        )
    endif ()
    add_subdirectory (googletest)
    target_link_libraries (drnsf PRIVATE gtest)
endif()

# Dependency: C++17 Filesystem
if (CMAKE_COMPILER_IS_GNUCXX)
    target_link_libraries (drnsf PRIVATE stdc++fs)
endif ()

# GCC Warnings
if (CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options (drnsf PRIVATE -Wall -Wextra)
    target_compile_options (drnsf PRIVATE -Wno-unused-parameter)
endif ()

# Add directory for generated header/include files
set (GEN_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/gen_include")
file (MAKE_DIRECTORY "${GEN_INCLUDE_DIR}")
target_include_directories (drnsf
    PRIVATE "${GEN_INCLUDE_DIR}"
)

# Process buildconfig.hh
configure_file (
    src/buildconfig.hh
    "${GEN_INCLUDE_DIR}/drnsf_buildconfig_out.hh"
    NEWLINE_STYLE LF
)

# Dependency: Python 3
find_package (Python3 REQUIRED COMPONENTS Interpreter)

# Tool: embedgen
# This command generates the drnsf_embed_data.hh file from the `data/'
# directory so that it can be added to the exectuable.
add_custom_target (embed_data_dir
    COMMAND "${Python3_EXECUTABLE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/tools/embedgen.py"
    "${CMAKE_CURRENT_SOURCE_DIR}/data"
    "${GEN_INCLUDE_DIR}/drnsf_embed_data.hh"
    COMMENT "Packing data/ directory for embedding..."
    SOURCES data/
)
add_dependencies (drnsf embed_data_dir)
