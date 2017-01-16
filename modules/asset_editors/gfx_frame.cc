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

#include "common.hh"
#include "gfx.hh"
#include "begin.hh"

constexpr static const char *mod_name = "Asset Editor: GFX Frame";

DEFINE_SHARED(res::ref<res::asset>,selected_asset);

void on_assets_window_body()
{
	gfx::frame::ref as = selected_asset;
	if (!as.ok())
		return;

	auto &vertices = as->get_vertices();
	for (std::size_t i = 0;i < vertices.size();i++) {
		auto vert = vertices[i];
		if (ImGui::InputFloat3(util::format("$",i).c_str(),vert.v,3)) {
			auto new_vertices = vertices;
			new_vertices[i] = vert;
			nx << [&](TRANSACT) {
				ts.describef("Move vertex $ in '$'",i,as);
				as->set_vertices(ts,new_vertices);
			};
		}
	}
} HOOK_EVENT(assets_window_body);

END_MODULE
