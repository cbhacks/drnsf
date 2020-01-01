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
model_fig::model_fig(scene &scene) :
    meshanim_fig(scene)
{
    m_mesh_tracker.on_acquire <<= [this](gfx::mesh *mesh) {
        set_mesh(mesh);
    };
    m_mesh_tracker.on_lose <<= [this] {
        set_mesh(nullptr);
    };

    m_anim_tracker.on_acquire <<= [this](gfx::anim *anim) {
        set_anim(anim);
    };
    m_anim_tracker.on_lose <<= [this] {
        set_anim(nullptr);
    };

    h_model_mesh_change <<= [this] {
        m_mesh_tracker.set_name(m_model->get_mesh());
    };
    h_model_anim_change <<= [this] {
        m_anim_tracker.set_name(m_model->get_anim());
    };
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

}
}
