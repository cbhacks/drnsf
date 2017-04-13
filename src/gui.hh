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
};

class window_impl;

class im_window : private util::nocopy {
private:
	window_impl *M;
	window m_wnd;
	GtkWidget *m_canvas;
	int m_canvas_width;
	int m_canvas_height;

	gboolean on_render(
		GtkGLArea *area,
		GdkGLContext *context);

	void on_resize(
		GtkGLArea *area,
		gint width,
		gint height);

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
