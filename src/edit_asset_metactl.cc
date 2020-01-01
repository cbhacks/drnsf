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
#include <typeinfo>
#include "edit.hh"

namespace drnsf {
namespace edit {

namespace {

// (s-func) do_options
// FIXME explain
template <typename T>
void do_options(T *asset)
{
    ImGui::Text("No options available.");
}
template <>
void do_options<res::asset>(res::asset *asset)
{
    if (ImGui::Button("Delete asset")) {
        asset->get_proj().get_transact().run([&](TRANSACT) {
            TS.describe("Delete '$'"_fmt(asset->get_name().full_path()));
            asset->destroy(TS);
        });

        // Exit now since the asset no longer exists. We must prevent the other
        // buttons from running.
        // TODO - somehow prevent this from occurring with the later option
        // sections
        return;
    }
}

// (internal type) asset_handler
// FIXME explain
template <typename AssetType>
struct asset_handler {
    void operator ()(AssetType *asset)
    {
        using type_info = reflect::asset_type_info<AssetType>;

        // TODO - disallow actions while transaction system is busy

        auto title = "Options for $"_fmt(type_info::name);
        if (ImGui::CollapsingHeader(
            title.c_str(), ImGuiTreeNodeFlags_DefaultOpen
        )) {
            ImGui::Indent();
            do_options<AssetType>(asset);
            ImGui::Unindent();
        }

        asset_handler<typename type_info::base_type>{}(
            static_cast<typename type_info::base_type *>(asset)
        );
    }
};
template <>
struct asset_handler<void> {
    void operator ()(void *) {}
};

}

// declared in edit.hh
void asset_metactl::frame()
{
    if (!m_name) {
        ImGui::Text("No asset selected.");
        return;
    }

    ImGui::Text("Name:     %s", m_name.full_path().c_str());
    ImGui::Separator();

    res::asset *asset = m_name.get();
    if (!asset) {
        ImGui::Text("(does not exist)");

        // TODO actions for non-existing atoms
        return;
    }

    auto handler = [](auto asset) {
        asset_handler<
            std::remove_pointer_t<decltype(asset)>
        >{}(asset);
    };

    util::dynamic_call<
        const decltype(handler) &,
        res::asset,
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
        res::asset>(handler, asset);
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
