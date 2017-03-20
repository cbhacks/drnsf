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
#include "edit.hh"
#include "gui.hh"

namespace edit {

window::window() :
	gui::window("Dr. N. Essef",800,600)
{
}

void window::frame(int delta_time)
{
	namespace im = gui::im;

	// Clear the screen.
	glClear(GL_COLOR_BUFFER_BIT);

	im::main_menu_bar([&]{
		im::menu("File",[&]{
			im::menu_item("New Project",[&]{
				// Prompt the user for confirmation if there is
				// already an open project with unsaved changes.
				// TODO

				// Discard the current mode.
				m_mode = nullptr;

				// Create and target a new project
				m_proj = std::make_shared<project>();
			});
			im::menu_item("Open Project");
			im::menu_item("Close Project",m_proj ? [&]{
				// Prompt the user for confirmation if there are
				// unsaved changes.
				// TODO

				// Discard the current mode.
				m_mode = nullptr;

				// Abandon the current project.
				m_proj = nullptr;
			} : std::function<void()>(nullptr));
			im::menu_separator();
			im::menu_item("Save Project");
			im::menu_item("Save Project As");
			im::menu_separator();
			im::menu_item("Exit DRNSF",[&]{
				// Prompt the user for confirmation if there are
				// unsaved changes.
				// TODO

				// Close the program.
				// TODO - change this to close only the one window?
				SDL_Quit();
				std::exit(EXIT_SUCCESS);
			});
		});
		im::menu("Edit",m_proj ? [&]{
			auto &&transact = m_proj->get_transact();

			if (transact.has_undo()) {
				auto title = transact.get_undo().describe();

				im::menu_item("Undo: $"_fmt(title),[&]{
					transact.undo();
				});
			} else {
				im::menu_item("Undo");
			}

			if (transact.has_redo()) {
				auto title = transact.get_redo().describe();

				im::menu_item("Redo: $"_fmt(title),[&]{
					transact.redo();
				});
			} else {
				im::menu_item("Redo");
			}

			im::menu_separator();

			for (auto &&modedef : modedef::get_list()) {
				auto title = modedef->get_title();
				im::menu_item("Mode: $"_fmt(title),[&]{
					m_mode = modedef->create(*m_proj);
				});
			}
		} : std::function<void()>(nullptr));
	});

	if (m_mode) {
		m_mode->update(delta_time);
		m_mode->render();
		m_mode->show_gui();
	}
}

}
