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
meshanim_fig::meshanim_fig(scene &scene) :
    meshframe_fig(scene)
{
    m_frame_tracker.on_acquire <<= [this](gfx::frame *frame) {
        set_frame(frame);
    };
    m_frame_tracker.on_lose <<= [this] {
        set_frame(nullptr);
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

}
}
