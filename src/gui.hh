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

#pragma once

#include "../imgui/imgui.h"
#include "sys.hh"

namespace gui {

class window : public sys::window {
protected: // FIXME
	ImGuiIO &m_io;
	int m_width;
	int m_height;
	bool m_textinput_active;

protected:
	void on_key(SDL_Keysym keysym,bool down) override;
	void on_text(const char *text) override;
	void on_mousemove(int x,int y) override;
	void on_mousewheel(int y) override;
	void on_mousebutton(int button,bool down) override;
	void on_resize(int width,int height) override;

	void on_frame(int delta_time) override;

public:
	window(const std::string &title,int width,int height);

	virtual void frame(int delta) = 0;
};

}
