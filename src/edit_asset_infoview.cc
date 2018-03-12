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
// Implementation class for asset_infoview (PIMPL).
class asset_infoview::impl final : private gui::composite {
    friend class asset_infoview;

private:
    // (var) m_outer
    // A reference to the outer asset_infoview.
    asset_infoview &m_outer;

    // (var) m_proj
    // A reference to the project this tree applies to.
    res::project &m_proj;

    // (var) m_selected_asset
    // The currently selected asset name.
    res::atom m_selected_asset;

    // (var) m_assetname, m_assetname_title
    // A label showing the selected asset's name. The _title widget is a
    // label to identify the purpose of the other to the user.
    gui::label m_assetname;
    gui::label m_assetname_title;

    // (var) m_assettype, m_assettype_title
    // A label indicating the asset's type. The _title widget is another
    // label alongside this one which identifies it ("Asset Type").
    gui::label m_assettype;
    gui::label m_assettype_title;

    // (handler) h_asset_appear
    // Hooks the project's on_asset_appear event so that, if the selected
    // asset comes into existence, the details can be updated.
    decltype(res::project::on_asset_appear)::watch h_asset_appear;

    // (handler) h_asset_disappear
    // Hooks the project's on_asset_disappear event so that the currently
    // selected asset's details can be erased if it disappears.
    decltype(res::project::on_asset_disappear)::watch h_asset_disappear;

public:
    // (explicit ctor)
    // Initializes the widget and installs event handlers.
    explicit impl(
        asset_infoview &outer,
        res::project &proj) :
        composite(outer, gui::layout::fill()),
        m_outer(outer),
        m_proj(proj),
        m_assetname(*this, gui::layout::grid(1, 1, 2, 0, 1, 2)),
        m_assetname_title(*this, gui::layout::grid(0, 1, 2, 0, 1, 2), "Asset Name"),
        m_assettype(*this, gui::layout::grid(1, 1, 2, 1, 1, 2)),
        m_assettype_title(*this, gui::layout::grid(0, 1, 2, 1, 1, 2), "Asset Type")
    {
        h_asset_appear <<= [this](res::asset &asset) {
            if (asset.get_name() == m_selected_asset) {
                update(asset);
            }
        };
        h_asset_appear.bind(m_proj.on_asset_appear);

        h_asset_disappear <<= [this](res::asset &asset) {
            if (asset.get_name() == m_selected_asset) {
                update_null();
            }
        };
        h_asset_disappear.bind(m_proj.on_asset_disappear);

        m_assetname.show();
        m_assetname_title.show();
        m_assettype.show();
        m_assettype_title.show();
    }

    // (func) update
    // Updates the display text for a specific asset instance (NOT a ref
    // or atom!).
    void update(res::asset &asset)
    {
        m_assettype.set_text(typeid(asset).name());
    }

    // (func) update_null
    // Updates the displayed information for a no-such-asset-exists state.
    void update_null()
    {
        m_assettype.set_text("");
    }
};

// declared in edit.hh
asset_infoview::asset_infoview(gui::container &parent, gui::layout layout, res::project &proj) :
    composite(parent, layout)
{
    M = new impl(*this, proj);
    M->show();
}

// declared in edit.hh
asset_infoview::~asset_infoview()
{
    delete M;
}

// declared in edit.hh
void asset_infoview::set_selected_asset(res::atom atom)
{
    M->m_selected_asset = atom;

    if (!atom) {
        M->m_assetname.set_text("");
        M->update_null();
    } else {
        M->m_assetname.set_text(atom.full_path());
        if (auto ptr = atom.get()) {
            M->update(*ptr);
        } else {
            M->update_null();
        }
    }
}

}
}
