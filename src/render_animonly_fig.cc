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
animonly_fig::animonly_fig(viewport &vp) :
    m_framefig(vp)
{
    m_frame_tracker.on_acquire <<= [this](gfx::frame *frame) {
        m_framefig.set_frame(frame);
    };
    m_frame_tracker.on_lose <<= [this] {
        m_framefig.set_frame(nullptr);
    };
    h_anim_frames_change <<= [this] {
        if (m_anim->get_frames().empty()) {
            m_frame_tracker.set_name(nullptr);
        } else {
            m_frame_tracker.set_name(m_anim->get_frames()[0]);
        }
    };
}

// declared in render.hh
void animonly_fig::show()
{
    m_framefig.show();
}

// declared in render.hh
void animonly_fig::hide()
{
    m_framefig.hide();
}

gfx::anim * const &animonly_fig::get_anim() const
{
    return m_anim;
}

void animonly_fig::set_anim(gfx::anim *anim)
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

const glm::mat4 &animonly_fig::get_matrix() const
{
    return m_framefig.get_matrix();
}

void animonly_fig::set_matrix(glm::mat4 matrix)
{
    m_framefig.set_matrix(matrix);
}

}
}
