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
uniform ivec3 u_Resolution;

flat out vec3 v_LayerVec;
flat out vec3 v_PlaneXVec;
flat out vec3 v_PlaneYVec;

flat out float v_LayerPosition;

smooth out vec2 v_PositionInPlane;
smooth out vec3 v_PositionInTree;

void main()
{
    // Decide which position in the plane this vertex should appear. Each set
    // of 6 vertices (2 triangles) makes up one 1x1 plane.
    switch (gl_VertexID % 6) {
    case 0:
    case 5:
        v_PositionInPlane = vec2(0.0, 0.0);
        break;

    case 1:
        v_PositionInPlane = vec2(0.0, 1.0);
        break;

    case 2:
    case 3:
        v_PositionInPlane = vec2(1.0, 1.0);
        break;

    case 4:
        v_PositionInPlane = vec2(1.0, 0.0);
        break;
    }

    int planeID = gl_VertexID / 6;
    if (planeID < u_Resolution.x + 1) {
        int layerID = planeID;
        v_LayerPosition = float(layerID) / float(u_Resolution.x);
        v_LayerVec = vec3(1.0, 0.0, 0.0);
        v_PlaneXVec = vec3(0.0, 1.0, 0.0);
        v_PlaneYVec = vec3(0.0, 0.0, 1.0);
    } else if (planeID < u_Resolution.x + u_Resolution.y + 2) {
        int layerID = planeID - u_Resolution.x - 1;
        v_LayerPosition = float(layerID) / float(u_Resolution.y);
        v_LayerVec = vec3(0.0, 1.0, 0.0);
        v_PlaneXVec = vec3(0.0, 0.0, 1.0);
        v_PlaneYVec = vec3(1.0, 0.0, 0.0);
    } else {
        int layerID = planeID - u_Resolution.x - u_Resolution.y - 2;
        v_LayerPosition = float(layerID) / float(u_Resolution.z);
        v_LayerVec = vec3(0.0, 0.0, 1.0);
        v_PlaneXVec = vec3(1.0, 0.0, 0.0);
        v_PlaneYVec = vec3(0.0, 1.0, 0.0);
    }

    v_PositionInTree =
        v_LayerVec * v_LayerPosition +
        v_PlaneXVec * v_PositionInPlane.x +
        v_PlaneYVec * v_PositionInPlane.y;

    gl_Position = u_Matrix * vec4(v_PositionInTree, 1.0);
}
