//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2018  DRNSF contributors
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
model_fig::model_fig(viewport &vp) :
    m_meshanimfig(vp)
{
    m_mesh_tracker.on_acquire <<= [this](gfx::mesh *mesh) {
        m_meshanimfig.set_mesh(mesh);
    };
    m_mesh_tracker.on_lose <<= [this] {
        m_meshanimfig.set_mesh(nullptr);
    };

    m_anim_tracker.on_acquire <<= [this](gfx::anim *anim) {
        m_meshanimfig.set_anim(anim);
    };
    m_anim_tracker.on_lose <<= [this] {
        m_meshanimfig.set_anim(nullptr);
    };

    h_model_mesh_change <<= [this] {
        m_mesh_tracker.set_name(m_model->get_mesh());
    };
    h_model_anim_change <<= [this] {
        m_anim_tracker.set_name(m_model->get_anim());
    };
}

// declared in render.hh
void model_fig::show()
{
    m_meshanimfig.show();
}

// declared in render.hh
void model_fig::hide()
{
    m_meshanimfig.hide();
}

// declared in render.hh
gfx::model * const &model_fig::get_model() const
{
    return m_model;
}

// declared in render.hh
void model_fig::set_model(gfx::model *model)
{
    if (m_model != model)
    {
        if (m_model) {
            h_model_mesh_change.unbind();
            h_model_anim_change.unbind();
            m_mesh_tracker.set_name(nullptr);
            m_anim_tracker.set_name(nullptr);
        }
        m_model = model;
        if (m_model) {
            h_model_mesh_change.bind(m_model->p_mesh.on_change);
            h_model_anim_change.bind(m_model->p_anim.on_change);
            m_mesh_tracker.set_name(m_model->get_mesh());
            m_anim_tracker.set_name(m_model->get_anim());
        }
    }
}

// declared in render.hh
const glm::mat4 &model_fig::get_matrix() const
{
    return m_meshanimfig.get_matrix();
}

// declared in render.hh
void model_fig::set_matrix(glm::mat4 matrix)
{
    m_meshanimfig.set_matrix(matrix);
}

}
}
