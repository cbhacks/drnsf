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
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "edit.hh"
#include "gfx.hh"

namespace drnsf {
namespace edit {

class map_view::impl : private util::nocopy {
private:
	editor &m_ed;
	gui::gl_canvas m_canvas;
	gl::vert_array m_cube_va;
	gl::buffer m_cube_vb;
	gl::buffer m_cube_ib;

	decltype(m_canvas.on_render)::watch h_render;

	void render(int width,int height);

public:
	explicit impl(gui::container &parent,editor &ed) :
		m_ed(ed),
		m_canvas(parent)
	{
		h_render <<= [this](int width,int height) {
			render(width,height);
		};
		h_render.bind(m_canvas.on_render);

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

		glBindVertexArray(m_cube_va);
		glBindBuffer(GL_ARRAY_BUFFER,m_cube_vb);
		glBufferData(
			GL_ARRAY_BUFFER,
			sizeof(cube_vb_data),
			cube_vb_data,
			GL_STATIC_DRAW
		);
		glVertexPointer(3,GL_FLOAT,0,0);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m_cube_ib);
		glBufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			sizeof(cube_ib_data),
			cube_ib_data,
			GL_STATIC_DRAW
		);
		glBindVertexArray(0);
	}

	void show()
	{
		m_canvas.show();
	}
};

void map_view::impl::render(int width,int height)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float norm_width = 1;
	float norm_height = (float)height / width;
	if (norm_height < 1) {
		norm_width /= norm_height;
		norm_height = 1;
	}

	float zoom = 1.8;

	auto projection = glm::frustum(
		-norm_width * zoom,
		+norm_width * zoom,
		-norm_height * zoom,
		+norm_height * zoom,
		1.8f,
		200.0f
	);
	projection = glm::translate(projection,glm::vec3(0.0f,0.0f,-5.4));

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadMatrixf(&projection[0][0]);

	glBindVertexArray(m_cube_va);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawElements(GL_LINES,56,GL_UNSIGNED_BYTE,0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindVertexArray(0);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

map_view::map_view(gui::container &parent,editor &ed)
{
	M = new impl(parent,ed);
}

map_view::~map_view()
{
	delete M;
}

void map_view::show()
{
	M->show();
}

}
}
