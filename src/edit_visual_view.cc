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

namespace drnsf {
namespace edit {

visual_view::visual_view(gui::container &parent,project &proj) :
	m_canvas(parent),
	m_proj(proj)
{
	h_asset_appear <<= [this](res::asset &asset) {
		if (typeid(asset) == typeid(gfx::frame)) {
			auto &frame = static_cast<gfx::frame &>(asset);
		} else if (typeid(asset) == typeid(gfx::mesh)) {
			auto &mesh = static_cast<gfx::mesh &>(asset);
		} else if (typeid(asset) == typeid(gfx::model)) {
			auto &model = static_cast<gfx::model &>(asset);
		}
	};
	h_asset_appear.bind(proj.on_asset_appear);

	h_asset_disappear <<= [this](res::asset &asset) {
		if (typeid(asset) == typeid(gfx::frame)) {
			auto &frame = static_cast<gfx::frame &>(asset);
		} else if (typeid(asset) == typeid(gfx::mesh)) {
			auto &mesh = static_cast<gfx::mesh &>(asset);
		} else if (typeid(asset) == typeid(gfx::model)) {
			auto &model = static_cast<gfx::model &>(asset);
		}
	};
	h_asset_disappear.bind(proj.on_asset_disappear);
}

void visual_view::show()
{
	m_canvas.show();
}

}
}
