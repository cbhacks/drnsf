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

namespace drnsf {
namespace render {

// declared in render.hh
world_fig::world_fig(scene &scene) :
    model_fig(scene)
{
    m_model_tracker.on_acquire <<= [this](gfx::model *model) {
        set_model(model);
    };
    m_model_tracker.on_lose <<= [this] {
        set_model(nullptr);
    };

    h_world_model_change <<= [this] {
        m_model_tracker.set_name(m_world->get_model());
    };
    h_world_x_change <<= [this] {
        model_fig::set_matrix(glm::translate(m_matrix, {
            m_world->get_x(),
            m_world->get_y(),
            m_world->get_z()
        }));
    };
    h_world_y_change <<= [this] {
        model_fig::set_matrix(glm::translate(m_matrix, {
            m_world->get_x(),
            m_world->get_y(),
            m_world->get_z()
        }));
    };
    h_world_z_change <<= [this] {
        model_fig::set_matrix(glm::translate(m_matrix, {
            m_world->get_x(),
            m_world->get_y(),
            m_world->get_z()
        }));
    };
}

// declared in render.hh
gfx::world * const &world_fig::get_world() const
{
    return m_world;
}

// declared in render.hh
void world_fig::set_world(gfx::world *world)
{
    if (m_world != world)
    {
        if (m_world) {
            h_world_model_change.unbind();
            h_world_x_change.unbind();
            h_world_y_change.unbind();
            h_world_z_change.unbind();
            m_model_tracker.set_name(nullptr);
        }
        m_world = world;
        if (m_world) {
            h_world_model_change.bind(m_world->p_model.on_change);
            h_world_x_change.bind(m_world->p_x.on_change);
            h_world_y_change.bind(m_world->p_y.on_change);
            h_world_z_change.bind(m_world->p_z.on_change);
            m_model_tracker.set_name(m_world->get_model());
            model_fig::set_matrix(glm::translate(m_matrix, {
                m_world->get_x(),
                m_world->get_y(),
                m_world->get_z()
            }));
        }
    }
}

// declared in render.hh
const glm::mat4 &world_fig::get_matrix() const
{
    return m_matrix;
}

// declared in render.hh
void world_fig::set_matrix(glm::mat4 matrix)
{
    m_matrix = matrix;

    if (m_world) {
        model_fig::set_matrix(glm::translate(matrix, {
            m_world->get_x(),
            m_world->get_y(),
            m_world->get_z()
        }));
    }
}

}
}
