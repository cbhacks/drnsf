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
#include <typeinfo>
#include "edit.hh"

namespace drnsf {
namespace edit {

// declared in edit.hh
void asset_metactl::frame()
{
    if (!m_name) {
        ImGui::Text("No asset selected.");
        return;
    }

    res::asset *asset = m_name.get();
    if (!asset) {
        ImGui::Text("Name:     %s", m_name.full_path().c_str());
        ImGui::Text("Type:     (does not exist)");

        // TODO actions for non-existing atoms
        return;
    }

    ImGui::Text("Name:     %s", m_name.full_path().c_str());
    ImGui::Text("Type:     %s", typeid(asset).name());

    // TODO actions for existing assets
}

// declared in edit.hh
asset_metactl::asset_metactl(gui::container &parent, gui::layout layout):
    widget_im(parent, layout)
{
    h_asset_appear <<= [this](res::asset &asset) {
        if (asset.get_name() == m_name) {
            // TODO - This will be useful in the future when this isn't an
            // ImGui widget.
        }
    };
    h_asset_disappear <<= [this](res::asset &asset) {
        if (asset.get_name() == m_name) {
            // TODO - This will be useful in the future when this isn't an
            // ImGui widget.
        }
    };
}

// declared in edit.hh
void asset_metactl::set_name(res::atom name)
{
    if (m_name) {
        h_asset_appear.unbind();
        h_asset_disappear.unbind();
    }
    m_name = name;
    if (name) {
        auto proj = name.get_proj();
        h_asset_appear.bind(proj->on_asset_appear);
        h_asset_disappear.bind(proj->on_asset_disappear);
    }
}

}
}
