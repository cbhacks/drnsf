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

#include <gtk/gtk.h>
#include "../imgui/imgui.h"

namespace drnsf {
namespace gui {

class window : private util::nocopy {
public:
	GtkWidget *M;

	explicit window(const std::string &title,int width,int height);
	~window();

	void show();
};

class gl_canvas : private util::nocopy {
private:
	bool m_is_init = false;

	static gboolean sigh_render(
		GtkGLArea *area,
		GdkGLContext *context,
		gpointer user_data);

	static void sigh_resize(
		GtkGLArea *area,
		int width,
		int height,
		gpointer user_data);

	static void sigh_motion_notify_event(
		GtkWidget *widget,
		GdkEvent *event,
		gpointer user_data);

	static void sigh_scroll_event(
		GtkWidget *widget,
		GdkEvent *event,
		gpointer user_data);

	static void sigh_button_press_event(
		GtkWidget *widget,
		GdkEvent *event,
		gpointer user_data);

	static void sigh_button_release_event(
		GtkWidget *widget,
		GdkEvent *event,
		gpointer user_data);

public:
	GtkWidget *M;

	explicit gl_canvas(window &parent);
	~gl_canvas();

	void show();

	util::event<> on_init;
	util::event<> on_cleanup;
	util::event<> on_render;
	util::event<int,int> on_resize;
	util::event<int,int> on_mousemove;
	util::event<int> on_mousewheel;
	util::event<int,bool> on_mousebutton;
};

class window_impl;

class im_window : private util::nocopy {
private:
	window_impl *M;
	window m_wnd;
	gl_canvas m_canvas;
	unsigned int m_canvas_font;
	int m_canvas_width;
	int m_canvas_height;
	decltype(m_canvas.on_init)::watch h_init;
	decltype(m_canvas.on_cleanup)::watch h_cleanup;
	decltype(m_canvas.on_render)::watch h_render;
	decltype(m_canvas.on_resize)::watch h_resize;
	decltype(m_canvas.on_mousemove)::watch h_mousemove;
	decltype(m_canvas.on_mousewheel)::watch h_mousewheel;
	decltype(m_canvas.on_mousebutton)::watch h_mousebutton;

	void render();

public:
	explicit im_window(const std::string &title,int width,int height);
	~im_window();

	int get_width() const;
	int get_height() const;

	void run_once();

	util::event<int> on_frame;
};

namespace im {

using namespace ImGui;

void label(
	const std::string &text);

void subwindow(
	const std::string &id,
	const std::string &title,
	const std::function<void()> &f);

void main_menu_bar(
	const std::function<void()> &f);

void menu(
	const std::string &text,
	const std::function<void()> &f = nullptr);

void menu_item(
	const std::string &text,
	const std::function<void()> &f = nullptr);

void menu_separator();

class scope : private util::nocopy {
public:
	explicit scope(int id);
	explicit scope(void *ptr);
	~scope();
};

}

}
}
