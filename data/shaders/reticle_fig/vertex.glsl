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

const vec4 SCALE = vec4(200.0,200.0,200.0,1.0);

uniform mat4 u_Matrix;

in vec4 a_Position;
in int a_Axis;

flat out int v_Axis;
smooth out float v_Value;

void main()
{
    gl_Position = u_Matrix * (a_Position * SCALE);
    v_Axis = a_Axis;
    v_Value = a_Position.x + a_Position.y + a_Position.z;
}
