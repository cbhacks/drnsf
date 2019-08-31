//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2019  DRNSF contributors
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
zone_fig::zone_fig(scene &scene) :
    m_box_fig(scene),
    m_octree_fig(scene)
{
}

game::zone * const &zone_fig::get_zone() const
{
    return m_zone;
}

void zone_fig::set_zone(game::zone *zone)
{
    if (m_zone != zone)
    {
        m_box_fig.set_zone(zone);
        m_octree_fig.set_zone(zone);
    }
}

const glm::mat4 &zone_fig::get_matrix() const
{
    return m_matrix;
}

void zone_fig::set_matrix(glm::mat4 matrix)
{
    if (m_matrix != matrix) {
        m_box_fig.set_matrix(matrix);
        m_octree_fig.set_matrix(matrix);
    }
}

const bool &zone_fig::get_visible() const
{
    return m_visible;
}

void zone_fig::set_visible(bool visible)
{
    if (m_visible != visible) {
        m_box_fig.set_visible(visible);
        m_octree_fig.set_visible(visible);
    }
}

}
}
