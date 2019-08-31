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

uniform ivec3 u_Resolution;

uniform usamplerBuffer u_TreeTex;
uniform usampler3D u_FlatTex;

flat in vec3 v_LayerVec;
flat in vec3 v_PlaneXVec;
flat in vec3 v_PlaneYVec;

flat in float v_LayerPosition;

smooth in vec2 v_PositionInPlane;
smooth in vec3 v_PositionInTree;

out vec4 f_Color;

void main()
{
    vec3 stepping = 1.0 / vec3(u_Resolution);
    uint nodeA = texture(u_FlatTex, v_PositionInTree + v_LayerPosition * v_LayerVec * stepping * 0.25).r;
    uint nodeB = texture(u_FlatTex, v_PositionInTree - v_LayerPosition * v_LayerVec * stepping * 0.25).r;

    uint node;
    if (nodeA != 0U) {
        node = nodeA;
    } else if (nodeB != 0U) {
        node = nodeB;
    } else {
        node = 0U;
        discard;
    }

    uint nodeValue;
    if (node == 0x1CU) {
        nodeValue = 0U; // FIXME
    } else if (node >= 0x24U) {
        nodeValue = texelFetch(u_TreeTex, int(node - 0x24U) >> 1).r;
    } else {
        nodeValue = 0U;
    }

    f_Color = vec4(
        float((nodeValue >>  1) & 0x1FU) / 31.0,
        float((nodeValue >>  6) & 0x1FU) / 31.0,
        float((nodeValue >> 11) & 0x1FU) / 31.0,
        1.0
    );

    f_Color.rgb *= 0.8;
    f_Color.rgb += 0.1;

    vec2 absPositionInPlane = v_PositionInPlane / vec2(length(v_PlaneXVec * stepping), length(v_PlaneYVec * stepping));
    vec2 rndPositionInPlane = round(absPositionInPlane);
    vec2 absRndDiff = abs(rndPositionInPlane - absPositionInPlane);
    if (absRndDiff.x < 0.1 || absRndDiff.y < 0.1) {
        f_Color = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
