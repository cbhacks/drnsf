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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../imgui/imgui.h"
#include "edit.hh"
#include "res.hh"
#include "gfx.hh"

namespace drnsf {

std::function<void(int)> create_mod_testbox(edit::core &);

namespace edit {

core::core(project &proj) :
    m_proj(proj),
    m_ed(m_proj),
    m_wnd(m_ed)
{
    h_frame <<= [this](int width, int height, int delta_time) {
        frame(width, height, delta_time);
    };
    h_frame.bind(m_wnd.m_cryptos.on_frame);

    // Create all of the editor modules.
    m_modules.push_back(create_mod_testbox(*this));

    m_wnd.show();
}

void core::frame(int width, int height, int delta)
{
    // Clear the screen.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up the 3D perspective.
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixf(&glm::perspective(
        70.0f,
        (float)width / height,
        500.0f,
        200000.0f
    )[0][0]);
    glTranslatef(0, 0, -800.0f);
    glTranslatef(0, 0, -g_camera_zoom);
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
