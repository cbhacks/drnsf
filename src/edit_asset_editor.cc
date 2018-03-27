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
#include "edit.hh"

namespace drnsf {
namespace edit {

// (inner class) impl
// FIXME explain
class asset_editor::impl final : private gui::composite {
    friend class asset_editor;

private:
    // (var) m_outer
    // A reference to the outer asset_editor object which uses this object.
    asset_editor &m_outer;

    // (var) m_proj
    // A reference to the project this editor operates on.
    res::project &m_proj;

    // (var) m_tree
    // FIXME explain
    asset_tree m_tree;

    // (var) m_mainview
    // FIXME explain
    asset_mainctl m_mainview;

    // (handler) h_tree_select
    // Handler for the asset tree's on_select event.
    decltype(m_tree.on_select)::watch h_tree_select;

public:
    // (explicit ctor)
    // Initializes the editor, and constructs the necessary widgets.
    explicit impl(
        asset_editor &outer,
        res::project &proj) :
        composite(outer, gui::layout::fill()),
        m_outer(outer),
        m_proj(proj),
        m_tree(*this, gui::layout::grid(0, 1, 3, 0, 1, 1), proj),
        m_mainview(*this, gui::layout::grid(1, 2, 3, 0, 1, 1))
    {
        h_tree_select <<= [this](res::atom atom) {
            g_selected_asset = atom;
            m_mainview.set_name(atom);
        };
        h_tree_select.bind(m_tree.on_select);

        m_tree.show();
        m_mainview.show();
    }
};

// declared in edit.hh
asset_editor::asset_editor(gui::container &parent, gui::layout layout, res::project &proj) :
    composite(parent, layout)
{
    M = new impl(*this, proj);
    M->show();
}

// declared in edit.hh
asset_editor::~asset_editor()
{
    delete M;
}

}
}
