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

#include "module.hh"

namespace drnsf {

BEGIN_MODULE

bool visible = false;

void frame(int delta)
{
	auto &&ns = m_core.m_proj.get_asset_root();

	ImGui::Checkbox("Assets",&visible);

	if (!visible)
		return;

	ImGui::Begin("Assets",&visible);

	for (auto &&name : ns.get_asset_names()) {
		std::string path = name.full_path();
		if (ImGui::Selectable(
			path.c_str(),
			edit::g_selected_asset == name
		)) {
			edit::g_selected_asset = name;
		}
	}

	ImGui::End();
}

END_MODULE

std::function<void(int)> create_assets_window(edit::core &core)
{
	return [m = mod(core)](int delta_time) mutable {
		m.frame(delta_time);
	};
}

}
