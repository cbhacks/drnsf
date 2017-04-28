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

class assets_view::impl : private util::nocopy {
private:
	class node;

	editor &m_ed;
	gui::treeview m_tree;
	std::map<res::atom,std::weak_ptr<node>> m_atom_nodes;
	std::map<res::asset *,std::shared_ptr<node>> m_asset_nodes;

	decltype(res::project::on_asset_appear)::watch h_asset_appear;
	decltype(res::project::on_asset_disappear)::watch h_asset_disappear;

public:
	explicit impl(gui::container &parent,editor &ed) :
		m_ed(ed),
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
		h_asset_appear.bind(m_ed.m_proj.on_asset_appear);

		h_asset_disappear <<= [this](res::asset &asset) {
			m_asset_nodes.erase(&asset);
		};
		h_asset_disappear.bind(m_ed.m_proj.on_asset_disappear);
	}

	void show()
	{
		m_tree.show();
	}
};

class assets_view::impl::node : private util::nocopy {
private:
	std::shared_ptr<node> m_parent;
	std::unique_ptr<gui::treeview::node> m_treenode;

public:
	explicit node(impl &view,res::atom atom)
	{
		auto parent_atom = atom.get_parent();
		if (parent_atom == view.m_ed.m_proj.get_asset_root()) {
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
	}
};

assets_view::assets_view(gui::container &parent,editor &ed)
{
	M = new impl(parent,ed);
}

assets_view::~assets_view()
{
	delete M;
}

void assets_view::show()
{
	M->show();
}

}
}
