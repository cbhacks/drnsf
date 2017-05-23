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

/*
 * edit.hh
 *
 * FIXME explain
 */

#include <list>
#include <set>
#include "../imgui/imgui.h"
#include "res.hh"
#include "transact.hh"
#include "gui.hh"
#include "render.hh"

namespace drnsf {
namespace edit {

/*
 * edit::asset_editor
 *
 * FIXME explain
 */
class asset_editor : public gui::widget {
private:
    // inner class defined in edit_asset_editor.cc
    class impl;

    // (var) M
    // The pointer to the internal implementation object (PIMPL).
    impl *M;

public:
    // (explicit ctor)
    // Constructs the widget and places it in the given parent container.
    explicit asset_editor(gui::container &parent,res::project &proj);

    // (dtor)
    // Destroys the widget, removing it from the parent container.
    ~asset_editor();

    // (func) get_handle
    // See gui::widget::get_handle.
    GtkWidget *get_handle() override;
};

/*
 * edit::asset_tree
 *
 * FIXME explain
 */
class asset_tree : public gui::widget {
private:
    // inner class defined in edit_asset_tree.cc
    class impl;

    // (var) M
    // The pointer to the internal implementation object (PIMPL).
    impl *M;

public:
    // (explicit ctor)
    // Constructs the widget and places it in the given parent container.
    explicit asset_tree(gui::container &parent,res::project &proj);

    // (dtor)
    // Destroys the widget, removing it from the parent container.
    ~asset_tree();

    // (func) get_handle
    // See gui::widget::get_handle.
    GtkWidget *get_handle() override;

    // (event) on_select
    // Raised when the selected asset has changed. Note that this can be
    // triggered automatically, for example if the currently selected asset
    // vanishes and the tree automatically selects another list item.
    util::event<res::atom> on_select;
};

/*
 * edit::asset_infoview
 *
 * FIXME explain
 */
class asset_infoview : public gui::widget {
private:
    // inner class defined in edit_asset_infoview.cc
    class impl;

    // (var) M
    // The pointer to the internal implementation object (PIMPL).
    impl *M;

public:
    // (explicit ctor)
    // Constructs the widget and places it in the given parent container.
    explicit asset_infoview(gui::container &parent,res::project &proj);

    // (dtor)
    // Destroys the widget, removing it from the parent container.
    ~asset_infoview();

    // (func) get_handle
    // See gui::widget::get_handle.
    GtkWidget *get_handle() override;

    // (func) set_selected_asset
    // Sets the asset name which this widget should show information for.
    void set_selected_asset(res::atom atom);
};

/*
 * edit::asset_propview
 *
 * FIXME explain
 */
class asset_propview : public gui::widget {
private:
    // inner class defined in edit_asset_propview.cc
    class impl;

    // (var) M
    // The pointer to the internal implementation object (PIMPL).
    impl *M;

public:
    // (explicit ctor)
    // Constructs the widget and places it in the given parent container.
    explicit asset_propview(gui::container &parent,res::project &proj);

    // (dtor)
    // Destroys the widget, removing it from the parent container.
    ~asset_propview();

    // (func) get_handle
    // See gui::widget::get_handle.
    GtkWidget *get_handle() override;
};

/*
 * edit::asset_viewport
 *
 * FIXME explain
 */
class asset_viewport : public gui::widget {
private:
    // inner class defined in edit_asset_viewport.cc
    class impl;

    // (var) M
    // The pointer to the internal implementation object (PIMPL).
    impl *M;

public:
    // (explicit ctor)
    // Constructs the widget and places it in the given parent container.
    explicit asset_viewport(gui::container &parent,res::project &proj);

    // (dtor)
    // Destroys the widget, removing it from the parent container.
    ~asset_viewport();

    // (func) get_handle
    // See gui::widget::get_handle.
    GtkWidget *get_handle() override;
};

// FIXME - temporary global for compatibility
extern res::atom g_selected_asset;
extern render::camera g_camera;
static float &g_camera_yaw = g_camera.yaw;
static float &g_camera_pitch = g_camera.pitch;
static float &g_camera_zoom = g_camera.zoom;

// FIXME FIXME FIXME FIXME FIXME
// slightly newer but still obsolete code below

class main_window : private util::nocopy {
private:
    gui::window m_wnd;
    gui::menu m_menu_file;
    gui::menu_item m_menu_file_exit;
    gui::tabview m_tabs;
    gui::tabview::page m_assets_tab;
    res::project *m_proj_p;
    std::unique_ptr<asset_editor> m_assets_view;

    decltype(gui::menu_item::on_click)::watch h_menu_file_exit_click;

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

class core : private util::nocopy {
public:
    project m_proj;
    old_editor m_ed;
    std::list<std::function<void(int)>> m_modules;
    edit::main_view m_wnd;
    decltype(m_wnd.m_cryptos.on_frame)::watch h_frame;

    core();

    void frame(int width,int height,int delta);
};

}
}
