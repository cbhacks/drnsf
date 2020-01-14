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
 * edit_assettool_props.hh
 *
 * FIXME explain
 */

#include "edit.hh"

namespace drnsf {
namespace edit {
namespace assettool_props {

/*
 * edit::assettool_props::tool
 *
 * FIXME explain
 */
class tool : public assettool {
private:
    // (var) m_ui
    // The main property field ui.
    asset_propctl m_ui{ *this, gui::layout::fill() };

    // (var) m_tracker
    // FIXME explain
    res::tracker<res::asset> m_tracker;

public:
    // (explicit ctor)
    // FIXME explain
    explicit tool(
        gui::container &parent,
        gui::layout layout,
        context &ctx) :
        assettool(parent, layout, ctx)
    {
        m_ui.show();

        on_name_change <<= [this](res::atom name) {
            m_tracker.set_name(name);
        };

        m_tracker.on_acquire <<= [this](res::asset *asset) {
            m_ui.set_name(asset->get_name());
            set_available(true);
        };

        m_tracker.on_lose <<= [this] {
            set_available(false);
            m_ui.set_name(nullptr);
        };
    }

    std::string get_title() const override
    {
        return "Properties";
    }
};

}
}
}
