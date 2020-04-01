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

uniform mat4 u_Matrix;

// TODO - uniform block
uniform isamplerBuffer u_VertexList;
uniform isamplerBuffer u_TexinfoList;
uniform samplerBuffer u_ColorList;
uniform int u_ColorCount;

in int a_Index;
in int a_VertexIndex;
in int a_ColorIndex;
in int a_TexpageIndex;
in int a_TexinfoIndex;

smooth out vec3 v_Color;
smooth out vec2 v_Texcoord;
flat out int v_TexpageIndex;
flat out int v_Texinfo;

int bitfieldExtract(int a, int b, int c)
{
  int mask = ((1 << c) - 1);
  return (a >> b) & mask;
}

vec3 colorFetch(int i)
{
    float r = texelFetch(u_ColorList, i * 3 + 0).r;
    float g = texelFetch(u_ColorList, i * 3 + 1).r;
    float b = texelFetch(u_ColorList, i * 3 + 2).r;
    return vec3(r, g, b);
}

vec3 intToRGB(int i)
{
    float r = ((i & 0xFF0000) >> 16) / 255.0;
    float g = ((i & 0xFF00) >> 8) / 255.0;
    float b = ((i & 0xFF) >> 0) / 255.0;
    return vec3(r, g, b);
}

void main()
{
    int x_i         = texelFetch(u_VertexList, a_VertexIndex * 5 + 0).r;
    int y_i         = texelFetch(u_VertexList, a_VertexIndex * 5 + 1).r;
    int z_i         = texelFetch(u_VertexList, a_VertexIndex * 5 + 2).r;
    int fx          = texelFetch(u_VertexList, a_VertexIndex * 5 + 3).r;
    int color_index = texelFetch(u_VertexList, a_VertexIndex * 5 + 4).r;

    float x = float(x_i);
    float y = float(y_i);
    float z = float(z_i);

    gl_Position = u_Matrix * vec4(x, y, z, 1.0);

    v_Color = vec3(1.0, 1.0, 1.0);

    if (a_ColorIndex >= 0 && a_ColorIndex < u_ColorCount) {
        v_Color *= colorFetch(a_ColorIndex);
    }

    if ((fx & 4) == 4) {
        v_Color *= intToRGB(color_index);
    } 
    else if (color_index >= 0 && color_index < u_ColorCount) {
        v_Color *= colorFetch(color_index);
    }

    int texinfoa_i = texelFetch(u_TexinfoList, a_TexinfoIndex * 2 + 0).r;
    int texinfob_i = texelFetch(u_TexinfoList, a_TexinfoIndex * 2 + 1).r;

    const ivec2[3] pos_masks = ivec2[3](ivec2(0x30ff0c, 0xf3cc30),
                                        ivec2(0x8799e1, 0x9e7186),
                                        ivec2(0x4b66d2, 0x6db249));

    int segment = bitfieldExtract(texinfob_i, 2, 2);
    int color_mode = bitfieldExtract(texinfob_i, 0, 2);
    int x_offsu = bitfieldExtract(texinfob_i, 4, 5);
    int y_offsu = bitfieldExtract(texinfoa_i, 3, 5);
    int region_index = bitfieldExtract(texinfob_i, 9, 10);

    int ppi = (2 << (2 - color_mode));
    int fw = region_index / 25;
    ivec2 wh = ivec2((4 << (region_index % 5)), (4 << ((region_index / 5) % 5)));
    
    float page_width = float(128 * ppi);
    int x_offs = ((segment * 32) + x_offsu) * ppi;
    int y_offs = y_offsu * 4;
    v_Texcoord = vec2(
      float(x_offs + (wh.x * bitfieldExtract(pos_masks[a_Index].x, fw, 1))) / page_width,
      float(y_offs + (wh.y * bitfieldExtract(pos_masks[a_Index].y, fw, 1))) / 128.0
    );

    v_Texinfo = texinfob_i;
    v_TexpageIndex = a_TexpageIndex;
}
