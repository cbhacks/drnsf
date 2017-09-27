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

#include <unordered_set>
#include <gtk/gtk.h>
#include "../imgui/imgui.h"
#include "gl.hh"

namespace drnsf {
namespace gui {

/*
 * gui::sys_handle
 * gui::sys_container_handle
 *
 * These are aliases for whatever types are appropriate for representing a
 * widget handle and widget-container handle on the system, respectively.
 */
using sys_handle = GtkWidget *;
using sys_container_handle = GtkContainer *;

/*
 * gui::layout
 *
 * FIXME explain
 */
struct layout {
    /*
     * gui::layout::h_pin
     * gui::layout::v_pin
     *
     * These types define a horizontal or vertical position for one side of a
     * widget (left, right, top, or bottom) relative to its parent container. The
     * `factor' field defines the anchor point in the parent container; a value of
     * 0.0f corresponds to the top or left side of the container, while a value of
     * 1.0f corresponds to the bottom or right side of the container. The `offset'
     * field specifies an offset from that point, in pixels, for this location.
     * X values increase from left to right, and Y values increase from top to
     * bottom (NOT bottom to top).
     *
     * For example, some possible pin values:
     *
     * `h_pin( 0.0f, 10 )`  10 pixels from the left side of the container
     * `h_pin( 0.5f, 0 )`   the horizontal center of the container
     * `h_pin( 1.0f, -64 )` 64 pixels from the right side of the container
     * `v_pin( 1.0f, 0 )`   the bottom of the container
     */
    struct h_pin {
        float factor;
        int offset;
    };
    struct v_pin {
        float factor;
        int offset;
    };

    /*
     * gui::layout::h_bar
     * gui::layout::v_bar
     *
     * These types consist of h_pin or v_pin pairs, respectively. An h_bar defines
     * the left and right side positions of a widget relative to its container, and
     * a v_bar defines the top and bottom side positions.
     */
    struct h_bar {
        h_pin left;
        h_pin right;
    };
    struct v_bar {
        v_pin top;
        v_pin bottom;
    };

    h_bar h;
    v_bar v;

    // FIXME comment
    static constexpr layout fill()
    {
        return {
            { { 0.0f, 0 }, { 1.0f, 0 } },
            { { 0.0f, 0 }, { 1.0f, 0 } }
        };
    }

    static constexpr layout grid(
        int h_num_offset,
        int h_num_width,
        int h_denom,
        int v_num_offset,
        int v_num_width,
        int v_denom)
    {
        float h_denom_f = h_denom;
        float v_denom_f = v_denom;
        return {
            {
                { h_num_offset / h_denom_f, 0 },
                { (h_num_offset + h_num_width) / h_denom_f, 0 }
            },
            {
                { v_num_offset / v_denom_f, 0 },
                { (v_num_offset + v_num_width) / v_denom_f, 0 }
            }
        };
    }
};

// defined later in this file
class container;

/*
 * gui::widget
 *
 * Common base class for all widget types.
 */
class widget : public util::nocopy {
    friend class container;

private:
    // (func) apply_layout
    // Internal function for applying the widget layout to the actual widget
    // handle used by the system.
    void apply_layout(GtkAllocation &ctn_alloc);

protected:
    // (var) m_handle
    // The internal system handle for this widget. The base widget class will
    // release this handle or destroy the associated system object.
    const sys_handle m_handle;

    // (var) m_parent
    // A reference to the container this widget exists inside of. All widgets
    // must exist within a container.
    container &m_parent;

    // (var) m_layout
    // The layout area which defines the location and size of this widget
    // relative to its container.
    layout m_layout;

    // (explicit ctor)
    // Constructs the base widget data with the given handle, parent, and
    // layout. The widget will automatically be resized according to its parent
    // and specified layout.
    //
    // This constructor takes ownership of the given handle, and will free it
    // even if an exception is thrown in the constructor. The handle parameter
    // takes an rvalue reference to avoid accidentally passing a handle which
    // was not meant to be relinquished to `gui::widget'.
    explicit widget(sys_handle &&handle,container &parent,layout layout);

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
    friend class widget;

private:
    // (func) m_widgets
    // The set of widgets contained inside this container.
    std::unordered_set<widget *> m_widgets;

protected:
    // (func) apply_layouts
    // FIXME explain
    void apply_layouts(GtkAllocation &alloc);

public:
    // (pure func) get_container_handle
    // FIXME explain
    virtual sys_container_handle get_container_handle() = 0;
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
    sys_container_handle get_container_handle() override;
};

/*
 * gui::composite
 *
 * FIXME explain
 */
class composite : private widget, public container {
public:
    // (ctor)
    // FIXME explain
    composite(container &parent,layout layout);

    // (func) get_container_handle
    // FIXME explain
    sys_container_handle get_container_handle() override;

    using widget::show;
    using widget::hide;
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
    // (ctor)
    // FIXME explain
    widget_gl(container &parent,layout layout);

    using widget::show;
    using widget::hide;
};

/*
 * gui::widget_2d
 *
 * FIXME explain
 */
class widget_2d : private widget {
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
    // (pure func) draw_2d
    // Draws the contents of this widget. This function will not be called on
    // every frame, only when the widget is resized or after invalidate is
    // called (see below).
    virtual void draw_2d(int width,int height,cairo_t *cr) = 0;

    // (func) invalidate
    // Marks the current rendered output of this widget as out-of-date or stale
    // so that draw_2d will be called again. If the widget is not currently
    // visible to the user (due to being hidden, obscured, off-screen, etc)
    // then the call to draw_2d may be deferred until this is not the case, or
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
    // (ctor)
    // FIXME explain
    widget_2d(container &parent,layout layout);

    using widget::show;
    using widget::hide;
};

/*
 * gui::widget_im
 *
 * FIXME explain
 */
class widget_im : private widget_gl {
private:
    // (var) m_im
    // The ImGui context for this widget. Each `widget_im' instance has its own
    // context so that they do not interfere with eachother.
    ImGuiContext *m_im;

    // (var) m_io
    // A pointer to a structure for the ImGui context which is used to provide
    // input into ImGui, such as mouse position or keystroke inputs.
    ImGuiIO *m_io;

    // (var) m_timer
    // A handle to a timer used to repeatedly invalidate the underlying GL
    // widget so that ImGui can run every frame.
    guint m_timer;

    // (var) m_last_update
    // The time of the last update. This information is used each `draw_gl'
    // call to determine the amount of time passed since the previous update,
    // which is needed by ImGui.
    long m_last_update;

    // (func) draw_gl
    // FIXME explain
    void draw_gl(int width,int height,gl::renderbuffer &rbo) final override;

    // (func) mousemove
    // FIXME explain
    void mousemove(int x,int y) final override;

    // (func) mousewheel
    // FIXME explain
    void mousewheel(int delta_y) final override;

    // (func) mousebutton
    // FIXME explain
    void mousebutton(int number,bool down) final override;

    // (func) key
    // FIXME explain
    void key(int code,bool down) final override;

    // (func) text
    // FIXME explain
    void text(const char *str) final override;

protected:
    // (pure func) frame
    // FIXME explain
    virtual void frame() = 0;

public:
    // (ctor)
    // FIXME explain
    widget_im(container &parent,layout layout);

    // (dtor)
    // FIXME explain
    ~widget_im();

    using widget_gl::show;
    using widget_gl::hide;
};

/*
 * gui::label
 *
 * FIXME explain
 */
class label : private widget_2d {
private:
    // (var) m_text
    // The text displayed by the label.
    std::string m_text;

    // (func) draw_2d
    // Implements draw_2d to render the label text.
    void draw_2d(int width,int height,cairo_t *cr) override;

public:
    // (ctor)
    // FIXME explain
    label(container &parent,layout layout,const std::string &text = "");

    // (func) set_text
    // Sets the text displayed by the label.
    void set_text(const std::string &text);

    using widget_2d::show;
    using widget_2d::hide;
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
    explicit treeview(container &parent,layout layout);
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
class gl_canvas : public widget {
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
    explicit gl_canvas(container &parent,layout layout);

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
