//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017  DRNSF contributors
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
// Implementation class for asset_tree (PIMPL).
class asset_tree::impl : private util::nocopy {
    friend class asset_tree;

private:
    // defined later in this file
    class node;

    // (var) m_outer
    // A reference to the outer asset_tree.
    asset_tree &m_outer;

    // (var) m_proj
    // A reference to the project this tree applies to.
    res::project &m_proj;

    // (var) m_selected_asset
    // The selected asset name.
    res::atom m_selected_asset;

    // (var) m_tree
    // The treeview widget which this object drives. This is the actual
    // widget which appears on screen. asset_tree merely adapts the widget
    // for use as an asset browser/selector.
    gui::treeview m_tree;

    // (var) m_atom_nodes
    // The tree nodes for each atom. These are weak pointers to ensure that
    // atoms only have nodes if they are bound to an asset or have a
    // descendant node which are.
    std::map<res::atom,std::weak_ptr<node>> m_atom_nodes;

    // (var) m_asset_nodes
    // The tree nodes for each asset in the project. This map's shared_ptrs
    // also keep the nodes alive whereas the m_atom_nodes field above does
    // not.
    std::map<res::asset *,std::shared_ptr<node>> m_asset_nodes;

    // (var) m_selected_node
    // The node under the selected asset name. This shared_ptr keeps the
    // node alive even if it has no descendants and is not bound to an
    // asset.
    std::shared_ptr<node> m_selected_node;

    // (handler) h_asset_appear
    // Hooks the project's on_asset_appear event so that the asset can be
    // added to the tree when this occurs.
    decltype(res::project::on_asset_appear)::watch h_asset_appear;

    // (handler) h_asset_disappear
    // Hooks the project's on_asset_disappear event so that the asset can
    // be removed from the tree when this occurs.
    decltype(res::project::on_asset_disappear)::watch h_asset_disappear;

public:
    // (explicit ctor)
    // Initializes the treeview widget and installs event handlers.
    explicit impl(
        asset_tree &outer,
        gui::container &parent,
        res::project &proj) :
        m_outer(outer),
        m_proj(proj),
        m_tree(parent)
    {
        h_asset_appear <<= [this](res::asset &asset) {
            auto name = asset.get_name();
            auto &atom_node_wp = m_atom_nodes[name];
            auto &atom_node_sp = m_asset_nodes[&asset];
            atom_node_sp = atom_node_wp.lock();
            if (!atom_node_sp) {
                atom_node_sp = std::make_shared<node>(
                    *this,
                    name
                );
                atom_node_wp = atom_node_sp;
            }
        };
        h_asset_appear.bind(m_proj.on_asset_appear);

        h_asset_disappear <<= [this](res::asset &asset) {
            m_asset_nodes.erase(&asset);
        };
        h_asset_disappear.bind(m_proj.on_asset_disappear);
    }
};

// (inner class) node
// A treeview node which is associated with a particular atom. There is exactly
// one node for each atom, except for atoms which are not associated with an
// asset and have no descendants which are.
class asset_tree::impl::node : private util::nocopy {
private:
    // (var) m_parent
    // A non-weak pointer to the parent node. This ensures a parent node
    // whose atom is not associated with an asset is kept alive by its
    // child nodes.
    std::shared_ptr<node> m_parent;

    // (var) m_treenode
    // The actual treeview node object. This is a pointer rather than a
    // direct object because construction must be deferred to somewhere
    // midway into this class's constructor.
    std::unique_ptr<gui::treeview::node> m_treenode;

    // (handler) h_select, h_deselect
    // Hooks the treeview node's on_select and on_deselect events so that
    // theses event can be propagated out to the asset_tree.
    decltype(gui::treeview::node::on_select)::watch h_select;
    decltype(gui::treeview::node::on_deselect)::watch h_deselect;

public:
    // (explicit ctor)
    // Constructs the node object and the backing treeview node at the
    // correct location in the treeview. This may also construct parent
    // nodes if they do not yet exist.
    explicit node(impl &view,res::atom atom)
    {
        auto parent_atom = atom.get_parent();
        if (parent_atom == view.m_proj.get_asset_root()) {
            m_treenode = std::make_unique<gui::treeview::node>(
                view.m_tree
            );
        } else {
            auto &parent_node_wp = view.m_atom_nodes[parent_atom];
            m_parent = parent_node_wp.lock();
            if (!m_parent) {
                m_parent = std::make_shared<node>(
                    view,
                    parent_atom
                );
                parent_node_wp = m_parent;
            }
            m_treenode = std::make_unique<gui::treeview::node>(
                *m_parent->m_treenode
            );
        }
        m_treenode->set_text(atom.name());

        h_select <<= [this,atom,&view]{
            view.m_selected_asset = atom;
            view.m_outer.on_select(atom);
            view.m_selected_node = view.m_atom_nodes[atom].lock();
        };
        h_select.bind(m_treenode->on_select);

        h_deselect <<= [this,atom,&view]{
            if (view.m_selected_asset == atom) {
                view.m_selected_asset = nullptr;
                view.m_outer.on_select(nullptr);
            }
        };
        h_deselect.bind(m_treenode->on_deselect);
    }
};

// declared in edit.hh
asset_tree::asset_tree(gui::container &parent,res::project &proj)
{
    M = new impl(*this,parent,proj);
}

// declared in edit.hh
asset_tree::~asset_tree()
{
    delete M;
}

// declared in edit.hh
GtkWidget *asset_tree::get_handle()
{
    return M->m_tree.get_handle();
}

}
}
