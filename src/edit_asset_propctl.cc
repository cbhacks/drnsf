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

namespace drnsf {
namespace edit {

namespace {

// (internal typedef) base_of
// FIXME explain
template <typename AssetType>
using base_of = typename reflect::asset_type_info<AssetType>::base_type;

// (internal class) prop_handler
// FIXME explain
template <typename AssetType, int PropNum>
class prop_handler : public prop_handler<AssetType, PropNum - 1> {
private:
    // (internal typedef) prop_info
    // An alias for the reflection type which provides information about the
    // specified property.
    using prop_info = reflect::asset_prop_info<AssetType, PropNum>;

    // (var) m_field
    // The `edit::field' which operates on the value of this property.
    edit::field<typename prop_info::type> m_field;

    // (var) m_asset
    // A pointer to the asset this control is bound to, or null if no asset is
    // bound.
    AssetType *m_asset;

    // (handler) h_change
    // Hooks the property's change event so that the field can be updated when
    // the value changes.
    util::event<>::watch h_change;

public:
    // (default ctor)
    // Sets up event handlers for the field and asset property to forward any
    // changes between the two.
    prop_handler()
    {
        m_field.on_change <<= [this](typename prop_info::type new_value) {
            // Ensure there is at least an asset bound to this object.
            if (!m_asset) {
                return;
            }

            // Don't commit any changes if the transaction system is busy. This
            // should only happen if the user is in the middle of a long-running
            // asynchronous operation.
            auto &nx = m_asset->get_proj().get_transact();
            if (nx.get_status() != transact::status::ready) {
                return;
            }

            nx.run([&](TRANSACT) {
                // FIXME condense multiple consecutive changes of a single
                // property into a single transaction
                (m_asset->*prop_info::ptr).set(TS, std::move(new_value));

                TS.describe("Change $ on '$'"_fmt(
                    prop_info::name,
                    m_asset->get_name()
                ));
            });
        };
        h_change <<= [this]{
            // Forward changes in the property value to the field displaying
            // the value.
            m_field.bind(&(m_asset->*prop_info::ptr).get());
        };
    }

    // (func) bind
    // Binds the editor to the given asset. The editor's previous binding, if
    // any, is discarded.
    void bind(AssetType &asset)
    {
        prop_handler<AssetType, PropNum - 1>::bind(asset);
        if (m_asset) {
            h_change.unbind();
        }
        m_asset = &asset;
        h_change.bind((asset.*prop_info::ptr).on_change);
        m_field.bind(&(asset.*prop_info::ptr).get());
    }

    // (func) unbind
    // Releases the editor's existing binding, if any.
    void unbind()
    {
        prop_handler<AssetType, PropNum - 1>::unbind();
        if (m_asset) {
            h_change.unbind();
        }
        m_asset = nullptr;
        m_field.bind(nullptr);
    }

    // (func) frame
    // FIXME explain
    void frame()
    {
        prop_handler<AssetType, PropNum - 1>::frame();
        ImGui::TextUnformatted(prop_info::name);
        ImGui::NextColumn();
        ImGui::PushID(&(m_asset->*prop_info::ptr));
        m_field.frame();
        ImGui::PopID();
        ImGui::NextColumn();
    }
};
template <typename AssetType>
class prop_handler<AssetType, -1> : private util::nocopy {
public:
    // (func) bind, unbind, frame
    // Performs no operation. This terminates the various call chains going up
    // through the `prop_handler' inheritance tree.
    void bind(AssetType &asset) {}
    void unbind() {}
    void frame() {}
};

// (internal class) asset_propctl_of
// FIXME explain
template <typename AssetType>
class asset_propctl_of : public asset_propctl_of<base_of<AssetType>> {
private:
    // (internal typedef) type_info
    // An alias for the reflection type which provides information about the
    // specified asset type.
    using type_info = reflect::asset_type_info<AssetType>;

    // (var) m_top_prop_handler
    // The prop handler for the last property for AssetType. This prop handler
    // includes the prop handlers for all of the other properties for the given
    // asset type as well.
    prop_handler<AssetType, type_info::prop_count - 1> m_top_prop_handler;

public:
    // (func) bind
    // Binds all of the constituent property field editors to the given asset.
    // The previous binding, if any, is discarded.
    void bind(res::asset *asset) override
    {
        asset_propctl_of<base_of<AssetType>>::bind(asset);
        m_top_prop_handler.bind(*static_cast<AssetType *>(asset));
    }

    // (func) unbind
    // Unbinds all of the constituent property field editors, if they are bound
    // to an asset.
    void unbind() override
    {
        asset_propctl_of<base_of<AssetType>>::unbind();
        m_top_prop_handler.unbind();
    }

    // (func) frame
    // FIXME explain
    void frame() override
    {
        asset_propctl_of<base_of<AssetType>>::frame();

        // Don't display anything for this asset type if it doesn't have any
        // properties of its own.
        if (type_info::prop_count == 0) {
            return;
        }

        // Display a heading for this asset type's property list. The user can
        // click on it to show/hide the entire group if desired.
        auto title = "Properties for $"_fmt(type_info::name);
        if (!ImGui::CollapsingHeader(
            title.c_str(), ImGuiTreeNodeFlags_DefaultOpen
        )) {
            return;
        }

        ImGui::Columns(2);
        ImGui::SetColumnOffset(1, (ImGui::CalcTextSize("F").x + 1) * 24);
        m_top_prop_handler.frame();
        ImGui::Columns(1);
    }
};
template <>
class asset_propctl_of<void> : public util::polymorphic {
public:
    virtual void bind(res::asset *asset) {}
    virtual void unbind() {}
    virtual void frame() {}
};

}

// declared in edit.hh
void asset_propctl::frame()
{
    if (!m_name) {
        ImGui::Text("No asset selected.");
        return;
    }

    res::asset *asset = m_name.get();
    if (!asset) {
        ImGui::Text("Selected asset does not exist.");
        return;
    }

    static_cast<asset_propctl_of<void> &>(*m_inner).frame();
}

// declared in edit.hh
asset_propctl::asset_propctl(gui::container &parent, gui::layout layout):
    widget_im(parent, layout)
{
    h_asset_appear <<= [this](res::asset &asset) {
        if (asset.get_name() == m_name) {
            auto handler = [this, &asset](auto asset_ptr) {
                auto inner = new asset_propctl_of<
                    std::remove_pointer_t<decltype(asset_ptr)>
                >();
                m_inner.reset(inner);
                inner->bind(&asset);
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
void asset_propctl::set_name(res::atom name)
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
