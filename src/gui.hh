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

class container : private util::nocopy {
public:
	virtual GtkContainer *get_container_handle() = 0;
};

class window : public container {
private:
	GtkWidget *M;

public:
	explicit window(const std::string &title,int width,int height);
	~window();

	void show();

	GtkContainer *get_container_handle() override;
};

class label : private util::nocopy {
private:
	GtkWidget *M;

public:
	explicit label(container &parent,const std::string &text);
	~label();

	void show();
};

class tabview : private util::nocopy {
private:
	GtkWidget *M;

public:
	class page;

	explicit tabview(container &parent);
	~tabview();

	void show();
};

class tabview::page : public container {
private:
	GtkWidget *M;
	tabview &m_view;
	int m_pagenum;

public:
	explicit page(tabview &view,const std::string &title);
	~page();

	GtkContainer *get_container_handle() override;
};

class splitview : private util::nocopy {
private:
	GtkWidget *M;

	struct side : public container {
		GtkWidget *M;

		GtkContainer *get_container_handle() override;
	} m_left,m_right;

public:
	explicit splitview(container &parent);
	~splitview();

	void show();

	container &get_left();
	container &get_right();
};

class gl_canvas : private util::nocopy {
private:
	GtkWidget *M;

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

	static gboolean sigh_motion_notify_event(
		GtkWidget *widget,
		GdkEvent *event,
		gpointer user_data);

	static gboolean sigh_scroll_event(
		GtkWidget *widget,
		GdkEvent *event,
		gpointer user_data);

	static gboolean sigh_button_event(
		GtkWidget *widget,
		GdkEvent *event,
		gpointer user_data);

	static gboolean sigh_key_event(
		GtkWidget *widget,
		GdkEvent *event,
		gpointer user_data);

public:
	class program;
	class shader;
	class texture;
	class buffer;
	class vert_array;
	class uniform;
	class attrib;

	explicit gl_canvas(container &parent);
	~gl_canvas();

	void show();

	void invalidate();

	util::event<> on_init;
	util::event<> on_cleanup;
	util::event<> on_render;
	util::event<int,int> on_resize;
	util::event<int,int> on_mousemove;
	util::event<int> on_mousewheel;
	util::event<int,bool> on_mousebutton;
	util::event<int,bool> on_key;
	util::event<const char *> on_text;
};

class gl_canvas::program : private util::nocopy {
private:
	unsigned int m_id;

	decltype(on_init)::watch h_init;
	decltype(on_cleanup)::watch h_cleanup;

public:
	explicit program(gl_canvas &canvas);

	void attach(shader &sh);

	void link();

	operator unsigned int();

	util::event<> on_link;
};

class gl_canvas::shader : private util::nocopy {
	friend class program;

private:
	unsigned int m_id;

	decltype(on_init)::watch h_init;
	decltype(on_cleanup)::watch h_cleanup;

public:
	explicit shader(gl_canvas &canvas,int type);

	void compile(const char *code);

	operator unsigned int();
};

class gl_canvas::texture : private util::nocopy {
private:
	unsigned int m_id;

	decltype(on_init)::watch h_init;
	decltype(on_cleanup)::watch h_cleanup;

public:
	explicit texture(gl_canvas &canvas);

	operator unsigned int();
};

class gl_canvas::buffer : private util::nocopy {
private:
	unsigned int m_id;

	decltype(on_init)::watch h_init;
	decltype(on_cleanup)::watch h_cleanup;

public:
	explicit buffer(gl_canvas &canvas);

	operator unsigned int();
};

class gl_canvas::vert_array : private util::nocopy {
private:
	unsigned int m_id;

	decltype(on_init)::watch h_init;
	decltype(on_cleanup)::watch h_cleanup;

public:
	explicit vert_array(gl_canvas &canvas);

	operator unsigned int();
};

class gl_canvas::uniform : private util::nocopy {
private:
	int m_id;

	decltype(program::on_link)::watch h_link;

public:
	explicit uniform(program &prog,std::string name);

	operator int();
};

class gl_canvas::attrib : private util::nocopy {
private:
	int m_id;

	decltype(program::on_link)::watch h_link;

public:
	explicit attrib(program &prog,std::string name);

	operator int();
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
