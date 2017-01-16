//
// WILLYMOD - An unofficial Crash Bandicoot level editor
// Copyright (C) 2016  WILLYMOD project contributors
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
			"WILLYMOD",
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

	// Create and initialize the editor.
	edit::core editor;

	// Inform the editor about the initial window size.
	editor.window_resize(DISPLAYWIDTH,DISPLAYHEIGHT);

	// Run the main application/game loop.
	while (true) {
		// Handle all of the pending events.
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
			case SDL_KEYDOWN:
				editor.key(ev.key.keysym.sym,true);
				break;
			case SDL_KEYUP:
				editor.key(ev.key.keysym.sym,false);
				break;
			case SDL_TEXTINPUT:
				editor.text(ev.text.text);
				break;
			case SDL_MOUSEMOTION:
				editor.mouse_move(
					ev.motion.x,
					ev.motion.y
				);
				editor.mouse_move_rel(
					ev.motion.xrel,
					ev.motion.yrel
				);
				break;
			case SDL_MOUSEWHEEL:
				editor.mouse_scroll(ev.wheel.y);
				break;
			case SDL_MOUSEBUTTONDOWN:
				editor.mouse_button(ev.button.button,true);
				break;
			case SDL_MOUSEBUTTONUP:
				editor.mouse_button(ev.button.button,false);
				break;
			case SDL_WINDOWEVENT:
				switch (ev.window.event) {
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					editor.window_resize(
						ev.window.data1,
						ev.window.data2
					);
					break;
				}
				break;
			}
		}

		// Calculate the time passed since the last frame.
		static Uint32 last_update = 0;
		Uint32 current_update = SDL_GetTicks();
		Uint32 delta_time = current_update - last_update;
		last_update = current_update;

		// Update the engine.
		editor.frame(delta_time);

		// Present the finished render output to the user's display.
		SDL_GL_SwapWindow(wnd);
	}

	// Clean-up.
	SDL_Quit();

	return EXIT_SUCCESS;
}
