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

#version 110

uniform mat4 u_Projection;

attribute vec2 a_Position;
attribute vec2 a_TexCoord;
attribute vec4 a_Color;

varying vec4 v_Color;
varying vec2 v_TexCoord;

void main()
{
	gl_Position = u_Projection * vec4(u_Position,0.0,1.0);
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
}
