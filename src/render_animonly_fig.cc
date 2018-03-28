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
    h_asset_appear <<= [this](res::asset &asset) {
        if (m_anim->get_frames().empty())
            return;
        auto frame_ref = m_anim->get_frames()[0];
        if (frame_ref != asset.get_name())
            return;
        if (!frame_ref.ok())
            return;
        m_framefig.set_frame(&static_cast<gfx::frame &>(asset));
    };
    h_asset_disappear <<= [this](res::asset &asset) {
        if (m_anim->get_frames().empty())
            return;
        if (m_anim->get_frames()[0] != asset.get_name())
            return;
        m_framefig.set_frame(nullptr);
    };
    h_anim_frames_change <<= [this] {
        m_framefig.set_frame(nullptr);

        if (m_anim->get_frames().empty())
            return;
        auto frame_ref = m_anim->get_frames()[0];
        if (!frame_ref)
            return;
        auto frame = frame_ref.get();
        if (!frame)
            return;
        m_framefig.set_frame(frame);
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
            h_asset_appear.unbind();
            h_asset_disappear.unbind();
            h_anim_frames_change.unbind();
            m_framefig.set_frame(nullptr);
        }
        m_anim = anim;
        if (m_anim) {
            h_asset_appear.bind(m_anim->get_proj().on_asset_appear);
            h_asset_disappear.bind(m_anim->get_proj().on_asset_disappear);
            h_anim_frames_change.bind(m_anim->p_frames.on_change);
            if (!m_anim->get_frames().empty()) {
                auto frame = m_anim->get_frames()[0].get();
                if (frame) {
                    m_framefig.set_frame(frame);
                }
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
