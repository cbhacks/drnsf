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
 * edit_mode_map.hh
 *
 * FIXME explain
 */

#include "edit.hh"

namespace drnsf {
namespace edit {
namespace mode_map {

/*
 * edit::mode_map::mainctl
 *
 * FIXME explain
 */
class mainctl : private render::scene, private render::viewport {
private:
    // (var) m_ctx
    // The context this map editor is attached to.
    context &m_ctx;

    // (var) m_reticle
    // FIXME explain
    render::reticle_fig m_reticle{*this};

    // (var) m_world_figs
    // FIXME explain
    std::unordered_map<
        gfx::world *,
        std::unique_ptr<render::world_fig>> m_world_figs;

    // (var) m_world_tracker
    // Tracks the appearance and disappearance of `gfx::world' assets to apply
    // those changes to the `m_world_figs' collection.
    res::tree_tracker<gfx::world> m_world_tracker;

    // (handler) h_project_change
    // Hooks the editor context's project change event to update the view for
    // the new target project.
    decltype(context::on_project_change)::watch h_project_change;

public:
    // (explicit ctor)
    // FIXME explain
    explicit mainctl(
        gui::container &parent,
        gui::layout layout,
        context &ctx);

    using viewport::show;
    using viewport::hide;
    using viewport::get_layout;
    using viewport::set_layout;
    using viewport::get_real_size;
    using viewport::get_screen_pos;
};

/*
 * edit::mode_map::handler
 *
 * FIXME explain
 */
class handler : public mode_handler {
private:
    // (var) m_mainctl
    // The main widget for this mode.
    mainctl m_mainctl;

public:
    // (explicit ctor)
    // Constructs the new map mode handler and its map view widget.
    explicit handler(gui::container &parent, context &ctx) :
        mode_handler(ctx),
        m_mainctl(parent, gui::layout::fill(), ctx) {}

    // (func) start
    // Displays the map editor.
    void start() override
    {
        m_mainctl.show();
    }

    // (func) stop
    // Hides the map editor.
    void stop() noexcept override
    {
        m_mainctl.hide();
    }
};

}
}
}
