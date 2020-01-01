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
#include "render.hh"

namespace drnsf {
namespace render {

// declared in render.hh
figure::figure(scene &scene) :
    m_scene(scene)
{
    scene.m_figs.insert(this);
}

// declared in render.hh
figure::~figure()
{
    m_scene.m_figs.erase(this);

    // Invalidate the scene if this figure is visible, as it has now gone
    // from visible to non-existent.
    if (m_visible) {
        m_scene.invalidate();
    }
}

// declared in render.hh
void figure::invalidate()
{
    if (m_visible) {
        m_scene.invalidate();
    }
}

// declared in render.hh
const bool &figure::get_visible() const
{
    return m_visible;
}

// declared in render.hh
void figure::set_visible(bool visible)
{
    if (visible != m_visible) {
        m_visible = visible;
        m_scene.invalidate();
    }
}

}
}
