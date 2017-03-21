//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017  DRNSF contributors
//
// See the AUTHORS.md file for more details.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "common.hh"
#include <SDL.h>
#include <GL/gl.h>
#include "../imgui/imgui.h"
#include "edit.hh"
#include "res.hh"
#include "gfx.hh"

#define DISPLAYWIDTH 800
#define DISPLAYHEIGHT 600

namespace edit {

core::core() :
	window("DRNSF",DISPLAYWIDTH,DISPLAYHEIGHT)
{
	// Create all of the editor modules.
	for (auto &&info : module_info::get_set()) {
		m_modules[info] = info->create(*this);
	}

	m_wnd.m_proj = m_proj;
	m_wnd.m_ed = std::make_shared<editor>(*m_proj);
}

void core::frame(int delta)
{
	// Inform all of the modules if this is the first frame. FIXME doesn't work anymore
	if (m_firstframe) {
		for (auto &&kv : m_modules) {
			auto &&mod = kv.second;
			mod->firstframe();
		}
		m_firstframe = false;
	}

	// Clear the screen.
	glClearColor(0.5,0.5,0.5,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Calculate the normalized display size.
	double norm_width = 1;
	double norm_height = (double)get_height() / get_width();
	if (norm_height < 1) {
		norm_width /= norm_height;
		norm_height = 1;
	}

	// Set up the 3D perspective.
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glFrustum(
		-norm_width * m_cam.lens_focus * m_cam.lens_near,
		+norm_width * m_cam.lens_focus * m_cam.lens_near,
		-norm_height * m_cam.lens_focus * m_cam.lens_near,
		+norm_height * m_cam.lens_focus * m_cam.lens_near,
		m_cam.lens_near,
		m_cam.lens_far
	);
	glTranslatef(0,0,-m_cam.lens_near);
	glTranslatef(0,0,-m_cam.lens_adjust);
	glMatrixMode(GL_MODELVIEW);

	// Enable z-buffering.
	glEnable(GL_DEPTH_TEST);

	// Run the core GUI menus.
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("View")) {
			for (auto &&kv : m_modules) {
				auto &&mod = kv.second;
				mod->show_view_menu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	// Run all of the enabled editor modules.
	for (auto &&kv : m_modules) {
		auto &&mod = kv.second;
		mod->frame(delta);
	}

	// Disable the previously-enabled z-buffering.
	glDisable(GL_DEPTH_TEST);

	// Restore the default projection matrix.
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

const decltype(core::m_modules) &core::get_modules() const
{
	return m_modules;
}

}
