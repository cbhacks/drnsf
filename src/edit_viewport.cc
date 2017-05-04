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

namespace drnsf {
namespace edit {

// (inner class) impl
// Implementation class for viewport (PIMPL).
class viewport::impl : private util::nocopy {
	friend class viewport;

private:
	// (var) m_outer
	// A reference to the outer viewport.
	viewport &m_outer;

	// (var) m_canvas
	// The viewport's OpenGL canvas.
	gui::gl_canvas m_canvas;

	// (handler) h_render
	// Hook for the GL canvas on_render event.
	decltype(m_canvas.on_render)::watch h_render;

	// (func) render
	// Non-inline implementation for the h_render hook.
	void render(int width,int height);

public:
	// (explicit ctor)
	// Initializes the viewport widget.
	explicit impl(
		viewport &outer,
		gui::container &parent) :
		m_outer(outer),
		m_canvas(parent)
	{
		h_render <<= [this](int width,int height) {
			render(width,height);
		};
		h_render.bind(m_canvas.on_render);
	}
};

// declared previously in this file
void viewport::impl::render(int width,int height)
{
	glClearColor(0,0,0,0); //FIXME not necessary once garbage is removed
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// TODO
}

// declared in edit.hh
viewport::viewport(gui::container &parent)
{
	M = new impl(*this,parent);
}

// declared in edit.hh
viewport::~viewport()
{
	delete M;
}

// declared in edit.hh
void viewport::show()
{
	M->m_canvas.show();
}

}
}
