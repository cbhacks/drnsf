//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2020  DRNSF contributors
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
#include <glm/gtc/matrix_transform.hpp>
#include "render.hh"
#include "gl.hh"

namespace drnsf {
namespace render {

// declared in render.hh
void scene::invalidate()
{
    for (auto &&vp : m_viewports) {
        vp->invalidate();
    }
}

// declared in render.hh
scene::~scene()
{
    while (!m_viewports.empty()) {
        (**m_viewports.begin()).set_scene(nullptr);
    }
}

// declared in render.hh
void scene::draw(const env &e)
{
    for (auto &&fig : m_figs) {
        if (!fig->m_visible)
            continue;

        fig->draw(e);
    }
}

}
}
