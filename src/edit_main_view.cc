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
#include <gtk/gtk.h>
#include "edit.hh"
#include "gui.hh"

namespace drnsf {
namespace edit {

main_view::main_view(old_editor &ed) :
	m_ed(ed),
	m_canvas_wnd("Old UI",800,600),
	m_cryptos_wnd("Ancient UI",800,600),
	m_canvas(m_canvas_wnd),
	m_cryptos(m_cryptos_wnd)
{
	h_frame <<= [this](int width,int height,int delta_time) {
		frame(delta_time);
	};
	h_frame.bind(m_canvas.on_frame);

	m_canvas.show();
	m_cryptos.show();
}

void main_view::frame(int delta_time)
{
	namespace im = gui::im;

	// Clear the screen.
	glClear(GL_COLOR_BUFFER_BIT);

	im::main_menu_bar([&]{
		im::menu("Edit",[&]{
			auto &&transact = m_ed.m_proj.get_transact();

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
					m_ed.m_mode = modedef->create(m_ed);
				});
			}
		});
	});

	if (true) {
		if (m_ed.m_mode) {
			auto &&mode = m_ed.m_mode;
			mode->update(delta_time);
			mode->render();
			mode->show_gui();
		}

		for (auto &&pane : m_ed.m_panes) {
			im::subwindow(pane->get_id(),pane->get_title(),[&]{
				pane->show();
			});
		}
	}
}

void main_view::show()
{
	m_canvas_wnd.show();
	m_cryptos_wnd.show();
}

}
}
