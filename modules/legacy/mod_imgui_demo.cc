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

constexpr static const char *mod_name = "ImGui Demo";

bool show_demo_wnd = false;

void frame(int delta) override
{
	if (show_demo_wnd) {
		ImGui::ShowTestWindow(&show_demo_wnd);
	}
}

void show_tools_menu() override
{
	if (ImGui::MenuItem("Show ImGui Demo")) {
		show_demo_wnd = true;
	}
}

END_MODULE
