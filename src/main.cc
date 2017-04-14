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
#include <GL/gl.h>
#include <iostream>
#include "edit.hh"

int main(int argc,char *argv[])
{
	using namespace drnsf;

	// Initialize GTK+.
	gtk_init(&argc,&argv);

	// Create the editor.
	edit::core editor;

	// Add the legacy ui redraw thing.
	g_idle_add(
		[](gpointer user_data) -> gboolean {
			gui::im_window::run_once();
			return true;
		},
		nullptr
	);

	// Run the main application/game loop.
	gtk_main();

	return EXIT_SUCCESS;
}
