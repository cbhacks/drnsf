#
# WILLYMOD - An unofficial Crash Bandicoot level editor
# Copyright (C) 2016  WILLYMOD project contributors
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

add_library (willymod_modules OBJECT
	mod_modctl.cc

	panels/asset_list.cc

	assets_window.cc

	asset_editors/gfx_frame.cc
	asset_editors/gfx_model.cc

	legacy/mod_undo.cc
	legacy/mod_camera_ctrl.cc
	legacy/mod_imgui_demo.cc
	legacy/mod_testbox.cc
)

# Allow includes from /modules root
target_include_directories (willymod_modules PRIVATE ${PROJECT_SOURCE_DIR}/modules)

# Allow includes from /src
target_include_directories (willymod_modules PRIVATE ${PROJECT_SOURCE_DIR}/src)

# Dependency: OpenGL (headers)
find_package (OpenGL REQUIRED)
target_include_directories (willymod_modules PRIVATE ${OPENGL_INCLUDE_DIRS})

# GCC Warnings
if (CMAKE_COMPILER_IS_GNUCXX)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")
endif ()
