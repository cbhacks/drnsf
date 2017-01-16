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

#pragma once

#include "../imgui/imgui.h"

namespace gui {

static void tooltip(const std::string &text)
{
	if (!ImGui::IsItemHovered())
		return;

	ImGui::BeginTooltip();
	ImGui::PushTextWrapPos(200);
	ImGui::Text(text.c_str());
	ImGui::PopTextWrapPos();
	ImGui::EndTooltip();
}

static void hint(const std::string &text)
{
	ImGui::TextDisabled("(?)");
	tooltip(text);
}

template <typename T>
static bool asset_ref_input(res::ref<T> &ref)
{
	if (ref == nullptr) {
		ImGui::Text("(null)");
	} else {
		ImGui::Text(ref.c_str());
		ImGui::SameLine();
		if (ref.ok()) {
			ImGui::TextColored({0,1,0,1},"OK");
		} else if (static_cast<res::name>(ref).has_asset()) {
			ImGui::TextColored({1,0.5,0,1},"Type mismatch");
			ImGui::SameLine();
			hint(
				"An asset exists with this name, but its type "
				"is not acceptable for this field."
			);
		} else {
			ImGui::TextColored({1,0,0,1},"No such asset");
		}
	}
	return false;
}

}
