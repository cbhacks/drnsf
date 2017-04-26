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
#include <epoxy/gl.h>
#include "edit.hh"
#include "gfx.hh"

namespace drnsf {
namespace edit {

map_view::map_view(gui::container &parent,editor &ed) :
	m_ed(ed),
	m_canvas(parent)
{
	h_render <<= [this]{
		m_canvas.post_job([]{
			glClear(
				GL_COLOR_BUFFER_BIT |
				GL_DEPTH_BUFFER_BIT
			);
		});

		for (auto &&asset_p : m_ed.m_proj.get_asset_list()) {
			auto *model = dynamic_cast<gfx::model *>(
				asset_p.get()
			);

			if (!model) continue;

			auto anim = model->get_anim().get();
			if (!anim) continue;

			auto mesh = model->get_mesh().get();
			if (!mesh) continue;

			auto &&frames = anim->get_frames();
			if (frames.empty()) continue;

			auto frame = frames[0].get();
			if (!frame) continue;
		}
	};
	h_render.bind(m_canvas.on_render);
}

void map_view::show()
{
	m_canvas.show();
}

}
}
