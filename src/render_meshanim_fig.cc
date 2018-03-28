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
meshanim_fig::meshanim_fig(viewport &vp) :
    m_meshframefig(vp)
{
    h_frame_acquire <<= [this](gfx::frame *frame) {
        m_meshframefig.set_frame(frame);
    };
    h_frame_acquire.bind(m_frame_tracker.on_acquire);
    h_frame_lose <<= [this] {
        m_meshframefig.set_frame(nullptr);
    };
    h_frame_lose.bind(m_frame_tracker.on_lose);
    h_anim_frames_change <<= [this] {
        if (m_anim->get_frames().empty()) {
            m_frame_tracker.set_name(nullptr);
        } else {
            m_frame_tracker.set_name(m_anim->get_frames()[0]);
        }
    };
}

// declared in render.hh
void meshanim_fig::show()
{
    m_meshframefig.show();
}

// declared in render.hh
void meshanim_fig::hide()
{
    m_meshframefig.hide();
}

// declared in render.hh
gfx::mesh * const &meshanim_fig::get_mesh() const
{
    return m_meshframefig.get_mesh();
}

// declared in render.hh
void meshanim_fig::set_mesh(gfx::mesh *mesh)
{
    m_meshframefig.set_mesh(mesh);
}

// declared in render.hh
gfx::anim * const &meshanim_fig::get_anim() const
{
    return m_anim;
}

// declared in render.hh
void meshanim_fig::set_anim(gfx::anim *anim)
{
    if (m_anim != anim)
    {
        if (m_anim) {
            h_anim_frames_change.unbind();
            m_frame_tracker.set_name(nullptr);
        }
        m_anim = anim;
        if (m_anim) {
            h_anim_frames_change.bind(m_anim->p_frames.on_change);
            if (!m_anim->get_frames().empty()) {
                m_frame_tracker.set_name(m_anim->get_frames()[0]);
            }
        }
    }
}

// declared in render.hh
const glm::mat4 &meshanim_fig::get_matrix() const
{
    return m_meshframefig.get_matrix();
}

// declared in render.hh
void meshanim_fig::set_matrix(glm::mat4 matrix)
{
    m_meshframefig.set_matrix(matrix);
}

}
}
