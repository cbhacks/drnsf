//
// WILLYMOD - An unofficial Crash Bandicoot level editor
// Copyright (C) 2016  WILLYMOD project contributors
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

constexpr static const char *mod_name = "Assets Window";

DEFINE_SHARED(res::ref<res::asset>,selected_asset);

bool visible = false;

void frame(int delta) override
{
	if (!visible)
		return;

	ImGui::Begin("Assets",&visible);

	ImGui::BeginChild("Tree",{200,0},true);
	for (auto &&name : ns.get_asset_names()) {
		if (ImGui::Selectable(
			name.c_str(),
			selected_asset == name
		)) {
			selected_asset = name;
		}
	}
	ImGui::EndChild();

	ImGui::SameLine();
	ImGui::BeginGroup();
	if (selected_asset.ok()) {
		selected_asset->run_gui();
		
		ImGui::Separator();
		ImGui::Separator();
		ImGui::Separator();

		ImGui::Text(selected_asset.c_str()); // FIXME 80 col
		if (ImGui::Button("Delete")) {
			nx << [&](TRANSACT) {
				ts.describef("Delete '$'",selected_asset);
				selected_asset->destroy(ts);
			};
		}

		ImGui::SameLine();
		ImGui::Button("Rename");

		ImGui::Separator();
		raise("assets_window_body");
	}
	ImGui::EndGroup();

	ImGui::End();
}

void show_view_menu() override
{
	ImGui::MenuItem("Assets",nullptr,&visible);
}

END_MODULE
