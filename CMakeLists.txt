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
	src/gui_window.cc
	src/gui_im.cc

	src/edit.hh
	src/edit_editor.cc
	src/edit_pane.cc
	src/edit_core.cc
	src/edit_mode.cc
	src/edit_window.cc
	src/edit_mode_classic.cc
	src/edit_mode_imdemo.cc

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
	src/nsf_spage.cc
	src/nsf_raw_entry.cc
	src/nsf_wgeo_v2.cc

	src/module.hh
	src/module_assets_window.cc
	src/module_testbox.cc
	src/module_camera_ctrl.cc

	$<TARGET_OBJECTS:imgui>
)

# Enable C++14
set_target_properties (drnsf PROPERTIES
	CXX_STANDARD 14
	CXX_STANDARD_REQUIRED ON
)

# Dependency: SDL2
find_package (SDL2 REQUIRED)
# Fix an SDL2 bug (error CMP0004).
# https://bugzilla.libsdl.org/show_bug.cgi?id=3295
string (STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)
target_include_directories (drnsf PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries (drnsf PRIVATE ${SDL2_LIBRARIES})

# Dependency: OpenGL
find_package (OpenGL REQUIRED)
target_include_directories (drnsf PRIVATE ${OPENGL_INCLUDE_DIRS})
target_link_libraries (drnsf PRIVATE ${OPENGL_LIBRARIES})

# GCC Warnings
if (CMAKE_COMPILER_IS_GNUCXX)
	target_compile_options (drnsf PRIVATE -Wall -Wextra)
	target_compile_options (drnsf PRIVATE -Wno-unused-parameter)
endif ()
