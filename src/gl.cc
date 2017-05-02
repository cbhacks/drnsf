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
#include "gl.hh"

namespace drnsf {
namespace gl {

/*
 * gl::g_wnd, gl::g_glctx
 *
 * FIXME explain
 */
GdkWindow *g_wnd;
GdkGLContext *g_glctx;

// declared in gl.hh
void init()
{
	// Set attributes for the hidden background window we are about to
	// create (see below).
	GdkWindowAttr wnd_attr;
	wnd_attr.event_mask = 0;
	wnd_attr.width = 100;
	wnd_attr.height = 100;
	wnd_attr.wclass = GDK_INPUT_OUTPUT;
	wnd_attr.window_type = GDK_WINDOW_TOPLEVEL;

	// Create the hidden window. This window is necessary to actually get
	// an OpenGL context, even if we don't render to it.
	g_wnd = gdk_window_new(nullptr,&wnd_attr,0);
	// Failure presumably results in a crash or premature exit/abort. This
	// is the typical behavior in GTK/GDK/Glib, and nothing can be done
	// about it. Blame their devs.

	// Create an OpenGL context for the hidden window. This context will
	// be used for all rendering in the application. Normal render output
	// will be discarded because this window will never appear, so render
	// jobs should target an FBO+RBO or FBO+texture which can then be
	// blitted to the actual appropriate display widget.
	GError *error = nullptr;
	g_glctx = gdk_window_create_gl_context(g_wnd,&error);
	if (!g_glctx) {
		// TODO
	}

	// Set the new context as the "current" one.
	gdk_gl_context_make_current(g_glctx);
}

}
}
