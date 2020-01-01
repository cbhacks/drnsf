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
#include "edit_mode_assets.hh"

namespace drnsf {
namespace edit {
namespace mode_assets {

// declared in edit.hh
mainctl::mainctl(
    gui::container &parent,
    gui::layout layout,
    context &ctx) :
    composite(parent, layout),
    m_ctx(ctx)
{
    m_tree.on_select <<= [this](res::atom atom) {
        m_body.set_name(atom);
    };

    h_project_change <<= [this](const std::shared_ptr<res::project> &proj) {
        if (proj) {
            m_tree.set_base(proj->get_asset_root());
        } else {
            m_tree.set_base(nullptr);
        }
    };
    h_project_change.bind(m_ctx.on_project_change);

    if (m_ctx.get_proj()) {
        m_tree.set_base(m_ctx.get_proj()->get_asset_root());
    }

    m_tree.show();
    m_body.show();
}

}
}
}
