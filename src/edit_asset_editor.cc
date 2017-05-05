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
// FIXME explain
class asset_editor::impl : private util::nocopy {
	friend class asset_editor;

private:
	// (var) m_outer
	// A reference to the outer asset_editor object which uses this object.
	asset_editor &m_outer;

	// (var) m_proj
	// A reference to the project this editor operates on.
	res::project &m_proj;

	// (var) m_split
	// A vertical split of the widget. The left side contains the asset
	// tree while the right side contains the selected asset's details.
	gui::splitview m_split;

	// (var) m_tree
	// FIXME explain
	asset_tree m_tree;

	// (var) m_grid
	// FIXME explain
	gui::gridview m_grid;

	// (var) m_infoview
	// General informational widget for the currently selected asset.
	asset_infoview m_infoview;

	// (var) m_propview
	// A property editor for the currently selected asset.
	asset_propview m_propview;

	// (var) m_viewport
	// A GL viewport used to display the currently selected asset to the
	// user.
	asset_viewport m_viewport;

	// (handler) h_tree_select
	// Handler for the asset tree's on_select event.
	decltype(m_tree.on_select)::watch h_tree_select;

public:
	// (explicit ctor)
	// Initializes the editor, and constructs the necessary widgets.
	explicit impl(
		asset_editor &outer,
		gui::container &parent,
		res::project &proj) :
		m_outer(outer),
		m_proj(proj),
		m_split(parent),
		m_tree(m_split.get_left(),proj),
		m_grid(m_split.get_right(),2,2,true),
		m_infoview(m_grid.make_slot(0,0,1,1),proj),
		m_propview(m_grid.make_slot(0,1,2,1),proj),
		m_viewport(m_grid.make_slot(1,0,1,1),proj)
	{
		h_tree_select <<= [this](res::atom atom) {
			g_selected_asset = atom;
		};
		h_tree_select.bind(m_tree.on_select);

		m_tree.show();
		m_grid.show();
		m_infoview.show();
		m_propview.show();
		m_viewport.show();
	}
};

// declared in edit.hh
asset_editor::asset_editor(gui::container &parent,res::project &proj)
{
	M = new impl(*this,parent,proj);
}

// declared in edit.hh
asset_editor::~asset_editor()
{
	delete M;
}

// declared in edit.hh
void asset_editor::show()
{
	M->m_split.show();
}

}
}
