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
// Implementation class for asset_viewport (PIMPL).
class asset_viewport::impl : private util::nocopy {
	friend class asset_viewport;

private:
	// (var) m_outer
	// A reference to the outer asset_viewport.
	asset_viewport &m_outer;

	// (var) m_proj
	// A reference to the project this viewport applies to.
	res::project &m_proj;

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
		asset_viewport &outer,
		gui::container &parent,
		res::project &proj) :
		m_outer(outer),
		m_proj(proj),
		m_canvas(parent)
	{
		h_render <<= [this](int width,int height) {
			render(width,height);
		};
		h_render.bind(m_canvas.on_render);
	}
};

// declared previously in this file
void asset_viewport::impl::render(int width,int height)
{
	glClearColor(0,0,0,0); //FIXME not necessary once garbage is removed
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// TODO
}

// declared in edit.hh
asset_viewport::asset_viewport(gui::container &parent,res::project &proj)
{
	M = new impl(*this,parent,proj);
}

// declared in edit.hh
asset_viewport::~asset_viewport()
{
	delete M;
}

// declared in edit.hh
void asset_viewport::show()
{
	M->m_canvas.show();
}

}
}
