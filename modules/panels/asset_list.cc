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

BEGIN_MODULE(panel_asset_list)

constexpr static const char *mod_name = "Panel: Asset List";

BEGIN_PANEL(pn,"Asset List")
	for (auto &&name : ns.get_asset_names()) {
		ImGui::Selectable(name.get_str().c_str(),false);
		ImGui::PushID(&name.get_asset());
		if (ImGui::BeginPopupContextItem("Asset context menu")) {
			if (ImGui::Button("Delete")) {
				BEGIN_TRANSACTION // FIXME describe
				ts.describef("Delete '$'",name);
				name.get_asset().destroy(ts);
				END_TRANSACTION
			}
			ImGui::EndPopup();
		}
		ImGui::PopID();
	}
END_PANEL

END_MODULE
