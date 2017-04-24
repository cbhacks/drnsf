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

classic_view::classic_view(gui::container &parent) :
	m_split(parent),
	m_tree(m_split.get_left()),
	m_test_node(m_tree),
	m_test_subnode(m_test_node),
	m_detail(m_split.get_right(),"[details here]")
{
	m_test_node.set_text("Test Node!");
	m_test_subnode.set_text("Test Subnode!");
	m_tree.show();
	m_detail.show();
}

void classic_view::show()
{
	m_split.show();
}

}
}
