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
#include "../imgui/imgui.h"
#include "gui.hh"

#define DISPLAYWIDTH  800
#define DISPLAYHEIGHT 600

namespace gui {

window::window() :
	sys::window("DRNSF",DISPLAYWIDTH,DISPLAYHEIGHT),
	m_io(ImGui::GetIO())
{
	// FIXME window_resize(DISPLAYWIDTH,DISPLAYHEIGHT);
}

void window::on_key(SDL_Keysym keysym,bool down)
{
	key(keysym.sym,down);
}

void window::on_text(const char *text)
{
	this->text(text);
}

void window::on_mousemove(int x,int y)
{
	mouse_move(x,y);
}

void window::on_mousewheel(int y)
{
	mouse_scroll(y);
}

void window::on_mousebutton(int button,bool down)
{
	mouse_button(button,down);
}

void window::on_resize(int width,int height)
{
	window_resize(width,height);
}

void window::on_frame(int delta_time)
{
	frame(delta_time);
}

}
