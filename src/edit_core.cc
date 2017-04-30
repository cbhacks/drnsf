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
#include "../imgui/imgui.h"
#include "edit.hh"
#include "res.hh"
#include "gfx.hh"

namespace drnsf {

std::function<void(int)> create_mod_testbox(edit::core &);

namespace edit {

core::core() :
	m_ed(m_proj),
	m_wnd(m_ed)
{
	h_frame <<= [this](int width,int height,int delta_time) {
		frame(width,height,delta_time);
	};
	h_frame.bind(m_wnd.m_cryptos.on_frame);

	// Create all of the editor modules.
	m_modules.push_back(create_mod_testbox(*this));

	m_wnd.show();
}

void core::frame(int width,int height,int delta)
{
	// Clear the screen.
	glClearColor(0.5,0.5,0.5,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Calculate the normalized display size.
	double norm_width = 1;
	double norm_height = (double)height / width;
	if (norm_height < 1) {
		norm_width /= norm_height;
		norm_height = 1;
	}

	// Set up the 3D perspective.
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glFrustum(
		-norm_width * g_camera_zoom,
		+norm_width * g_camera_zoom,
		-norm_height * g_camera_zoom,
		+norm_height * g_camera_zoom,
		1.8f,
		200.0f
	);
	glTranslatef(0,0,-1.8f);
	glTranslatef(0,0,-3.6f);
	glMatrixMode(GL_MODELVIEW);

	// Enable z-buffering.
	glEnable(GL_DEPTH_TEST);

	// Run all of the enabled editor modules.
	for (auto &&mod : m_modules) {
		mod(delta);
	}

	// Disable the previously-enabled z-buffering.
	glDisable(GL_DEPTH_TEST);

	// Restore the default projection matrix.
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

}
}
