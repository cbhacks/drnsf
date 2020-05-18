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

uniform usampler2D u_Textures[8];

smooth in vec3 v_Color;
smooth in vec2 v_Texcoord;
flat in int v_TexpageIndex;
flat in int v_Texinfo;

out vec4 f_Color;

int bitfieldExtract(int a, int b, int c)
{
  int mask = ((1 << c) - 1);
  return (a >> b) & mask;
}

vec3 unpackBGR555(int value)
{
    return vec3(
        bitfieldExtract(value, 0, 5) / 32.0,
        bitfieldExtract(value, 5, 5) / 32.0,
        bitfieldExtract(value, 10, 5) / 32.0
    );
}

int sample16(ivec2 uv)
{
    switch (v_TexpageIndex) {
    case 0: return int(texelFetch(u_Textures[0], uv, 0).r);
    case 1: return int(texelFetch(u_Textures[1], uv, 0).r);
    case 2: return int(texelFetch(u_Textures[2], uv, 0).r);
    case 3: return int(texelFetch(u_Textures[3], uv, 0).r);
    case 4: return int(texelFetch(u_Textures[4], uv, 0).r);
    case 5: return int(texelFetch(u_Textures[5], uv, 0).r);
    case 6: return int(texelFetch(u_Textures[6], uv, 0).r);
    case 7: return int(texelFetch(u_Textures[7], uv, 0).r);
    }
}

int sample8(ivec2 uv, int clut_y)
{
    int texel = sample16(ivec2(uv.x / 2, uv.y));
    int cindex = bitfieldExtract(texel, (uv.x & 1) * 8, 8);

    return sample16(ivec2(cindex, clut_y));
}

int sample4(ivec2 uv, int clut_y, int clut_x)
{
    int texel = sample16(ivec2(uv.x / 4, uv.y));
    int cindex = bitfieldExtract(texel, (uv.x & 3) * 4, 4);

    return sample16(ivec2(cindex + clut_x * 16, clut_y));
}

void main()
{
    f_Color = vec4(v_Color, 1.0);

    if (v_Texinfo != 0) {
        int color_mode = bitfieldExtract(v_Texinfo, 0, 2);
        int clut_x = bitfieldExtract(v_Texinfo, 19, 4);
        int clut_y = bitfieldExtract(v_Texinfo, 23, 7);
        int page_width = 128 * (2 << (2 - color_mode));
        ivec2 texcoord = ivec2(v_Texcoord.x * page_width, v_Texcoord.y * 128);

        int texcolor;
        if (color_mode == 0) {
            texcolor = sample4(texcoord, clut_y, clut_x);
        } else if (color_mode == 1) {
            texcolor = sample8(texcoord, clut_y);
        } else {
            texcolor = sample16(texcoord);
        }

        if (texcolor == 0) {
            discard;
        } else {
            f_Color.rgb *= 2.0 * unpackBGR555(texcolor);
        }
    }
}
