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
 * edit::editor
 *
 * FIXME explain
 */
class editor : private util::nocopy {
private:
    // (var) m_proj
    // The project currently open under this editor, or null if there is no
    // project currently open.
    std::shared_ptr<res::project> m_proj;

public:
    // (explicit ctor)
    // Creates an editor with the specified project open. The editor takes a
    // copy of the shared pointer.
    explicit editor(std::shared_ptr<res::project> proj);

    // (func) get_proj, set_proj
    // Gets or sets the project associated with this editor.
    //
    // When setting the project, a copy of the shared pointer to the previous
    // editor is kept while delivering the on_project_change event. This ensures
    // that, if the editor held the final shared_ptr, the project is kept alive
    // until the event handlers have all been run.
    //
    // It is important that set_proj never be called from inside a transaction,
    // as the project running the transaction could cease to exist during this
    // call if there are no other shared_ptr's to the project remaining.
    const std::shared_ptr<res::project> &get_proj() const;
    void set_proj(std::shared_ptr<res::project> proj);

    // (event) on_project_change
    // Raised whenever the project is changed by `set_proj'. The previous
    // project, if any, will be kept alive during the execution of this event's
    // handlers, but may be released once the event has finished.
    util::event<const std::shared_ptr<res::project> &> on_project_change;
};

namespace menus {

/*
 * edit::menus::mni_open
 *
 * File -> Open
 * Opens a file. (FIXME what kind?)
 */
class mni_open : private gui::menu::item {
private:
    editor &m_ed;
    void on_activate() final override;

public:
    explicit mni_open(gui::menu &menu, editor &ed) :
        item(menu, "Open"),
        m_ed(ed) {}
};

/*
 * edit::menus::mni_exit
 *
 * File -> Exit
 * Exits the application.
 */
class mni_exit : private gui::menu::item {
private:
    void on_activate() final override;

public:
    explicit mni_exit(gui::menu &menu) :
        item(menu, "Exit") {}
};

/*
 * edit::menus::mnu_file
 *
 * "File" menu.
 */
class mnu_file : private gui::menubar::item {
private:
    editor &m_ed;
    mni_open m_open{*this, m_ed};
    mni_exit m_exit{*this};

public:
    explicit mnu_file(gui::menubar &menubar, editor &ed) :
        item(menubar, "File"),
        m_ed(ed) {}
};

/*
 * edit::menus::mni_undo
 *
 * Edit -> Undo
 * Undoes the last operation.
 */
class mni_undo : private gui::menu::item {
private:
    editor &m_ed;
    void update();
    void on_activate() final override;

    decltype(transact::nexus::on_status_change)::watch h_status_change;

public:
    explicit mni_undo(gui::menu &menu, editor &ed);
};

/*
 * edit::menus::mni_redo
 *
 * Edit -> Undo
 * Redoes the last undone operation.
 */
class mni_redo : private gui::menu::item {
private:
    editor &m_ed;
    void update();
    void on_activate() final override;

    decltype(transact::nexus::on_status_change)::watch h_status_change;

public:
    explicit mni_redo(gui::menu &menu, editor &ed);
};

/*
 * edit::menus::mnu_edit
 *
 * "Edit" menu.
 */
class mnu_edit : private gui::menubar::item {
private:
    editor &m_ed;
    mni_undo m_undo{*this, m_ed};
    mni_redo m_redo{*this, m_ed};

public:
    explicit mnu_edit(gui::menubar &menubar, editor &ed) :
        item(menubar, "Edit"),
        m_ed(ed) {}
};

}

/*
 * edit::asset_editor
 *
 * FIXME explain
 */
class asset_editor : private gui::composite {
private:
    // inner class defined in edit_asset_editor.cc
    class impl;

    // (var) M
    // The pointer to the internal implementation object (PIMPL).
    impl *M;

public:
    // (ctor)
    // Constructs the widget and places it in the given parent container.
    asset_editor(gui::container &parent, gui::layout layout, res::project &proj);

    // (dtor)
    // Destroys the widget, removing it from the parent container.
    ~asset_editor();

    using composite::show;
    using composite::hide;
    using composite::get_layout;
    using composite::set_layout;
    using composite::get_real_size;
    using composite::get_screen_pos;
};

/*
 * edit::asset_tree
 *
 * FIXME explain
 */
class asset_tree : private gui::composite {
private:
    // inner class defined in edit_asset_tree.cc
    class impl;

    // (var) M
    // The pointer to the internal implementation object (PIMPL).
    impl *M;

public:
    // (ctor)
    // Constructs the widget and places it in the given parent container.
    asset_tree(gui::container &parent, gui::layout layout, res::project &proj);

    // (dtor)
    // Destroys the widget, removing it from the parent container.
    ~asset_tree();

    using composite::show;
    using composite::hide;
    using composite::get_layout;
    using composite::set_layout;
    using composite::get_real_size;
    using composite::get_screen_pos;

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
class asset_infoview : private gui::composite {
private:
    // inner class defined in edit_asset_infoview.cc
    class impl;

    // (var) M
    // The pointer to the internal implementation object (PIMPL).
    impl *M;

public:
    // (ctor)
    // Constructs the widget and places it in the given parent container.
    asset_infoview(gui::container &parent, gui::layout layout, res::project &proj);

    // (dtor)
    // Destroys the widget, removing it from the parent container.
    ~asset_infoview();

    using composite::show;
    using composite::hide;
    using composite::get_layout;
    using composite::set_layout;
    using composite::get_real_size;
    using composite::get_screen_pos;

    // (func) set_selected_asset
    // Sets the asset name which this widget should show information for.
    void set_selected_asset(res::atom atom);
};

/*
 * edit::asset_propview
 *
 * FIXME explain
 */
class asset_propview : private gui::composite {
private:
    // inner class defined in edit_asset_propview.cc
    class impl;

    // (var) M
    // The pointer to the internal implementation object (PIMPL).
    impl *M;

public:
    // (ctor)
    // Constructs the widget and places it in the given parent container.
    asset_propview(gui::container &parent, gui::layout layout, res::project &proj);

    // (dtor)
    // Destroys the widget, removing it from the parent container.
    ~asset_propview();

    using composite::show;
    using composite::hide;
    using composite::get_layout;
    using composite::set_layout;
    using composite::get_real_size;
    using composite::get_screen_pos;
};

/*
 * edit::asset_viewport
 *
 * FIXME explain
 */
class asset_viewport : private gui::composite {
private:
    // inner class defined in edit_asset_viewport.cc
    class impl;

    // (var) M
    // The pointer to the internal implementation object (PIMPL).
    impl *M;

public:
    // (ctor)
    // Constructs the widget and places it in the given parent container.
    asset_viewport(gui::container &parent, gui::layout layout, res::project &proj);

    // (dtor)
    // Destroys the widget, removing it from the parent container.
    ~asset_viewport();

    using composite::show;
    using composite::hide;
    using composite::get_layout;
    using composite::set_layout;
    using composite::get_real_size;
    using composite::get_screen_pos;
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
    editor &m_ed;
    gui::window m_wnd;
    gui::menubar m_newmenubar;
    menus::mnu_file m_mnu_file{m_newmenubar, m_ed};
    menus::mnu_edit m_mnu_edit{m_newmenubar, m_ed};
    res::project *m_proj_p;
    std::unique_ptr<asset_editor> m_assets_view;

public:
    explicit main_window(editor &ed);

    void show();

    void set_project(res::project &proj);
};

//// SEEN BELOW: soon-to-be-obsolete code ////

class im_canvas : public gui::widget_im {
private:
    void frame() final override;

public:
    im_canvas(gui::container &parent, gui::layout layout) :
        widget_im(parent, layout) {}

    using widget_im::show;

    util::event<int, int, int> on_frame;
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
    gui::gl_canvas m_cryptos;
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
    explicit pane(old_editor &ed, std::string id);
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
    project &m_proj;
    old_editor m_ed;
    std::list<std::function<void()>> m_modules;
    edit::main_view m_wnd;
    decltype(m_wnd.m_cryptos.on_render)::watch h_render;

    core(project &proj);

    void frame(int width, int height);
};

}
}
