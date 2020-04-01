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
in int ai_TexpageIndex;
in int ai_TexinfoIndex;

flat out int ao_Index0;
flat out int ao_VertexIndex0;
flat out int ao_ColorIndex0;
flat out int ao_TexpageIndex0;
flat out int ao_TexinfoIndex0;
flat out int ao_Index1;
flat out int ao_VertexIndex1;
flat out int ao_ColorIndex1;
flat out int ao_TexpageIndex1;
flat out int ao_TexinfoIndex1;
flat out int ao_Index2;
flat out int ao_VertexIndex2;
flat out int ao_ColorIndex2;
flat out int ao_TexpageIndex2;
flat out int ao_TexinfoIndex2;

void main()
{
    ao_Index0 = 2;
    ao_VertexIndex0 = ai_VertexIndex0;
    ao_ColorIndex0 = ai_ColorIndex0;
    ao_TexpageIndex0 = ai_TexpageIndex;
    ao_TexinfoIndex0 = ai_TexinfoIndex;
    ao_Index1 = 1;
    ao_VertexIndex1 = ai_VertexIndex1;
    ao_ColorIndex1 = ai_ColorIndex1;
    ao_TexpageIndex1 = ai_TexpageIndex;
    ao_TexinfoIndex1 = ai_TexinfoIndex;
    ao_Index2 = 0;
    ao_VertexIndex2 = ai_VertexIndex2;
    ao_ColorIndex2 = ai_ColorIndex2;
    ao_TexpageIndex2 = ai_TexpageIndex;
    ao_TexinfoIndex2 = ai_TexinfoIndex;
}
