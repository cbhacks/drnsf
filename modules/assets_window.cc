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
#include "begin.hh"

DEFINE_SHARED(res::ref<res::asset>,selected_asset);

bool visible = false;

void frame(int delta) override
{
	ImGui::Checkbox("Assets",&visible);

	if (!visible)
		return;

	ImGui::Begin("Assets",&visible);

	for (auto &&name : ns.get_asset_names()) {
		std::string path = name.full_path();
		if (ImGui::Selectable(
			path.c_str(),
			selected_asset == name
		)) {
			selected_asset = name;
		}
	}

	ImGui::End();
}

END_MODULE
