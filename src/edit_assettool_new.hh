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
 * edit_assettool_new.hh
 *
 * FIXME explain
 */

#include "edit.hh"

namespace drnsf {
namespace edit {
namespace assettool_new {

/*
 * edit::assettool_new::new_asset_ctl
 *
 * UI for the "new" editor asset mode; provides Create Asset buttons for each
 * of the available asset types.
 */
template <typename HeadType, typename... TailTypes>
struct new_asset_ctl : new_asset_ctl<TailTypes...> {
    using new_asset_ctl<TailTypes...>::new_asset_ctl;
    using new_asset_ctl<TailTypes...>::m_name;

    void frame() override
    {
        auto str = "Create $"_fmt(reflect::asset_type_info<HeadType>::name);
        if constexpr (!std::is_abstract_v<HeadType>) {
            if (ImGui::Button(str.c_str())) {
                if (!m_name || m_name.get())
                    return;

                auto proj = m_name.get_proj();
                auto &nx = proj->get_transact();
                if (nx.get_status() != transact::status::ready)
                    return;

                nx.run([&](TRANSACT) {
                    res::asset::create<HeadType>(TS, m_name, *proj);

                    TS.describe("Create '$'"_fmt(m_name));
                });
                return;
            }
        }

        new_asset_ctl<TailTypes...>::frame();
    }
};
template <>
struct new_asset_ctl<res::asset> : gui::widget_im {
    using widget_im::widget_im;

    res::atom m_name;

    void frame() override {}
};

/*
 * edit::assettool_new::tool
 *
 * FIXME explain
 */
class tool : public assettool {
private:
    // (var) m_ui
    // The tool UI which allows the user to create an asset with the current
    // selected name.
    new_asset_ctl<
        gfx::frame,
        gfx::anim,
        gfx::mesh,
        gfx::model,
        gfx::world,
        misc::raw_data,
        nsf::archive,
        nsf::spage,
        nsf::raw_entry,
        nsf::wgeo_v2,
        nsf::entry,
        res::asset
    > m_ui{ *this, gui::layout::fill() };

    // (var) m_tracker
    // FIXME explain
    res::tracker<res::asset> m_tracker;

    // (func) update_availability
    // Updates the availability of the tool; the tool is only available for
    // asset names with no asset currently bound.
    void update_availability()
    {
        auto name = get_name();
        set_available(name && !name.get());
    }

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
            m_ui.m_name = name;
            m_tracker.set_name(name);
            set_available(name && !name.get());
        };

        m_tracker.on_acquire <<= [this](res::asset *asset) {
            set_available(false);
        };

        m_tracker.on_lose <<= [this] {
            if (get_name()) {
                set_available(true);
            }
        };
    }

    std::string get_title() const override
    {
        return "New";
    }
};

}
}
}
