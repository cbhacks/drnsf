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
#include <iostream>
#include <epoxy/gl.h>
#include "gui.hh"

namespace drnsf {
namespace gui {

gboolean gl_canvas::sigh_render(
	GtkGLArea *area,
	GdkGLContext *context,
	gpointer user_data)
{
	// Restore the depth test setting to the OpenGL default (OFF). GTK+
	// enables this at the start of every frame if it was built with a
	// depth buffer requested.
	glDisable(GL_DEPTH_TEST);

	auto self = static_cast<gl_canvas *>(user_data);

	self->run_jobs();
	self->on_render();
	return true;
}

void gl_canvas::sigh_resize(
	GtkGLArea *area,
	int width,
	int height,
	gpointer user_data)
{
	static_cast<gl_canvas *>(user_data)->on_resize(width,height);
}

gboolean gl_canvas::sigh_motion_notify_event(
	GtkWidget *widget,
	GdkEvent *event,
	gpointer user_data)
{
	static_cast<gl_canvas *>(user_data)->on_mousemove(
		event->motion.x,
		event->motion.y
	);
	return true;
}

gboolean gl_canvas::sigh_scroll_event(
	GtkWidget *widget,
	GdkEvent *event,
	gpointer user_data)
{
	if (event->scroll.direction != GDK_SCROLL_SMOOTH) {
		return false;
	}

	static_cast<gl_canvas *>(user_data)->on_mousewheel(
		-event->scroll.delta_y
	);
	return true;
}

gboolean gl_canvas::sigh_button_event(
	GtkWidget *widget,
	GdkEvent *event,
	gpointer user_data)
{
	static_cast<gl_canvas *>(user_data)->on_mousebutton(
		event->button.button,
		event->button.type == GDK_BUTTON_PRESS
	);
	return true;
}

gboolean gl_canvas::sigh_key_event(
	GtkWidget *widget,
	GdkEvent *event,
	gpointer user_data)
{
	auto self = static_cast<gl_canvas *>(user_data);
	self->on_key(
		event->key.keyval,
		event->key.type == GDK_KEY_PRESS
	);
	if (event->key.type == GDK_KEY_PRESS) {
		// FIXME deprecated garbage
		self->on_text(event->key.string);
	}
	return true;
}

gl_canvas::gl_canvas(container &parent)
{
	M = gtk_gl_area_new();
	gtk_gl_area_set_has_depth_buffer(GTK_GL_AREA(M),true);
	gtk_widget_set_events(
		M,
		GDK_POINTER_MOTION_MASK |
		GDK_SMOOTH_SCROLL_MASK |
		GDK_BUTTON_PRESS_MASK |
		GDK_BUTTON_RELEASE_MASK |
		GDK_KEY_PRESS_MASK |
		GDK_KEY_RELEASE_MASK
	);
	gtk_widget_set_can_focus(M,true);
	g_signal_connect(M,"render",G_CALLBACK(sigh_render),this);
	g_signal_connect(M,"resize",G_CALLBACK(sigh_resize),this);
	g_signal_connect(
		M,
		"motion-notify-event",
		G_CALLBACK(sigh_motion_notify_event),
		this
	);
	g_signal_connect(
		M,
		"scroll-event",
		G_CALLBACK(sigh_scroll_event),
		this
	);
	g_signal_connect(
		M,
		"button-press-event",
		G_CALLBACK(sigh_button_event),
		this
	);
	g_signal_connect(
		M,
		"button-release-event",
		G_CALLBACK(sigh_button_event),
		this
	);
	g_signal_connect(
		M,
		"key-press-event",
		G_CALLBACK(sigh_key_event),
		this
	);
	g_signal_connect(
		M,
		"key-release-event",
		G_CALLBACK(sigh_key_event),
		this
	);
	gtk_container_add(parent.get_container_handle(),M);
}

gl_canvas::~gl_canvas()
{
	gtk_gl_area_make_current(GTK_GL_AREA(M));
	run_jobs();
	gdk_gl_context_clear_current(); //necessary to avoid crash on
	// the next invocation of this destructor!!!
	gtk_widget_destroy(M);
}

void gl_canvas::show()
{
	gtk_widget_show(M);
}

void gl_canvas::invalidate()
{
	gtk_gl_area_queue_render(GTK_GL_AREA(M));
}

}
}
