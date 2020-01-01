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
#include "edit.hh"
#include "edit_mode_classic.hh"

namespace drnsf {
namespace edit {
namespace mode_classic {

// declared in edit.hh
mainctl::mainctl(
    gui::container &parent,
    gui::layout layout,
    context &ctx) :
    composite(parent, layout),
    m_ctx(ctx)
{
    // A separate watch object is used here instead of directly inserting a
    // function into the event to ensure the handler does not outlive the body
    // widget; this would cause a problem during destruction if a node was
    // selected in the tree at that time.
    h_tree_select <<= [this](res::atom atom) {
        m_body.set_name(atom);
    };
    h_tree_select.bind(m_tree.on_select);

    h_project_change <<= [this](const std::shared_ptr<res::project> &proj) {
        m_tree.set_proj(proj.get());
    };
    h_project_change.bind(m_ctx.on_project_change);
    h_project_change(m_ctx.get_proj());

    m_tree.show();
    m_body.show();
}

}
}
}
