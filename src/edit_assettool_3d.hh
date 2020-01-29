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

#pragma once

/*
 * edit_assettool_3d.hh
 *
 * FIXME explain
 */

#include "edit.hh"

namespace drnsf {
namespace edit {
namespace assettool_3d {

/*
 * edit::assettool_3d::vertex_ctl
 *
 * FIXME explain
 */
struct vertex_ctl : gui::widget_im {
    using widget_im::widget_im;

    void frame() override;
};

/*
 * edit::assettool_3d::tool
 *
 * FIXME explain
 */
class tool : public assettool {
private:
    // (var) m_vp
    // The 3D viewport which the various model editing subtools work with.
    render::viewport m_vp {*this, gui::layout::grid(0, 1, 1, 0, 2, 3) };

    // (var) m_tabview
    // Tabview containing tabs for the various model editing subtools.
    gui::tabview m_tabview {*this, gui::layout::grid(0, 1, 1, 2, 1, 3) };

    // FIXME explain
    gui::tabview::page m_vtxtab{m_tabview};
    vertex_ctl m_vtxctl {m_vtxtab, gui::layout::fill() };

    // (var) m_tracker
    // Tracks the asset name attached to this tool.
    res::tracker<res::asset> m_tracker;

    // (var) m_frame_tracker
    // Tracks the gfx::frame asset. This may be the main selected asset or a
    // dependency referenced by it.
    res::prop_tracker<gfx::frame> m_frame_tracker;

    // (var) m_anim_tracker
    // Tracks the gfx::anim asset. This may be the main selected asset or a
    // dependency referenced by it.
    res::prop_tracker<gfx::anim> m_anim_tracker;

    // (var) m_mesh_tracker
    // Tracks the gfx::mesh asset. This may be the main selected asset or a
    // dependency referenced by it.
    res::prop_tracker<gfx::mesh> m_mesh_tracker;

    // (var) m_model_tracker
    // Tracks the gfx::model asset. This may be the main selected asset or a
    // dependency referenced by it.
    res::prop_tracker<gfx::model> m_model_tracker;

    // (var) m_world_tracker
    // Tracks the gfx::world asset. This may be the main selected asset or a
    // dependency referenced by it.
    res::prop_tracker<gfx::world> m_world_tracker;

    // (var) m_wgeov2_tracker
    // Tracks the nsf::wgeo_v2 asset.
    res::tracker<nsf::wgeo_v2> m_wgeov2_tracker;

public:
    // (explicit ctor)
    // FIXME explain
    explicit tool(
        gui::container &parent,
        gui::layout layout,
        context &ctx) :
        assettool(parent, layout, ctx)
    {
        m_vp.show();
        m_tabview.show();
        m_vtxctl.show();
        m_vtxtab.set_text("Vertex");
        m_vtxtab.set_visible(true);

        on_name_change <<= [this](res::atom name) {
            m_tracker.set_name(name);
        };

        m_tracker.on_acquire <<= [this](res::asset *asset) {
            if (dynamic_cast<gfx::frame *>(asset)) {
                m_frame_tracker.set_name(asset->get_name());
                set_available(true);
            } else if (dynamic_cast<gfx::anim *>(asset)) {
                m_anim_tracker.set_name(asset->get_name());
                set_available(true);
            } else if (dynamic_cast<gfx::mesh *>(asset)) {
                m_mesh_tracker.set_name(asset->get_name());
                set_available(true);
            } else if (dynamic_cast<gfx::model *>(asset)) {
                m_model_tracker.set_name(asset->get_name());
                set_available(true);
            } else if (dynamic_cast<gfx::world *>(asset)) {
                m_world_tracker.set_name(asset->get_name());
                set_available(true);
            } else if (dynamic_cast<nsf::wgeo_v2 *>(asset)) {
                m_wgeov2_tracker.set_name(asset->get_name());
                set_available(true);
            }
        };
        m_tracker.on_lose <<= [this] {
            m_wgeov2_tracker.set_name(nullptr);
            m_world_tracker.set_name(nullptr);
            m_model_tracker.set_name(nullptr);
            m_mesh_tracker.set_name(nullptr);
            m_anim_tracker.set_name(nullptr);
            m_frame_tracker.set_name(nullptr);
            set_available(false);
        };

        m_frame_tracker.on_acquire <<= [this](gfx::frame *frame) {
            // TODO
        };
        m_frame_tracker.on_lose <<= [this] {
            // TODO
        };

        m_anim_tracker.on_acquire <<= [this](gfx::anim *anim) {
            // FIXME m_frame_tracker.set_prop(&anim->p_frame);
        };
        m_anim_tracker.on_lose <<= [this] {
            // FIXME m_frame_tracker.set_prop(nullptr);
        };

        m_model_tracker.on_acquire <<= [this](gfx::model *model) {
            m_anim_tracker.set_prop(&model->p_anim);
            m_mesh_tracker.set_prop(&model->p_mesh);
        };
        m_model_tracker.on_lose <<= [this] {
            m_anim_tracker.set_prop(nullptr);
            m_mesh_tracker.set_prop(nullptr);
        };

        m_world_tracker.on_acquire <<= [this](gfx::world *world) {
            m_model_tracker.set_prop(&world->p_model);
        };
        m_world_tracker.on_lose <<= [this] {
            m_model_tracker.set_prop(nullptr);
        };

        m_wgeov2_tracker.on_acquire <<= [this](nsf::wgeo_v2 *wgeo_v2) {
            m_world_tracker.set_prop(&wgeo_v2->p_world);
        };
        m_wgeov2_tracker.on_lose <<= [this] {
            m_world_tracker.set_prop(nullptr);
        };
    }

    std::string get_title() const override
    {
        return "3D";
    }
};

inline void vertex_ctl::frame()
{
    ImGui::Text("TODO no options available yet");
}

}
}
}
