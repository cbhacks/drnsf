//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017  DRNSF contributors
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

#include "common.hh"
#include "render.hh"

namespace drnsf {
namespace render {

const float cube_vb_data[] = {
	-1, -1, -1,
	+1, -1, -1,
	+1, +1, -1,
	-1, +1, -1,
	-1, -1, +1,
	+1, -1, +1,
	+1, +1, +1,
	-1, +1, +1
};

const unsigned char cube_ib_data[] = {
	0, 1,
	0, 2,
	0, 3,
	0, 4,
	0, 5,
	0, 6,
	0, 7,
	1, 2,
	1, 3,
	1, 4,
	1, 5,
	1, 6,
	1, 7,
	2, 3,
	2, 4,
	2, 5,
	2, 6,
	2, 7,
	3, 4,
	3, 5,
	3, 6,
	3, 7,
	4, 5,
	4, 6,
	4, 7,
	5, 6,
	5, 7,
	6, 7
};

// declared in render.hh
void reticle_fig::draw(const glm::mat4 &projection,const glm::mat4 &modelview)
{
	gl::buffer vb;
	gl::buffer ib;
	gl::vert_array va;

	glPushMatrix();
	glLoadMatrixf(&modelview[0][0]);
	glScalef(200.0f,200.0f,200.0f);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadMatrixf(&projection[0][0]);

	glBindVertexArray(va);
	glBindBuffer(GL_ARRAY_BUFFER,vb);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(cube_vb_data),
		cube_vb_data,
		GL_STATIC_DRAW
	);
	glVertexPointer(3,GL_FLOAT,0,0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ib);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		sizeof(cube_ib_data),
		cube_ib_data,
		GL_STATIC_DRAW
	);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawElements(GL_LINES,56,GL_UNSIGNED_BYTE,0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindVertexArray(0);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glPopMatrix();
}

}
}
