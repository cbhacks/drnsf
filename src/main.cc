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
#include <SDL.h>
#include <GL/gl.h>
#include <iostream>
#include "edit.hh"
#include "sys.hh"

#define DISPLAYWIDTH  800
#define DISPLAYHEIGHT 600

class main_window : public sys::window {
private:
	edit::core m_editor;

public:
	main_window() :
		window("DRNSF",DISPLAYWIDTH,DISPLAYHEIGHT) {}

	edit::core &get_editor() { return m_editor; }

	void on_key(SDL_Keysym keysym,bool down) override;
	void on_text(const char *text) override;
	void on_mousemove(int x,int y) override;
	void on_mousewheel(int y) override;
	void on_mousebutton(int button,bool down) override;
	void on_resize(int width,int height) override;

	void on_frame(int delta_time) override;
};

void main_window::on_key(SDL_Keysym keysym,bool down)
{
	m_editor.key(keysym.sym,down);
}

void main_window::on_text(const char *text)
{
	m_editor.text(text);
}

void main_window::on_mousemove(int x,int y)
{
	m_editor.mouse_move(x,y);
}

void main_window::on_mousewheel(int y)
{
	m_editor.mouse_scroll(y);
}

void main_window::on_mousebutton(int button,bool down)
{
	m_editor.mouse_button(button,down);
}

void main_window::on_resize(int width,int height)
{
	m_editor.window_resize(width,height);
}

void main_window::on_frame(int delta_time)
{
	m_editor.frame(delta_time);
}

int main(int argc,char *argv[])
{
	// Initialize SDL.
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		std::cerr <<
			"Error initializing SDL: " <<
			SDL_GetError() <<
			std::endl;
		return EXIT_FAILURE;
	}

	// Create the main window.
	main_window wnd;

	// Inform the editor about the initial window size.
	wnd.get_editor().window_resize(DISPLAYWIDTH,DISPLAYHEIGHT);

	// Run the main application/game loop.
	while (true) {
		wnd.run_once();
	}

	// Clean-up.
	SDL_Quit();

	return EXIT_SUCCESS;
}
