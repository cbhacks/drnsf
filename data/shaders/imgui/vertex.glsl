//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2020  DRNSF contributors
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

uniform mat4 u_ScreenOrtho;

in vec4 a_Position;
in vec2 a_TexCoord;
in vec4 a_Color;

smooth out vec2 v_TexCoord;
smooth out vec4 v_Color;

void main()
{
    gl_Position = u_ScreenOrtho * a_Position;
    v_TexCoord = a_TexCoord;
    v_Color = a_Color;
}
