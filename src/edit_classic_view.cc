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
#include "gfx.hh"
#include "misc.hh"
#include "nsf.hh"

namespace drnsf {
namespace edit {

classic_view::classic_view(gui::container &parent,project &proj) :
	m_split(parent),
	m_tree(m_split.get_left()),
	m_detail(m_split.get_right(),"[details here]"),
	m_proj(proj)
{
	m_tree.show();
	m_detail.show();

	h_asset_appear <<= [this](res::asset &asset){
		m_asset_nodes[&asset] = std::make_unique<asset_node>(
			*this,
			asset
		);
	};
	h_asset_appear.bind(m_proj.on_asset_appear);

	h_asset_disappear <<= [this](res::asset &asset){
		m_asset_nodes.erase(&asset);
	};
	h_asset_disappear.bind(m_proj.on_asset_disappear);
}

void classic_view::show()
{
	m_split.show();
}

}
}
