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
 * gui.hh
 *
 * FIXME explain
 */

#include <gtk/gtk.h>
#include "../imgui/imgui.h"
#include "gl.hh"

namespace drnsf {
namespace gui {

/*
 * gui::sys_handle
 *
 * This is an alias for whatever type is appropriate for representing a system
 * handle representing a GUI widget.
 */
using sys_handle = GtkWidget *;

/*
 * gui::widget
 *
 * Common base class for all widget types.
 */
class widget : public util::nocopy {
protected:
    // (var) m_handle
    // The internal system handle for this widget. The base widget class will
    // release this handle or destroy the associated system object.
    const sys_handle m_handle;

    // (explicit ctor)
    // FIXME explain
    //
    // This constructor takes ownership of the handle, and will free it when
    // the object is destroyed or if the constructor throws an exception.
    explicit widget(sys_handle handle);

public:
    // (dtor)
    // Destroys the widget (including the internal system handle), removing it
    // from its parent. A visible widget will vanish if destroyed.
    ~widget();

    // (func) show
    // Shows the widget, if it is hidden. By default, every widget is
    // hidden when first constructed.
    void show();

    // (func) hide
    // Hides the widget, if it is visible. By default, every widget is already
    // hidden when constructed.
    void hide();
};

/*
 * gui::container
 *
 * FIXME explain
 */
class container : private util::nocopy {
public:
    // (pure func) get_container_handle
    // FIXME explain
    virtual GtkContainer *get_container_handle() = 0;
};

/*
 * gui::window
 *
 * FIXME explain
 */
class window : public container {
    friend class menu;

private:
    // (var) M
    // FIXME explain
    GtkWidget *M;

    // (var) m_vbox
    // FIXME explain
    GtkWidget *m_vbox;

    // (var) m_menubar
    // FIXME explain
    GtkWidget *m_menubar;

    // (var) m_content
    // FIXME explain
    GtkWidget *m_content;

public:
    // (explicit ctor)
    // FIXME explain
    explicit window(const std::string &title,int width,int height);

    // (dtor)
    // FIXME explain
    ~window();

    // (func) show
    // FIXME explain
    void show();

    // (func) get_container_handle
    // FIXME explain
    GtkContainer *get_container_handle() override;
};

/*
 * gui::composite
 *
 * FIXME explain
 */
class composite : public widget, public container {
public:
    // (explicit ctor)
    // FIXME explain
    explicit composite(container &parent);

    // (func) get_container_handle
    // FIXME explain
    GtkContainer *get_container_handle() override;
};

/*
 * gui::widget_gl
 *
 * FIXME explain
 */
class widget_gl : private widget {
private:
    // (s-func) sigh_draw
    // Internal signal handler for GtkWidget::'draw'.
    static gboolean sigh_draw(
        GtkWidget *widget,
        cairo_t *cr,
        gpointer user_data);

    // (s-func) sigh_motion_notify_event
    // Internal signal handler for GtkWidget::'motion-notify-event'.
    static gboolean sigh_motion_notify_event(
        GtkWidget *widget,
        GdkEvent *event,
        gpointer user_data);

    // (s-func) sigh_scroll_event
    // Internal signal handler for GtkWidget::'scroll-event'.
    static gboolean sigh_scroll_event(
        GtkWidget *widget,
        GdkEvent *event,
        gpointer user_data);

    // (s-func) sigh_button_event
    // Internal signal handler for GtkWidget::'button-event'.
    static gboolean sigh_button_event(
        GtkWidget *widget,
        GdkEvent *event,
        gpointer user_data);

    // (s-func) sigh_key_event
    // Internal signal handler for GtkWidget::'key-event'.
    static gboolean sigh_key_event(
        GtkWidget *widget,
        GdkEvent *event,
        gpointer user_data);

protected:
    // (pure func) draw_gl
    // Draws the contents of this widget to the specified renderbuffer. Derived
    // classes must implement this method. This function will not be called on
    // every frame, only when the widget is resized or after invalidate is
    // called (see below).
    //
    // All global GL state will be default when this function is called. When
    // overriding it, you must ensure you return in such a state as well, or
    // this guarantee will not be met for widgets drawn afterwards.
    virtual void draw_gl(int width,int height,gl::renderbuffer &rbo) = 0;

    // (func) invalidate
    // Marks the current rendered output of this widget as out-of-date or stale
    // so that draw_gl will be called again. If the widget is not currently
    // visible to the user (due to being hidden, obscured, off-screen, etc)
    // then the call to draw_gl may be deferred until this is not the case, or
    // it may never be called if the widget is destroyed before such a case
    // occurs.
    void invalidate();

    // (func) mousemove
    // Called when the mouse moves into or while inside of the widget.
    //
    // The default implementation performs no operation.
    virtual void mousemove(int x,int y) {}

    // (func) mousewheel
    // Called when the mouse wheel is scrolled vertically. Whether this happens
    // to the focused widget or to the widget currently under the mouse could
    // vary by operating system.
    //
    // The default implementation performs no operation.
    virtual void mousewheel(int delta_y) {}

    // (func) mousebutton
    // Called when a mouse button is pressed or released while this widget has
    // focus. The button press could potentially be one which causes the widget
    // to gain focus.
    //
    // The default implementation performs no operation.
    virtual void mousebutton(int number,bool down) {}

    // (func) key
    // Called when a key is pressed or released while this widget has focus.
    //
    // The default implementation performs no operation.
    virtual void key(int code,bool down) {}

    // (func) text
    // Called when text is entered into the widget. This is different from key
    // input, as multiple keys in series could input one character, or another
    // sequence of different characters. For more information, read about input
    // methods such as IME and methods used for inputting characters often used
    // in southeast asian languages.
    //
    // The default implementation performs no operation.
    virtual void text(const char *str) {}

public:
    // (explicit ctor)
    // FIXME explain
    explicit widget_gl(container &parent);

    using widget::show;
    using widget::hide;
};

/*
 * gui::label
 *
 * FIXME explain
 */
class label : public widget {
public:
    // (explicit ctor)
    // FIXME explain
    explicit label(container &parent,const std::string &text = "");

    // (func) set_text
    // Sets the text displayed by the label.
    void set_text(const std::string &text);
};

/*
 * gui::tabview
 *
 * FIXME explain
 */
class tabview : public widget {
public:
    // inner class defined later in this file
    class page;

    // (explicit ctor)
    // FIXME explain
    explicit tabview(container &parent);
};

/*
 * gui::tabview::page
 *
 * FIXME explain
 */
class tabview::page : public container {
private:
    // (var) M
    // FIXME explain
    GtkWidget *M;

    // (var) m_view
    // FIXME explain
    tabview &m_view;

    // (var) m_pagenum
    // FIXME explain
    int m_pagenum;

public:
    // (explicit ctor)
    // FIXME explain
    explicit page(tabview &view,const std::string &title);

    // (dtor)
    // FIXME explain
    ~page();

    // (func) get_container_handle
    // FIXME explain
    GtkContainer *get_container_handle() override;
};

/*
 * gui::splitview
 *
 * FIXME explain
 */
class splitview : public widget {
private:
    // (inner class) side
    // FIXME explain
    struct side : public container {
        // (var) M
        // FIXME explain
        GtkWidget *M;

        // (func) get_container_handle
        // FIXME explain
        GtkContainer *get_container_handle() override;
    };

    // (var) m_left, m_right
    // FIXME explain
    side m_left;
    side m_right;

public:
    // (explicit ctor)
    // FIXME explain
    explicit splitview(container &parent);

    // (func) get_left
    // FIXME explain
    container &get_left();

    // (func) get_right
    // FIXME explain
    container &get_right();
};

/*
 * gui::gridview
 *
 * FIXME explain
 */
class gridview : public widget {
private:
    // (inner class) slot
    // Cheap proxy for gui::container. FIXME ??
    struct slot : public container {
        GtkWidget *m_box;

        slot() = default;

        GtkContainer *get_container_handle() override
        {
            return GTK_CONTAINER(m_box);
        }
    };

    // (var) m_slots
    // FIXME explain
    std::list<slot> m_slots;

public:
    // (explicit ctor)
    // Creates the gridview with the specified number of rows and columns,
    // and adds it to the parent widget. If `absolute' is true, the grid
    // cells each have equal size and the grid is stretched to fit the
    // available space; otherwise, the grid is sized to fit its contents.
    explicit gridview(container &parent,int cols,int rows,bool absolute);

    // (func) make_slot
    // Creates a slot at the specified column and row (zero-based) with
    // a width and height of (col_count,row_count) in columns and rows,
    // and returns a reference to this slot as a container.
    container &make_slot(
        int col,
        int row,
        int col_count = 1,
        int row_count = 1);
};

/*
 * gui::treeview
 *
 * FIXME explain
 */
class treeview : public widget {
public:
    // inner class defined later in this file
    class node;

private:
    // (var) m_tree
    // FIXME explain
    GtkWidget *m_tree;

    // (var) m_store
    // FIXME explain
    GtkTreeStore *m_store;

    // (var) m_selected_node
    // FIXME explain
    node *m_selected_node = nullptr;

    // (s-func) sigh_changed
    // FIXME explain
    static void sigh_changed(
        GtkTreeSelection *treeselection,
        gpointer user_data);

public:
    // (explicit ctor)
    // FIXME explain
    explicit treeview(container &parent);
};

/*
 * gui::treeview::node
 *
 * FIXME explain
 */
class treeview::node : private util::nocopy {
private:
    // (var) m_view
    // FIXME explain
    treeview &m_view;

    // (var) m_store
    // FIXME explain
    GtkTreeStore *m_store;

    // (var) m_iter
    // FIXME explain
    GtkTreeIter m_iter;

public:
    // (explicit ctor)
    // FIXME explain
    explicit node(treeview &parent);

    // (explicit ctor)
    // FIXME explain
    explicit node(node &parent);

    // (dtor)
    // FIXME explain
    ~node();

    // (func) set_text
    // FIXME explain
    void set_text(const std::string &text);

    // (event) on_select
    // FIXME explain
    util::event<> on_select;

    // (event) on_deselect
    // FIXME explain
    util::event<> on_deselect;
};

/*
 * gui::gl_canvas
 *
 * FIXME explain
 */
class gl_canvas : public widget, public gl::old::machine {
private:
    // (s-func) sigh_draw
    // FIXME explain
    static gboolean sigh_draw(
        GtkWidget *widget,
        cairo_t *cr,
        gpointer user_data);

    // (s-func) sigh_motion_notify_event
    // FIXME explain
    static gboolean sigh_motion_notify_event(
        GtkWidget *widget,
        GdkEvent *event,
        gpointer user_data);

    // (s-func) sigh_scroll_event
    // FIXME explain
    static gboolean sigh_scroll_event(
        GtkWidget *widget,
        GdkEvent *event,
        gpointer user_data);

    // (s-func) sigh_button_event
    // FIXME explain
    static gboolean sigh_button_event(
        GtkWidget *widget,
        GdkEvent *event,
        gpointer user_data);

    // (s-func) sigh_key_event
    // FIXME explain
    static gboolean sigh_key_event(
        GtkWidget *widget,
        GdkEvent *event,
        gpointer user_data);

public:
    // (explicit ctor)
    // FIXME explain
    explicit gl_canvas(container &parent);

    // (func) invalidate
    // FIXME explain
    void invalidate();

    // (event) on_render
    // FIXME explain
    util::event<int,int> on_render;

    // (event) on_mousemove
    // FIXME explain
    util::event<int,int> on_mousemove;

    // (event) on_mousewheel
    // FIXME explain
    util::event<int> on_mousewheel;

    // (event) on_mousebutton
    // FIXME explain
    util::event<int,bool> on_mousebutton;

    // (event) on_key
    // FIXME explain
    util::event<int,bool> on_key;

    // (event) on_text
    // FIXME explain
    util::event<const char *> on_text;
};

/*
 * gui::menu
 *
 * FIXME explain
 */
class menu : private util::nocopy {
    friend class menu_item;

private:
    // (var) m_item
    // FIXME explain
    GtkWidget *m_item;

    // (var) m_menu
    // FIXME explain
    GtkWidget *m_menu;

public:
    // (explicit ctor)
    // FIXME explain
    explicit menu(window &parent,const std::string &text);

    // (explicit ctor)
    // FIXME explain
    explicit menu(menu &parent,const std::string &text);

    // (dtor)
    // FIXME explain
    ~menu();
};

/*
 * gui::menu_item
 *
 * FIXME explain
 */
class menu_item : private util::nocopy {
private:
    // (var) M
    // FIXME explain
    GtkWidget *M;

    // (s-func) sigh_activate
    // FIXME explain
    static void sigh_activate(GtkMenuItem *menuitem,gpointer user_data);

public:
    // (explicit ctor)
    // FIXME explain
    explicit menu_item(menu &parent,const std::string &text);

    // (dtor)
    // FIXME explain
    ~menu_item();

    // (event) on_click
    // FIXME explain
    util::event<> on_click;
};

// FIXME obsolete
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
