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

#include <list>
#include <set>
#include "../imgui/imgui.h"
#include "res.hh"
#include "transact.hh"
#include "gui.hh"

namespace drnsf {
namespace edit {

class im_canvas : private util::nocopy {
private:
	gui::gl_canvas m_canvas;
	ImGuiContext *m_im;
	ImGuiIO *m_io;
	guint m_timer;
	long m_last_update;
	int m_canvas_width;
	int m_canvas_height;
	gui::gl_canvas::program m_gl_program;
	gui::gl_canvas::shader m_gl_vert_shader;
	gui::gl_canvas::shader m_gl_frag_shader;
	gl::texture m_gl_tex_font;
	gui::gl_canvas::uniform m_gl_uni_screenortho;
	gui::gl_canvas::uniform m_gl_uni_font;
	gui::gl_canvas::attrib m_gl_a_position;
	gui::gl_canvas::attrib m_gl_a_texcoord;
	gui::gl_canvas::attrib m_gl_a_color;
	gl::buffer m_gl_vb;
	gl::buffer m_gl_ib;
	gl::vert_array m_gl_va;
	decltype(m_canvas.on_init)::watch h_init;
	decltype(m_canvas.on_render)::watch h_render;
	decltype(m_canvas.on_resize)::watch h_resize;
	decltype(m_canvas.on_mousemove)::watch h_mousemove;
	decltype(m_canvas.on_mousewheel)::watch h_mousewheel;
	decltype(m_canvas.on_mousebutton)::watch h_mousebutton;
	decltype(m_canvas.on_key)::watch h_key;
	decltype(m_canvas.on_text)::watch h_text;

	void render();

public:
	explicit im_canvas(gui::container &parent);
	~im_canvas();

	int get_width() const;
	int get_height() const;

	void show();

	util::event<int> on_frame;
};

using res::project;

class classic_view : private util::nocopy {
private:
	gui::splitview m_split;
	gui::label m_tree;
	gui::label m_detail;

public:
	explicit classic_view(gui::container &parent);

	void show();
};

class core; // FIXME

class editor;

class window : private util::nocopy {
	friend class core; // FIXME

	friend class pane;

private:
	std::shared_ptr<editor> m_ed;
	gui::window m_wnd;
	gui::tabview m_tabs;
	gui::tabview::page m_classic_tab;
	gui::tabview::page m_canvas_tab;
	gui::tabview::page m_cryptos_tab;
	classic_view m_classic;
	im_canvas m_canvas;
	im_canvas m_cryptos;
	decltype(m_canvas.on_frame)::watch h_frame;

public:
	window();

	void frame(int delta_time);
};

class pane;
class mode;

class editor : private util::nocopy {
	friend class window;
	friend class pane;

private:
	const std::shared_ptr<project> m_proj;
	std::list<pane *> m_panes;
	std::unique_ptr<mode> m_mode;

public:
	explicit editor(std::shared_ptr<project> proj);

	project &get_project() const;
};

class pane : private util::nocopy {
private:
	std::string m_id;
	decltype(editor::m_panes)::iterator m_iter;

protected:
	editor &m_ed;

public:
	explicit pane(editor &ed,std::string id);
	~pane();

	virtual void show() = 0;

	const std::string &get_id() const;
	virtual std::string get_title() const = 0;
};

class mode : private util::nocopy {
protected:
	editor &m_ed;

	explicit mode(editor &ed) :
		m_ed(ed) {}

public:
	virtual ~mode() = default;

	virtual void update(double delta_time) {}
	virtual void render() {}
	virtual void show_gui() {}
};

class modedef : private util::nocopy {
private:
	std::string m_title;

protected:
	explicit modedef(std::string title);
	~modedef();

public:
	static const std::set<modedef*> &get_list();

	const std::string &get_title() const
	{
		return m_title;
	}

	virtual std::unique_ptr<mode> create(editor &ed) const = 0;
};

template <typename T>
class modedef_of : private modedef {
public:
	std::unique_ptr<mode> create(editor &ed) const override
	{
		return std::unique_ptr<mode>(new T(ed));
	}

	explicit modedef_of(std::string title) :
		modedef(title) {}
};

struct cam {
	float pitch = 30;
	float lens_near = 1.8;
	float lens_far = 200;
	float lens_adjust = 3.6;
	float lens_focus = 1.0;
};

class core : private util::nocopy {
public:
	std::shared_ptr<project> m_proj = std::make_shared<project>();
	std::list<std::function<void(int)>> m_modules;
	res::anyref m_selected_asset;
	cam m_cam;
	edit::window m_wnd;
	decltype(m_wnd.m_cryptos.on_frame)::watch h_frame;

	core();

	void frame(int delta);
};

}
}
