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

// FIXME - temporary global for compatibility
extern res::atom g_selected_asset;

class editor : private util::nocopy {
	friend class map_view;
	friend class assets_view;

private:
	res::project &m_proj;

public:
	explicit editor(res::project &proj);
};

class map_view : private util::nocopy {
private:
	editor &m_ed;
	gui::gl_canvas m_canvas;

	decltype(m_canvas.on_render)::watch h_render;

public:
	explicit map_view(gui::container &parent,editor &ed);

	void show();
};

class assets_view : private util::nocopy {
private:
	class impl;

	impl *M;

public:
	explicit assets_view(gui::container &parent,editor &ed);
	~assets_view();

	void show();
};

class detail_view : private util::nocopy {
private:
	class impl;

	impl *M;

public:
	explicit detail_view(gui::container &parent,editor &ed);
	~detail_view();

	void show();
};

class main_window : private util::nocopy {
private:
	gui::window m_wnd;
	gui::splitview m_split;
	gui::tabview m_tabs;
	gui::tabview::page m_assets_tab;
	gui::tabview::page m_detail_tab;
	res::project *m_proj_p;
	std::unique_ptr<editor> m_ed_p;
	std::unique_ptr<map_view> m_map_view;
	std::unique_ptr<assets_view> m_assets_view;
	std::unique_ptr<detail_view> m_detail_view;

public:
	main_window();

	void show();

	void set_project(res::project &proj);
};

//// SEEN BELOW: soon-to-be-obsolete code ////

class im_canvas : private util::nocopy {
private:
	gui::gl_canvas m_canvas;
	ImGuiContext *m_im;
	ImGuiIO *m_io;
	guint m_timer;
	long m_last_update;
	gl::old::program m_gl_program;
	gl::old::shader m_gl_vert_shader;
	gl::old::shader m_gl_frag_shader;
	gl::old::texture m_gl_tex_font;
	gl::old::buffer m_gl_vb;
	gl::old::buffer m_gl_ib;
	gl::old::vert_array m_gl_va;
	gl::old::uniform m_gl_u_screenortho;
	gl::old::uniform m_gl_u_font;
	decltype(m_canvas.on_render)::watch h_render;
	decltype(m_canvas.on_mousemove)::watch h_mousemove;
	decltype(m_canvas.on_mousewheel)::watch h_mousewheel;
	decltype(m_canvas.on_mousebutton)::watch h_mousebutton;
	decltype(m_canvas.on_key)::watch h_key;
	decltype(m_canvas.on_text)::watch h_text;

	void render(int width,int height);

public:
	explicit im_canvas(gui::container &parent);
	~im_canvas();

	void show();

	util::event<int,int,int> on_frame;
};

using res::project;

class core; // FIXME

class old_editor;

class main_view : private util::nocopy {
	friend class core; // FIXME

	friend class pane;

private:
	old_editor &m_ed;
	gui::window m_canvas_wnd;
	gui::window m_cryptos_wnd;
	im_canvas m_canvas;
	im_canvas m_cryptos;
	decltype(m_canvas.on_frame)::watch h_frame;

public:
	explicit main_view(old_editor &ed);

	void frame(int delta_time);

	void show();
};

class pane;
class mode;

class old_editor : private util::nocopy {
	friend class main_view;
	friend class pane;

private:
	project &m_proj;
	std::list<pane *> m_panes;
	std::unique_ptr<mode> m_mode;

public:
	explicit old_editor(project &proj);

	project &get_project() const;
};

class pane : private util::nocopy {
private:
	std::string m_id;
	decltype(old_editor::m_panes)::iterator m_iter;

protected:
	old_editor &m_ed;

public:
	explicit pane(old_editor &ed,std::string id);
	~pane();

	virtual void show() = 0;

	const std::string &get_id() const;
	virtual std::string get_title() const = 0;
};

class mode : private util::nocopy {
protected:
	old_editor &m_ed;

	explicit mode(old_editor &ed) :
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

	virtual std::unique_ptr<mode> create(old_editor &ed) const = 0;
};

template <typename T>
class modedef_of : private modedef {
public:
	std::unique_ptr<mode> create(old_editor &ed) const override
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
	project m_proj;
	old_editor m_ed;
	std::list<std::function<void(int)>> m_modules;
	cam m_cam;
	edit::main_view m_wnd;
	decltype(m_wnd.m_cryptos.on_frame)::watch h_frame;

	core();

	void frame(int width,int height,int delta);
};

}
}
