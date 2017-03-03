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
#include "gfx.hh"
#include "begin.hh"

constexpr static const char *mod_name = "Asset Editor: GFX Model";

DEFINE_SHARED(res::ref<res::asset>,selected_asset);

void on_assets_window_body()
{
	gfx::model::ref m = selected_asset;
	if (!m.ok())
		return;

	gfx::anim::ref anim = m->get_anim();
	ImGui::Text("Anim:");
	ImGui::SameLine();

	gfx::mesh::ref mesh = m->get_mesh();
	ImGui::Text("Mesh:");
	ImGui::SameLine();

	ImGui::BeginChild("Render",{0,0},true);
	if (anim.ok() && mesh.ok()) {
		ImGui::Text("<render full model>");
	} else if (anim.ok()) {
		ImGui::Text("<render only anim>");
	} else {
		ImGui::Text("Nothing could be rendered; invalid frame or animation.");
	}
	ImGui::EndChild();
} HOOK_EVENT(assets_window_body);

END_MODULE
