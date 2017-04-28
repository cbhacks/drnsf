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
#include "edit.hh"
#include "gfx.hh"

namespace drnsf {
namespace edit {

class map_view::impl : private util::nocopy {
private:
	editor &m_ed;
	gui::gl_canvas m_canvas;

	decltype(m_canvas.on_render)::watch h_render;

	void render();

public:
	explicit impl(gui::container &parent,editor &ed) :
		m_ed(ed),
		m_canvas(parent)
	{
		h_render <<= [this](int width,int height) {
			render();
		};
		h_render.bind(m_canvas.on_render);
	}

	void show()
	{
		m_canvas.show();
	}
};

void map_view::impl::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
