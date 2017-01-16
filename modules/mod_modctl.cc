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

BEGIN_MODULE(mod_modctl)

constexpr static const char *mod_name = "Module Control";

bool visible = false;

void frame(int delta) override
{
	// Exit now if the modules window is not open.
	if (!visible)
		return;

	// Begin the modules window. Exit early if the window is minimized.
	if (!ImGui::Begin("Modules",&visible)) {
		ImGui::End();
		return;
	}

	// Iterate over all of the available modules, both enabled and not.
	for (auto &&info : edit::module_info::get_set()) {
		ImGui::PushID(info);

		ImGui::Text(info->get_name());

		ImGui::PopID();
	}

	// Finish the window.
	ImGui::End();
}

void show_tools_menu() override
{
	// Display a tool menu option to display the modules window.
	if (ImGui::MenuItem("Modules...")) {
		visible = true;
	}
}

END_MODULE
