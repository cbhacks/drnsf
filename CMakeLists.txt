#
# DRNSF - An unofficial Crash Bandicoot level editor
# Copyright (C) 2017  DRNSF contributors
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

cmake_minimum_required (VERSION 3.8)
project (DRNSF VERSION 0.0)

add_library (imgui OBJECT
    imgui/imgui.h
    imgui/imconfig.h
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_demo.cpp
)

add_executable (drnsf
    src/main.cc

    src/common.hh
    src/buildconfig.hh  # processed by configure_file below

    src/util.hh
    src/util.cc
    src/util_binreader.cc

    src/fs.hh

    src/gui.hh
    src/gui.cc
    src/gui_widget.cc
    src/gui_window.cc
    src/gui_popup.cc
    src/gui_composite.cc
    src/gui_widget_gl.cc
    src/gui_widget_2d.cc
    src/gui_widget_im.cc
    src/gui_label.cc
    src/gui_treeview.cc
    src/gui_menu.cc
    src/gui_menubar.cc
    src/gui_dialogs.cc
    src/gui_gl_canvas.cc
    src/gui_im.cc

    src/gl.hh
    src/gl.cc

    src/render.hh
    src/render_viewport.cc
    src/render_figure.cc
    src/render_reticle_fig.cc
    src/render_meshframe_fig.cc

    src/edit.hh
    src/edit_editor2.cc
    src/edit_menus.cc
    src/edit_asset_editor.cc
    src/edit_asset_tree.cc
    src/edit_asset_infoview.cc
    src/edit_asset_propview.cc
    src/edit_asset_viewport.cc
    src/edit_main_window.cc
    src/edit_im_canvas.cc
    src/edit_editor.cc
    src/edit_pane.cc
    src/edit_core.cc
    src/edit_mode.cc
    src/edit_mode_classic.cc
    src/edit_main_view.cc

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
    src/nsf_archive.cc
    src/nsf_spage.cc
    src/nsf_raw_entry.cc
    src/nsf_wgeo_v2.cc

    src/module_testbox.cc

    $<TARGET_OBJECTS:imgui>

    src/embed_data.cc
)

# Enable C++17
set_target_properties (drnsf PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# Dependency: pkg-config
find_package (PkgConfig REQUIRED)

# Dependency: cairo
pkg_search_module (CAIRO REQUIRED cairo)
target_include_directories (drnsf PRIVATE ${CAIRO_INCLUDE_DIRS})
target_link_libraries (drnsf PRIVATE ${CAIRO_LIBRARIES})
target_compile_options (drnsf PRIVATE ${CAIRO_CFLAGS_OTHER})

# Dependency: GTK+ 3.0
pkg_search_module (GTK3 REQUIRED gtk+-3.0)
target_include_directories (drnsf PRIVATE ${GTK3_INCLUDE_DIRS})
target_link_libraries (drnsf PRIVATE ${GTK3_LIBRARIES})
target_compile_options (drnsf PRIVATE ${GTK3_CFLAGS_OTHER})

# Dependency: OpenGL
find_package (OpenGL REQUIRED)
target_include_directories (drnsf PRIVATE ${OPENGL_INCLUDE_DIRS})
target_link_libraries (drnsf PRIVATE ${OPENGL_LIBRARIES})

# Dependency: libepoxy
pkg_search_module (EPOXY REQUIRED epoxy)
target_include_directories (drnsf PRIVATE ${EPOXY_INCLUDE_DIRS})
target_link_libraries (drnsf PRIVATE ${EPOXY_LIBRARIES})
target_compile_options (drnsf PRIVATE ${EPOXY_CFLAGS_OTHER})

# Dependency: GLM
# (header-only, included as a git submodule)
target_include_directories (drnsf PRIVATE ${CMAKE_SOURCE_DIR}/glm)

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
find_package (PythonInterp 3 REQUIRED)

# Tool: embedgen
# This command generates the drnsf_embed_data.hh file from the `data/'
# directory so that it can be added to the exectuable.
add_custom_target (embed_data_dir
    COMMAND "${PYTHON_EXECUTABLE}"
    "${CMAKE_CURRENT_SOURCE_DIR}/tools/embedgen.py"
    "${CMAKE_CURRENT_SOURCE_DIR}/data"
    "${GEN_INCLUDE_DIR}/drnsf_embed_data.hh"
    COMMENT "Packing data/ directory for embedding..."
    SOURCES data/
)
add_dependencies (drnsf embed_data_dir)
