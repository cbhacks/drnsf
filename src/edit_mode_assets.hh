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
 * edit_mode_assets.hh
 *
 * FIXME explain
 */

#include "edit.hh"

namespace drnsf {
namespace edit {
namespace mode_assets {

/*
 * edit::mode_assets::mainctl
 *
 * FIXME explain
 */
class mainctl : private gui::composite {
private:
    // (var) m_ctx
    // The context this asset editor is attached to.
    context &m_ctx;

    // (var) m_tree
    // A treeview displaying the asset name tree for the current project.
    asset_tree m_tree{ *this, gui::layout::grid(0, 1, 3, 0, 1, 1) };

    // (var) m_body
    // The main editor for the tree's selected asset.
    asset_mainctl m_body{ *this, gui::layout::grid(1, 2, 3, 0, 1, 1) };

    // (handler) h_project_change
    // Hooks the editor context's project change event to update the mode for
    // the new target project.
    decltype(context::on_project_change)::watch h_project_change;

public:
    // (explicit ctor)
    // FIXME explain
    explicit mainctl(
        gui::container &parent,
        gui::layout layout,
        context &ctx);

    using composite::show;
    using composite::hide;
    using composite::get_layout;
    using composite::set_layout;
    using composite::get_real_size;
    using composite::get_screen_pos;
};

/*
 * edit::mode_assets::handler
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
    // Constructs the new asset mode handler and its widgets.
    explicit handler(gui::container &parent, context &ctx) :
        mode_handler(ctx),
        m_mainctl(parent, gui::layout::fill(), ctx) {}

    // (func) start
    // Displays the asset editor.
    void start() override
    {
        m_mainctl.show();
    }

    // (func) stop
    // Hides the asset editor.
    void stop() noexcept override
    {
        m_mainctl.hide();
    }
};

}
}
}
