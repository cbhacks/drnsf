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

namespace embed {
namespace cube_vert_glsl {
	extern const unsigned char data[];
	extern const std::size_t size;
}
namespace cube_frag_glsl {
	extern const unsigned char data[];
	extern const std::size_t size;
}
}

namespace edit {

class map_view::impl : private util::nocopy {
private:
	editor &m_ed;
	gui::gl_canvas m_canvas;
	gl::program m_cube_prog;
	gl::vert_array m_cube_va;
	gl::buffer m_cube_vb;
	gl::buffer m_cube_ib;

	bool m_mouse_down = false;
	int m_mouse_x_prev;
	int m_mouse_y_prev;

	decltype(m_canvas.on_render)::watch h_render;
	decltype(m_canvas.on_mousemove)::watch h_mousemove;
	decltype(m_canvas.on_mousewheel)::watch h_mousewheel;
	decltype(m_canvas.on_mousebutton)::watch h_mousebutton;

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

		h_mousemove <<= [this](int x,int y) {
			if (m_mouse_down) {
				int delta_x = x - m_mouse_x_prev;
				int delta_y = y - m_mouse_y_prev;

				g_camera_yaw += delta_x;

				g_camera_pitch += delta_y;
				if (g_camera_pitch > 90.0f) {
					g_camera_pitch = 90.0f;
				} else if (g_camera_pitch < -90.0f) {
					g_camera_pitch = -90.0f;
				}
				m_canvas.invalidate();//FIXME remove
			}

			m_mouse_x_prev = x;
			m_mouse_y_prev = y;
		};
		h_mousemove.bind(m_canvas.on_mousemove);

		h_mousewheel <<= [this](int delta_y) {
			g_camera_zoom -= g_camera_zoom * 0.1 * delta_y;
			if (g_camera_zoom < 500.0f) {
				g_camera_zoom = 500.0f;
			}
			m_canvas.invalidate();//FIXME remove
		};
		h_mousewheel.bind(m_canvas.on_mousewheel);

		h_mousebutton <<= [this](int button,bool down) {
			if (button == 1) {
				m_mouse_down = down;
			}
		};
		h_mousebutton.bind(m_canvas.on_mousebutton);

		{
			using std::string;
			gl::vert_shader cube_vert_shader;
			cube_vert_shader.compile({
				reinterpret_cast<const char *>(
					embed::cube_vert_glsl::data
				),
				embed::cube_vert_glsl::size
			});
			glAttachShader(m_cube_prog,cube_vert_shader);
		}

		{
			gl::frag_shader cube_frag_shader;
			cube_frag_shader.compile({
				reinterpret_cast<const char *>(
					embed::cube_frag_glsl::data
				),
				embed::cube_frag_glsl::size
			});
			glAttachShader(m_cube_prog,cube_frag_shader);
		}

		glLinkProgram(m_cube_prog);

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
	glClearColor(0,0,0,0); //FIXME remove

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto projection = glm::perspective(
		70.0f,
		static_cast<float>(width) / height,
		500.0f,
		200000.0f
	);
	projection = glm::translate(projection,glm::vec3(0.0f,0.0f,-800.0f));
	projection = glm::translate(
		projection,
		glm::vec3(0.0f,0.0f,-g_camera_zoom)
	);

	glPushMatrix();
	glRotatef(g_camera_pitch,1,0,0);
	glRotatef(g_camera_yaw,0,1,0);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadMatrixf(&projection[0][0]);

	glUseProgram(m_cube_prog);
	glBindVertexArray(m_cube_va);
	glEnableClientState(GL_VERTEX_ARRAY);
	glDrawElements(GL_LINES,56,GL_UNSIGNED_BYTE,0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindVertexArray(0);
	glUseProgram(0);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
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
