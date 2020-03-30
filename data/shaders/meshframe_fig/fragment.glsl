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

uniform isampler2D u_Textures[8];

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

vec4 sample16(ivec2 uv)
{
  ivec2 coords = ivec2(uv.x / 2, uv.y);
  ivec4 texel = texelFetch(u_Textures[v_TexpageIndex], coords, 0);
  int pixel = ((uv.x & 1) == 1) ? (texel.a << 8) + texel.b : (texel.g << 8) + texel.r;

  return vec4(
    ((pixel >> 0) & 0x1F) / 32.0f,
    ((pixel >> 5) & 0x1F) / 32.0f,
    ((pixel >> 10) & 0x1F) / 32.0f,
    ((pixel >> 15) & 1) / 1.0f
  );
}

vec4 sample8(ivec2 uv, int clut_y) 
{
  ivec2 coords = ivec2(uv.x / 4, uv.y);
  ivec4 texel = texelFetch(u_Textures[v_TexpageIndex], coords, 0);
  int cindex = ((uv.x & 2) == 2) ? ((uv.x & 1) == 1) ? texel.a : texel.b 
                                 : ((uv.x & 1) == 1) ? texel.g : texel.r;
  
  return sample16(ivec2(cindex, clut_y));
}

vec4 sample4(ivec2 uv, int clut_y, int clut_x)
{
  ivec2 coords = ivec2(uv.x / 8, uv.y);
  ivec4 texel = texelFetch(u_Textures[v_TexpageIndex], coords, 0);
  int cindex = ((uv.x & 4) == 4) ? 
        ((uv.x & 2) == 2) ? ((uv.x & 1) != 1) ? texel.a & 0xF : (texel.a >> 4) & 0xF
                          : ((uv.x & 1) != 1) ? texel.b & 0xF : (texel.b >> 4) & 0xF
      : ((uv.x & 2) == 2) ? ((uv.x & 1) != 1) ? texel.g & 0xF : (texel.g >> 4) & 0xF
                          : ((uv.x & 1) != 1) ? texel.r & 0xF : (texel.r >> 4) & 0xF;

  return sample16(ivec2(cindex + clut_x * 16, clut_y));
}

void main()
{
    if (v_Texinfo == 0) {
        f_Color = vec4(v_Color, 1.0);
    }
    else {
        int color_mode = bitfieldExtract(v_Texinfo, 0, 2);
        int clut_x = bitfieldExtract(v_Texinfo, 19, 4);
        int clut_y = bitfieldExtract(v_Texinfo, 23, 7);
        int page_width = 128 * (2 << (2 - color_mode));
        ivec2 texcoord = ivec2(v_Texcoord.x * page_width, v_Texcoord.y * 128);
        if (color_mode == 0) {
        	f_Color = sample4(texcoord, clut_y, clut_x);
        } else if (color_mode == 1) {
        	f_Color = sample8(texcoord, clut_y);
        } else {
            f_Color = sample16(texcoord);
        }
    }
}
