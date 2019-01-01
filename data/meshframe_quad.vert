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

in int ai_VertexIndex0;
in int ai_ColorIndex0;
in int ai_VertexIndex1;
in int ai_ColorIndex1;
in int ai_VertexIndex2;
in int ai_ColorIndex2;
in int ai_VertexIndex3;
in int ai_ColorIndex3;

flat out int ao_VertexIndexA0;
flat out int ao_ColorIndexA0;
flat out int ao_VertexIndexA1;
flat out int ao_ColorIndexA1;
flat out int ao_VertexIndexA2;
flat out int ao_ColorIndexA2;

flat out int ao_VertexIndexB0;
flat out int ao_ColorIndexB0;
flat out int ao_VertexIndexB1;
flat out int ao_ColorIndexB1;
flat out int ao_VertexIndexB2;
flat out int ao_ColorIndexB2;

void main()
{
    ao_VertexIndexA0 = ai_VertexIndex0;
    ao_ColorIndexA0 = ai_ColorIndex0;
    ao_VertexIndexA1 = ai_VertexIndex1;
    ao_ColorIndexA1 = ai_ColorIndex1;
    ao_VertexIndexA2 = ai_VertexIndex2;
    ao_ColorIndexA2 = ai_ColorIndex2;

    ao_VertexIndexB0 = ai_VertexIndex2;
    ao_ColorIndexB0 = ai_ColorIndex2;
    ao_VertexIndexB1 = ai_VertexIndex1;
    ao_ColorIndexB1 = ai_ColorIndex1;
    ao_VertexIndexB2 = ai_VertexIndex3;
    ao_ColorIndexB2 = ai_ColorIndex3;
}
