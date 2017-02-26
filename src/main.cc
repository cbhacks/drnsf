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

#define DISPLAYWIDTH  800
#define DISPLAYHEIGHT 600

class main_window : util::not_copyable {
private:
	SDL_Window *m_wnd;
	SDL_GLContext m_glctx;
	edit::core m_editor;

public:
	explicit main_window(SDL_Window *wnd,SDL_GLContext glctx);

	edit::core &get_editor() { return m_editor; }

	void on_key(SDL_Keysym keysym,bool down);
	void on_text(const char *text);
	void on_mousemove(int x,int y);
	void on_mousewheel(int y);
	void on_mousebutton(int button,bool down);
	void on_resize(int width,int height);

	void on_event(const SDL_Event &ev);
};

main_window::main_window(SDL_Window *wnd,SDL_GLContext glctx) :
	m_wnd(wnd),
	m_glctx(glctx)
{
}

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

void main_window::on_event(const SDL_Event &ev)
{
	switch (ev.type) {
	case SDL_KEYDOWN:
		on_key(ev.key.keysym,true);
		break;
	case SDL_KEYUP:
		on_key(ev.key.keysym,false);
		break;
	case SDL_TEXTINPUT:
		on_text(ev.text.text);
		break;
	case SDL_MOUSEMOTION:
		on_mousemove(ev.motion.x,ev.motion.y);
		break;
	case SDL_MOUSEWHEEL:
		on_mousewheel(ev.wheel.y);
		break;
	case SDL_MOUSEBUTTONDOWN:
		on_mousebutton(ev.button.button,true);
		break;
	case SDL_MOUSEBUTTONUP:
		on_mousebutton(ev.button.button,false);
		break;
	case SDL_WINDOWEVENT:
		switch (ev.window.event) {
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			on_resize(ev.window.data1,ev.window.data2);
			break;
		}
		break;
	}
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
	SDL_Window *wnd = SDL_CreateWindow(
			"DRNSF",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			DISPLAYWIDTH,
			DISPLAYHEIGHT,
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
		);
	if (!wnd) {
		std::cerr <<
			"Error creating the main window: " <<
			SDL_GetError() <<
			std::endl;
		SDL_Quit();
		return EXIT_FAILURE;
	}

	// Create an OpenGL context.
	SDL_GLContext glctx = SDL_GL_CreateContext(wnd);
	if (!glctx) {
		std::cerr <<
			"Error creating the OpenGL context: " <<
			SDL_GetError() <<
			std::endl;
		SDL_Quit();
		return EXIT_FAILURE;
	}

	// Enable v-sync.
	if (SDL_GL_SetSwapInterval(1) != 0) {
		std::cerr <<
			"Error enabling v-sync: " <<
			SDL_GetError() <<
			std::endl;
	}

	// Create the main window.
	main_window mw(wnd,glctx); // FIXME rename to wnd

	// Inform the editor about the initial window size.
	mw.get_editor().window_resize(DISPLAYWIDTH,DISPLAYHEIGHT);

	// Run the main application/game loop.
	while (true) {
		// Handle all of the pending events.
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			mw.on_event(ev);
		}

		// Calculate the time passed since the last frame.
		static Uint32 last_update = 0;
		Uint32 current_update = SDL_GetTicks();
		Uint32 delta_time = current_update - last_update;
		last_update = current_update;

		// Update the engine.
		mw.get_editor().frame(delta_time);

		// Present the finished render output to the user's display.
		SDL_GL_SwapWindow(wnd);
	}

	// Clean-up.
	SDL_Quit();

	return EXIT_SUCCESS;
}
