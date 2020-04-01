//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2020 DRNSF contributors
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

in int ai_VertexIndex0;
in int ai_ColorIndex0;
in int ai_VertexIndex1;
in int ai_ColorIndex1;
in int ai_VertexIndex2;
in int ai_ColorIndex2;
in int ai_VertexIndex3;
in int ai_ColorIndex3;
in int ai_TexpageIndex;
in int ai_TexinfoIndex;

flat out int ao_IndexA0;
flat out int ao_VertexIndexA0;
flat out int ao_ColorIndexA0;
flat out int ao_TexpageIndexA0;
flat out int ao_TexinfoIndexA0;
flat out int ao_IndexA1;
flat out int ao_VertexIndexA1;
flat out int ao_ColorIndexA1;
flat out int ao_TexpageIndexA1;
flat out int ao_TexinfoIndexA1;
flat out int ao_IndexA2;
flat out int ao_VertexIndexA2;
flat out int ao_ColorIndexA2;
flat out int ao_TexpageIndexA2;
flat out int ao_TexinfoIndexA2;

flat out int ao_IndexB0;
flat out int ao_VertexIndexB0;
flat out int ao_ColorIndexB0;
flat out int ao_TexpageIndexB0;
flat out int ao_TexinfoIndexB0;
flat out int ao_IndexB1;
flat out int ao_VertexIndexB1;
flat out int ao_ColorIndexB1;
flat out int ao_TexpageIndexB1;
flat out int ao_TexinfoIndexB1;
flat out int ao_IndexB2;
flat out int ao_VertexIndexB2;
flat out int ao_ColorIndexB2;
flat out int ao_TexpageIndexB2;
flat out int ao_TexinfoIndexB2;

void main()
{
    ao_IndexA0 = 2;
    ao_VertexIndexA0 = ai_VertexIndex0;
    ao_ColorIndexA0 = ai_ColorIndex0;
    ao_TexpageIndexA0 = ai_TexpageIndex;
    ao_TexinfoIndexA0 = ai_TexinfoIndex;
    ao_IndexA1 = 1;
    ao_VertexIndexA1 = ai_VertexIndex1;
    ao_ColorIndexA1 = ai_ColorIndex1;
    ao_TexpageIndexA1 = ai_TexpageIndex;
    ao_TexinfoIndexA1 = ai_TexinfoIndex;
    ao_IndexA2 = 0;
    ao_VertexIndexA2 = ai_VertexIndex2;
    ao_ColorIndexA2 = ai_ColorIndex2;
    ao_TexpageIndexA2 = ai_TexpageIndex;
    ao_TexinfoIndexA2 = ai_TexinfoIndex;

    ao_IndexB0 = 3;
    ao_VertexIndexB0 = ai_VertexIndex2;
    ao_ColorIndexB0 = ai_ColorIndex2;
    ao_TexpageIndexB0 = ai_TexpageIndex;
    ao_TexinfoIndexB0 = ai_TexinfoIndex;
    ao_IndexB1 = 1;
    ao_VertexIndexB1 = ai_VertexIndex1;
    ao_ColorIndexB1 = ai_ColorIndex1;
    ao_TexpageIndexB1 = ai_TexpageIndex;
    ao_TexinfoIndexB1 = ai_TexinfoIndex;
    ao_IndexB2 = 2;
    ao_VertexIndexB2 = ai_VertexIndex3;
    ao_ColorIndexB2 = ai_ColorIndex3;
    ao_TexpageIndexB2 = ai_TexpageIndex;
    ao_TexinfoIndexB2 = ai_TexinfoIndex;
}
