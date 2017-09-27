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

cmake_minimum_required (VERSION 3.5)
project (DRNSF)

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

    src/util.hh
    src/util.cc
    src/util_binreader.cc

    src/gui.hh
    src/gui_widget.cc
    src/gui_window.cc
    src/gui_composite.cc
    src/gui_widget_gl.cc
    src/gui_widget_2d.cc
    src/gui_widget_im.cc
    src/gui_label.cc
    src/gui_treeview.cc
    src/gui_gl_canvas.cc
    src/gui_menu.cc
    src/gui_menu_item.cc
    src/gui_im.cc

    src/gl.hh
    src/gl.cc

    src/render.hh
    src/render_viewport.cc
    src/render_figure.cc
    src/render_reticle_fig.cc
    src/render_meshframe_fig.cc

    src/edit.hh
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

    ${CMAKE_CURRENT_BINARY_DIR}/embed_data.cc
)

# Enable C++14
set_target_properties (drnsf PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

# Dependency: pkg-config
find_package (PkgConfig REQUIRED)

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

# GCC Warnings
if (CMAKE_COMPILER_IS_GNUCXX)
    target_compile_options (drnsf PRIVATE -Wall -Wextra)
    target_compile_options (drnsf PRIVATE -Wno-unused-parameter)
endif ()

# Tool: embedgen
# This command generates an embed_data.cc file from the `data/' directory
# so that it can be added to the exectuable.
add_executable (embedgen tools/embedgen.c)
add_custom_target (embed_data_dir
    COMMAND embedgen
    ${CMAKE_CURRENT_SOURCE_DIR}/data
    ${CMAKE_CURRENT_BINARY_DIR}/embed_data.cc
    DEPENDS embedgen
    COMMENT "Packing data/ directory into embed_data.cc..."
    SOURCES data/
)
add_dependencies (drnsf embed_data_dir)
