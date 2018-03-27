//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2018  DRNSF contributors
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
 * edit::context
 *
 * FIXME explain
 */
class context : private util::nocopy {
private:
    // (var) m_proj
    // The project currently open under this context, or null if there is no
    // project currently open.
    std::shared_ptr<res::project> m_proj;

public:
    // (explicit ctor)
    // Creates a context with the specified project open. The context takes a
    // copy of the shared pointer.
    explicit context(std::shared_ptr<res::project> proj);

    // (func) get_proj, set_proj
    // Gets or sets the project associated with this context.
    //
    // When setting the project, a copy of the shared pointer to the previous
    // project is kept while delivering the on_project_change event. This ensures
    // that, if this context held the final shared_ptr, the project is kept alive
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
    context &m_ctx;
    void on_activate() final override;

public:
    explicit mni_open(gui::menu &menu, context &ctx) :
        item(menu, "Open"),
        m_ctx(ctx) {}
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
    context &m_ctx;
    mni_open m_open{*this, m_ctx};
    mni_exit m_exit{*this};

public:
    explicit mnu_file(gui::menubar &menubar, context &ctx) :
        item(menubar, "File"),
        m_ctx(ctx) {}
};

/*
 * edit::menus::mni_undo
 *
 * Edit -> Undo
 * Undoes the last operation.
 */
class mni_undo : private gui::menu::item {
private:
    context &m_ctx;
    void update();
    void on_activate() final override;

    decltype(transact::nexus::on_status_change)::watch h_status_change;

public:
    explicit mni_undo(gui::menu &menu, context &ctx);
};

/*
 * edit::menus::mni_redo
 *
 * Edit -> Undo
 * Redoes the last undone operation.
 */
class mni_redo : private gui::menu::item {
private:
    context &m_ctx;
    void update();
    void on_activate() final override;

    decltype(transact::nexus::on_status_change)::watch h_status_change;

public:
    explicit mni_redo(gui::menu &menu, context &ctx);
};

/*
 * edit::menus::mnu_edit
 *
 * "Edit" menu.
 */
class mnu_edit : private gui::menubar::item {
private:
    context &m_ctx;
    mni_undo m_undo{*this, m_ctx};
    mni_redo m_redo{*this, m_ctx};

public:
    explicit mnu_edit(gui::menubar &menubar, context &ctx) :
        item(menubar, "Edit"),
        m_ctx(ctx) {}
};

}

/*
 * edit::asset_metactl
 *
 * This widget displays basic information about an asset, such as its name and
 * type, and provides inputs for some basic operations which can be applied to
 * it, such as deletion, renaming, etc.
 *
 * The asset is selected by name, and the control watches the name's associated
 * project to update itself when the named asset appears or disappears from the
 * project.
 */
class asset_metactl : private gui::widget_im {
private:
    // (var) m_name
    // FIXME explain
    res::atom m_name;

    // (handler) h_asset_appear, h_asset_disappear
    // Hooks the project's on_asset_appear and on_asset_disappear events so
    // that, if the selected asset comes into or out of existence, the widget
    // can be updated.
    decltype(res::project::on_asset_appear)::watch h_asset_appear;
    decltype(res::project::on_asset_disappear)::watch h_asset_disappear;

    // (func) frame
    // Implements `gui::widget_im::frame' to enact the widget's contents using
    // ImGui.
    void frame() override;

public:
    // (explicit ctor)
    // Constructs the metactl widget in the given container with the given
    // layout. By default, it is not set to use any asset name.
    explicit asset_metactl(gui::container &parent, gui::layout layout);

    // (func) set_name
    // FIXME explain
    void set_name(res::atom name);

    using widget_im::show;
    using widget_im::hide;
    using widget_im::get_layout;
    using widget_im::set_layout;
    using widget_im::get_real_size;
    using widget_im::get_screen_pos;
};

/*
 * edit::asset_viewctl
 *
 * This widget displays basic information about an asset, such as its name and
 * type, and provides inputs for some basic operations which can be applied to
 * it, such as deletion, renaming, etc.
 *
 * The asset is selected by name, and the control watches the name's associated
 * project to update itself when the named asset appears or disappears from the
 * project.
 */
class asset_viewctl : private gui::composite {
private:
    // (var) m_name
    // FIXME explain
    res::atom m_name;

    // (var) m_inner
    // A pointer to a specialized internal widget which handles the specific
    // type of the view's asset.
    std::unique_ptr<util::polymorphic> m_inner;

    // (handler) h_asset_appear, h_asset_disappear
    // Hooks the project's on_asset_appear and on_asset_disappear events so
    // that, if the selected asset comes into or out of existence, the widget
    // can be updated.
    decltype(res::project::on_asset_appear)::watch h_asset_appear;
    decltype(res::project::on_asset_disappear)::watch h_asset_disappear;

public:
    // (explicit ctor)
    // Constructs the viewctl widget in the given container with the given
    // layout. By default, it is not set to use any asset name.
    explicit asset_viewctl(gui::container &parent, gui::layout layout);

    // (func) set_name
    // FIXME explain
    void set_name(res::atom name);

    using composite::show;
    using composite::hide;
    using composite::get_layout;
    using composite::set_layout;
    using composite::get_real_size;
    using composite::get_screen_pos;
};

/*
 * edit::asset_propctl
 *
 * This widget displays the properties of an asset and allows the user to
 * modify their values.
 */
class asset_propctl : private gui::widget_im {
private:
    // (var) m_name
    // FIXME explain
    res::atom m_name;

    // (handler) h_asset_appear, h_asset_disappear
    // Hooks the project's on_asset_appear and on_asset_disappear events so
    // that, if the selected asset comes into or out of existence, the widget
    // can be updated.
    decltype(res::project::on_asset_appear)::watch h_asset_appear;
    decltype(res::project::on_asset_disappear)::watch h_asset_disappear;

    // (func) frame
    // Implements `gui::widget_im::frame' to enact the widget's contents using
    // ImGui.
    void frame() override;

public:
    // (explicit ctor)
    // Constructs the propctl widget in the given container with the given
    // layout. By default, it is not set to use any asset name.
    explicit asset_propctl(gui::container &parent, gui::layout layout);

    // (func) set_name
    // FIXME explain
    void set_name(res::atom name);

    using widget_im::show;
    using widget_im::hide;
    using widget_im::get_layout;
    using widget_im::set_layout;
    using widget_im::get_real_size;
    using widget_im::get_screen_pos;
};

/*
 * edit::asset_mainctl
 *
 * This widget displays an asset to the user and provides the ability to change
 * or manipulate the asset in some way
 *
 * This is a composite of several different asset widgets defined above.
 */
class asset_mainctl : private gui::composite {
private:
    // (var) m_metactl
    // An instance of the asset_metactl placed roughly in the top-left quarter
    // of the widget.
    asset_metactl m_metactl{*this, gui::layout::grid(0, 3, 8, 0, 1, 2)};

    // (var) m_viewctl
    // An instance of the asset_viewctl placed roughly in the top-right quarter
    // of the widget.
    asset_viewctl m_viewctl{*this, gui::layout::grid(3, 5, 8, 0, 1, 2)};

    // (var) m_propctl
    // An instance of the asset_propctl placed roughly in the bottom half of
    // the widget.
    asset_propctl m_propctl{*this, gui::layout::grid(0, 1, 1, 1, 1, 2)};

public:
    // (explicit ctor)
    // Constructs the widget in the given container with the given layout. The
    // initial asset name is null, so the widget will merely show generic "no
    // asset selected" messages.
    explicit asset_mainctl(gui::container &parent, gui::layout layout) :
        composite(parent, layout)
    {
        m_metactl.show();
        m_viewctl.show();
        m_propctl.show();
    }

    // (func) set_name
    // Sets the asset name used by all of the contained widgets.
    void set_name(res::atom name)
    {
        m_metactl.set_name(name);
        m_viewctl.set_name(name);
        m_propctl.set_name(name);
    }

    using composite::show;
    using composite::hide;
    using composite::get_layout;
    using composite::set_layout;
    using composite::get_real_size;
    using composite::get_screen_pos;
};

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
    context &m_ctx;
    gui::window m_wnd;
    gui::menubar m_newmenubar;
    menus::mnu_file m_mnu_file{m_newmenubar, m_ctx};
    menus::mnu_edit m_mnu_edit{m_newmenubar, m_ctx};
    res::project *m_proj_p;
    std::unique_ptr<asset_editor> m_assets_view;

public:
    explicit main_window(context &ctx);

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
