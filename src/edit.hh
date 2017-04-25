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

class editor : private util::nocopy {
private:
	res::project &m_proj;

public:
	explicit editor(res::project &proj);
};

class main_window : private util::nocopy {
private:
	gui::window m_wnd;
	res::project *m_proj_p;
	std::unique_ptr<editor> m_ed_p;

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
	int m_canvas_width;
	int m_canvas_height;
	gl::program m_gl_program;
	gl::shader m_gl_vert_shader;
	gl::shader m_gl_frag_shader;
	gl::texture m_gl_tex_font;
	gl::buffer m_gl_vb;
	gl::buffer m_gl_ib;
	gl::vert_array m_gl_va;
	gl::uniform m_gl_u_screenortho;
	gl::uniform m_gl_u_font;
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
	class asset_node : private util::nocopy {
	private:
		gui::treeview::node m_tree_node;

	public:
		asset_node(classic_view &view,res::asset &asset) :
			m_tree_node(view.m_tree)
		{
			m_tree_node.set_text(asset.get_name().full_path());
		}
	};

	gui::splitview m_split;
	gui::treeview m_tree;
	gui::label m_detail;
	project &m_proj;
	std::map<res::asset *,std::unique_ptr<asset_node>> m_asset_nodes;

	decltype(project::on_asset_appear)::watch h_asset_appear;
	decltype(project::on_asset_disappear)::watch h_asset_disappear;

public:
	explicit classic_view(gui::container &parent,project &proj);

	void show();
};

class visual_view : private util::nocopy {
private:
	gui::gl_canvas m_canvas;
	project &m_proj;

public:
	explicit visual_view(gui::container &parent,project &proj);

	void show();

	decltype(project::on_asset_appear)::watch h_asset_appear;
	decltype(project::on_asset_disappear)::watch h_asset_disappear;
};

class core; // FIXME

class old_editor;

class main_view : private util::nocopy {
	friend class core; // FIXME

	friend class pane;

private:
	old_editor &m_ed;
	gui::tabview m_tabs;
	gui::tabview::page m_classic_tab;
	gui::tabview::page m_visual_tab;
	gui::tabview::page m_canvas_tab;
	gui::tabview::page m_cryptos_tab;
	classic_view m_classic;
	visual_view m_visual;
	im_canvas m_canvas;
	im_canvas m_cryptos;
	decltype(m_canvas.on_frame)::watch h_frame;

public:
	explicit main_view(gui::container &parent,old_editor &ed);

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
	res::anyref m_selected_asset;
	cam m_cam;
	gui::window m_real_wnd;
	edit::main_view m_wnd;
	decltype(m_wnd.m_cryptos.on_frame)::watch h_frame;

	core();

	void frame(int delta);
};

}
}
