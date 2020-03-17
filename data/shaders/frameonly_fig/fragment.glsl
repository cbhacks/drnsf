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

uniform int u_MarkerID;

flat in int v_VertexID;

out vec4 f_Color;
out ivec2 f_VertexMarking;

void main()
{
    float dist = length(gl_PointCoord - 0.5) * 2.0;

    if (dist > 1.0)
        discard;

    gl_FragDepth = gl_FragCoord.z - 0.0001;

    f_Color = vec4(1.0,1.0,1.0,1.0);
    f_VertexMarking = ivec2(u_MarkerID, v_VertexID);
}
