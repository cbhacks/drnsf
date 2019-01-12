//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2019  DRNSF contributors
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

#version 140
#extension GL_ARB_shader_bit_encoding: require

uniform mat4 u_Matrix;

// TODO - uniform block
uniform isamplerBuffer u_VertexList;
uniform samplerBuffer u_ColorList;
uniform int u_ColorCount;

in int a_VertexIndex;
in int a_ColorIndex;

smooth out vec3 v_Color;

vec3 colorFetch(int i)
{
    float r = texelFetch(u_ColorList, i * 3 + 0).r;
    float g = texelFetch(u_ColorList, i * 3 + 1).r;
    float b = texelFetch(u_ColorList, i * 3 + 2).r;
    return vec3(r, g, b);
}

void main()
{
    int x_i         = texelFetch(u_VertexList, a_VertexIndex * 5 + 0).r;
    int y_i         = texelFetch(u_VertexList, a_VertexIndex * 5 + 1).r;
    int z_i         = texelFetch(u_VertexList, a_VertexIndex * 5 + 2).r;
    // FIXME - fx is unsigned in gfx.hh
    int fx          = texelFetch(u_VertexList, a_VertexIndex * 5 + 3).r;
    int color_index = texelFetch(u_VertexList, a_VertexIndex * 5 + 4).r;

    float x = intBitsToFloat(x_i);
    float y = intBitsToFloat(y_i);
    float z = intBitsToFloat(z_i);

    gl_Position = u_Matrix * vec4(x, y, z, 1.0);

    v_Color = vec3(1.0, 1.0, 1.0);

    if (a_ColorIndex >= 0 && a_ColorIndex < u_ColorCount) {
        v_Color *= colorFetch(a_ColorIndex);
    }

    if (color_index >= 0 && color_index < u_ColorCount) {
        v_Color *= colorFetch(color_index);
    }
}
