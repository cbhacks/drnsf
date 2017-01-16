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
#include <sstream>
#include "begin.hh"

constexpr static const char *mod_name = "Undo/Redo";

void show_edit_menu() override
{
	// Edit -> Undo
	if (nx.has_undo()) {
		std::ostringstream text;
		text << "Undo: " << nx.get_undo().describe();
		if (ImGui::MenuItem(text.str().c_str(),"Ctrl+Z",false,true)) {
			nx.undo();
		}
	} else {
		ImGui::MenuItem("Undo","Ctrl+Z",false,false);
	}

	// Edit -> Redo
	if (nx.has_redo()) {
		std::ostringstream text;
		text << "Redo: " << nx.get_redo().describe();
		if (ImGui::MenuItem(text.str().c_str(),"Ctrl+Y",false,true)) {
			nx.redo();
		}
	} else {
		ImGui::MenuItem("Redo","Ctrl+Y",false,false);
	}
}

END_MODULE
