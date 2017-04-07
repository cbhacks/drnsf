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

BEGIN_MODULE

bool visible = false;

void frame(int delta) override
{
	ImGui::Checkbox("Camera Control",&visible);

	if (!visible)
		return;

	ImGui::Begin("Camera Control",&visible);

	ImGui::InputFloat("Pitch",&cam().pitch,5);
	ImGui::InputFloat("Lens Near",&cam().lens_near,0.1);
	ImGui::InputFloat("Lens Far",&cam().lens_far,10);
	ImGui::InputFloat("Lens Adjust",&cam().lens_adjust,0.05);
	ImGui::InputFloat("Lens Focus",&cam().lens_focus,0.01);

	ImGui::End();
}

END_MODULE

std::unique_ptr<edit::module> create_mod_camera_ctrl(edit::core &core)
{
	return std::unique_ptr<edit::module>(new mod(core));
}
