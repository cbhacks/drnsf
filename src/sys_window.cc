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
#include <iostream>
#include "sys.hh"

namespace sys {

void window::on_event(const SDL_Event &ev)
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
		on_windowevent(ev.window);
		break;
	}
}

void window::on_windowevent(const SDL_WindowEvent &ev)
{
	switch (ev.event) {
	case SDL_WINDOWEVENT_SIZE_CHANGED:
		on_resize(ev.data1,ev.data2);
		break;
	}
}

window::window(const std::string &title,int width,int height)
{
	// Create the window.
	m_wnd = SDL_CreateWindow(
		title.c_str(),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width,
		height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);
	if (!m_wnd) {
		std::cerr <<
			"Error creating window: " <<
			SDL_GetError() <<
			std::endl;
		throw 0; // FIXME
	}

	// Create the OpenGL context.
	m_glctx = SDL_GL_CreateContext(m_wnd);
	if (!m_glctx) {
		std::cerr <<
			"Error creating OpenGL context: " <<
			SDL_GetError() <<
			std::endl;
		SDL_DestroyWindow(m_wnd);
		throw 0; // FIXME
	}

	// Enable v-sync.
	if (SDL_GL_SetSwapInterval(1) != 0) {
		std::cerr <<
			"Error enabling v-sync: " <<
			SDL_GetError() <<
			std::endl;
	}
}

window::~window()
{
	// Clean up the OpenGL context.
	SDL_GL_DeleteContext(m_glctx);

	// Close and remove the window.
	SDL_DestroyWindow(m_wnd);
}

void window::run_once()
{
	// Handle all of the pending events.
	SDL_Event ev;
	while (SDL_PollEvent(&ev)) {
		on_event(ev);
	}

	// Calculate the time passed since the last frame.
	static Uint32 last_update = 0;
	Uint32 current_update = SDL_GetTicks();
	Uint32 delta_time = current_update - last_update;
	last_update = current_update;

	// Update the window.
	SDL_GL_MakeCurrent(m_wnd,m_glctx);
	on_frame(delta_time);
	SDL_GL_SwapWindow(m_wnd);
}

}
