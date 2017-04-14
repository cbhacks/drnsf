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
#include "gui.hh"

namespace drnsf {
namespace gui {

im_window::im_window(const std::string &title,int width,int height) :
	m_wnd(title,width,height),
	m_canvas(m_wnd)
{
	h_frame <<= [this](int delta_time) {
		on_frame(delta_time);
	};
	h_frame.bind(m_canvas.on_frame);

	m_canvas.show();
	m_wnd.show();
}

int im_window::get_width() const
{
	return m_canvas.get_width();
}

int im_window::get_height() const
{
	return m_canvas.get_height();
}

}
}
