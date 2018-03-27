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

#include "gfx.hh"
#include "misc.hh"
#include "nsf.hh"

namespace drnsf {
namespace edit {

namespace {

// (internal typedef) base_of
// FIXME explain
template <typename AssetType>
using base_of = typename reflect::asset_type_info<AssetType>::base_type;

// (internal class) asset_viewctl_of
// FIXME explain
template <typename AssetType>
class asset_viewctl_of : public asset_viewctl_of<base_of<AssetType>> {
public:
    using asset_viewctl_of<base_of<AssetType>>::asset_viewctl_of;
};
template <>
class asset_viewctl_of<void> : private gui::label, public util::polymorphic {
public:
    // (explicit ctor)
    // FIXME explain
    explicit asset_viewctl_of(
        gui::container &parent,
        gui::layout layout,
        void *asset) :
        label(parent, layout)
    {
        set_text("No view for this type of asset.");
    }

    using label::show;
    using label::hide;
    using label::get_layout;
    using label::set_layout;
    using label::get_real_size;
    using label::get_screen_pos;
};

// FIXME explain
template <>
class asset_viewctl_of<gfx::frame> :
    private gui::label,
    public util::polymorphic {
public:
    // (explicit ctor)
    // FIXME explain
    explicit asset_viewctl_of(
        gui::container &parent,
        gui::layout layout,
        void *asset) :
        label(parent, layout)
    {
        // TODO - actual frame viewer
        set_text("[ FRAME VIEW HERE ]");
    }

    using label::show;
    using label::hide;
    using label::get_layout;
    using label::set_layout;
    using label::get_real_size;
    using label::get_screen_pos;
};

}

// declared in edit.hh
asset_viewctl::asset_viewctl(gui::container &parent, gui::layout layout) :
    composite(parent, layout)
{
    h_asset_appear <<= [this](res::asset &asset) {
        if (asset.get_name() == m_name) {

            auto handler = [this](auto asset_ptr) {
                auto inner = new asset_viewctl_of<
                    std::remove_pointer_t<decltype(asset_ptr)>
                >(*this, gui::layout::fill(), asset_ptr);
                m_inner.reset(inner);
                inner->show();
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
                res::asset>(handler, &asset);
        }
    };
    h_asset_disappear <<= [this](res::asset &asset) {
        if (asset.get_name() == m_name) {
            m_inner = nullptr;
        }
    };
}

// declared in edit.hh
void asset_viewctl::set_name(res::atom name)
{
    if (m_name) {
        h_asset_appear.unbind();
        h_asset_disappear.unbind();
        m_inner = nullptr;
    }
    m_name = name;
    if (name) {
        auto proj = name.get_proj();
        h_asset_appear.bind(proj->on_asset_appear);
        h_asset_disappear.bind(proj->on_asset_disappear);

        auto asset = name.get();
        if (asset) {
            h_asset_appear(*asset);
        }
    }
}

}
}
