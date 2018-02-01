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
#include <unordered_map>
#include <cairo.h>
#include "../imgui/imgui.h"
#include "gl.hh"

#if USE_GTK3
#include <gtk/gtk.h>
#endif

namespace drnsf {
namespace gui {

/*
 * gui::init
 *
 * This function should be called before any gui::* objects are created. The
 * `argc' and `argv' parameters to `main' should be given by-reference, as
 * some GUI targets require this.
 */
void init(int &argc, char **&argv);

/*
 * gui::run
 *
 * Runs the main window event loop for the GUI used.
 */
void run();

/*
 * gui::sys_handle
 *
 * This type is used in place of a particular frontend's object handle types,
 * such as Window (XLIB), GtkWidget (GTK3), HWND (WINAPI), etc.
 */
using sys_handle = void *;

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
class window;

/*
 * gui::widget
 *
 * Common base class for all widget types.
 */
class widget : public util::nocopy {
    friend class container;
    friend class window;
    friend void run();

private:
    // (s-var) s_all_widgets
    // The set of all existing widget objects, whether visible or not. This is
    // a map from each widget's internal handle (m_handle, defined below) to a
    // pointer to the respective widget object.
    static std::unordered_map<sys_handle, widget *> s_all_widgets;

    // (s-func) sigh_motion_notify_event
    // Internal signal handler for GtkWidget::'motion-notify-event'.
    static gboolean sigh_motion_notify_event(
        GtkWidget *wdg,
        GdkEvent *event,
        gpointer user_data);

    // (s-func) sigh_scroll_event
    // Internal signal handler for GtkWidget::'scroll-event'.
    static gboolean sigh_scroll_event(
        GtkWidget *wdg,
        GdkEvent *event,
        gpointer user_data);

    // (s-func) sigh_button_event
    // Internal signal handler for GtkWidget::'button-event'.
    static gboolean sigh_button_event(
        GtkWidget *wdg,
        GdkEvent *event,
        gpointer user_data);

    // (s-func) sigh_key_event
    // Internal signal handler for GtkWidget::'key-event'.
    static gboolean sigh_key_event(
        GtkWidget *wdg,
        GdkEvent *event,
        gpointer user_data);

    // (s-func) sigh_size_allocate
    // Internal signal handler for GtkWidget::'size-allocate'.
    static void sigh_size_allocate(
        GtkWidget *wdg,
        GdkRectangle *allocation,
        gpointer user_data);

    // (var) m_real_width, m_real_height
    // The actual height of the widget, in pixels.
    int m_real_width;
    int m_real_height;

    // (func) apply_layout
    // Applies the widget's layout to its size and position based on the area
    // of the container given as parameters.
    void apply_layout(int ctn_x, int ctn_y, int ctn_w, int ctn_h);

    // (func) mousemove
    // Called when the mouse moves into or while inside of the widget.
    //
    // The default implementation performs no operation.
    virtual void mousemove(int x, int y) {}

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
    virtual void mousebutton(int number, bool down) {}

    // (func) key
    // Called when a key is pressed or released while this widget has focus.
    //
    // The default implementation performs no operation.
    virtual void key(int code, bool down) {}

    // (func) text
    // Called when text is entered into the widget. This is different from key
    // input, as multiple keys in series could input one character, or another
    // sequence of different characters. For more information, read about input
    // methods such as IME and methods used for inputting characters often used
    // in southeast asian languages.
    //
    // The default implementation performs no operation.
    virtual void text(const char *str) {}

    // (func) on_resize
    // Called when the widget is resized. This may be caused by the parent
    // container being resized, causing the child widgets to re-apply their
    // defined layouts.
    //
    // The default implementation performs no operation.
    virtual void on_resize(int width, int height) {}

    // (func) update
    // FIXME explain
    virtual int update(int delta_ms)
    {
        return INT_MAX;
    }

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
    layout m_layout{};

    // (explicit ctor)
    // Constructs the base widget data with the given handle and parent.
    //
    // The derived class should call set_layout in its constructor.
    //
    // This constructor takes ownership of the given handle, and will free it
    // even if an exception is thrown in the constructor. The handle parameter
    // takes an rvalue reference to avoid accidentally passing a handle which
    // was not meant to be relinquished to `gui::widget'.
    explicit widget(sys_handle &&handle, container &parent);

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

    // (func) get_layout, set_layout
    // Gets or sets the widget's layout. The layout determines how the widget
    // is positioned and sized relative to its parent widget. When the layout
    // is changed, the widget's location will be reconfigured according to the
    // new layout.
    //
    // See the definition for the gui::layout type for more details.
    const layout &get_layout() const;
    void set_layout(layout layout);

    // (func) get_real_size
    // Gets the real size of the widget in pixels, as determined by its layout
    // and its parent container's real size.
    void get_real_size(int &width, int &height);
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
    void apply_layouts(int x, int y, int w, int h);

public:
    // (pure func) get_container_handle
    // FIXME explain
    virtual sys_handle get_container_handle() = 0;

    // (pure func) get_container_size
    // Gets the size of the container, in pixels.
    virtual void get_container_size(int &ctn_w, int &ctn_h) = 0;
};

/*
 * gui::composite
 *
 * FIXME explain
 */
class composite : private widget, public container {
private:
    // (func) on_resize
    // Implements on_resize to apply the change in container size to the child
    // widgets.
    void on_resize(int width, int height) final override;

public:
    // (ctor)
    // FIXME explain
    composite(container &parent, layout layout);

    // (func) get_container_handle
    // FIXME explain
    sys_handle get_container_handle() override;

    // (func) get_container_size
    // Implements gui::container::get_container_size.
    void get_container_size(int &ctn_w, int &ctn_h) override;

    using widget::show;
    using widget::hide;
};

// defined later in this file
class menubar;

/*
 * gui::window
 *
 * FIXME explain
 */
class window : public container {
    friend class menubar;

private:
    // (s-var) s_all_windows
    // The set of all existing window objects, whether visible or not. This is
    // a map from each window's internal handle (m_handle, defined below) to a
    // pointer to the respective object.
    static std::unordered_map<sys_handle, window *> s_all_windows;

    // (var) m_handle
    // Internal handle to the underlying system window.
    sys_handle m_handle;

    // (var) m_content
    // A handle to a GtkFixed object contained inside the actual window. This
    // is the true container for the window's widgets, as a GtkWindow cannot
    // contain more than one widget. gui::widget is designed for use within
    // GtkFixed containers only.
    GtkWidget *m_content;

    // (var) m_menubar
    // A pointer to the menubar associated with this window, or null if there
    // is none.
    gui::menubar *m_menubar = nullptr;

    // (var) m_width, m_height
    // The size of the popup window, in pixels.
    int m_width;
    int m_height;

protected:
    // (func) exit_dialog
    // FIXME explain
    void exit_dialog();

public:
    // (explicit ctor)
    // FIXME explain
    explicit window(const std::string &title, int width, int height);

    // (dtor)
    // FIXME explain
    ~window();

    // (func) show
    // FIXME explain
    void show();

    // (func) show_dialog
    // FIXME explain
    void show_dialog();

    // (func) get_container_handle
    // FIXME explain
    sys_handle get_container_handle() override;

    // (func) get_container_size
    // Implements gui::container::get_container_size.
    void get_container_size(int &ctn_w, int &ctn_h) override;
};

/*
 * gui::popup
 *
 * This class implements a "popup window". This is similar to a normal window,
 * however it cannot have a menubar associated with it, and usually does not
 * have any usual window stylings such as a title bar, close button, etc.
 *
 * This kind of window is useful for implementing dropdowns, tooltips, menus,
 * etc.
 */
class popup : public container {
private:
    // (s-var) s_all_popups
    // The set of all existing popup objects, whether visible or not. This is
    // a map from each popup's internal handle (m_handle, defined below) to a
    // pointer to the respective object.
    static std::unordered_map<sys_handle, popup *> s_all_popups;

    // (var) m_handle
    // A handle to the underlying system window.
    sys_handle m_handle;

    // (var) m_content
    // A handle to a GtkFixed object contained inside the actual popup window.
    // This is the true container for the popup's widgets, as a GtkWindow cannot
    // contain more than one widget. gui::widget is designed for use within
    // GtkFixed containers only.
    //
    // The same strategy is used in gui::window.
    sys_handle m_content;

    // (var) m_width, m_height
    // The size of the popup window, in pixels.
    int m_width;
    int m_height;

public:
    // (explicit ctor)
    // FIXME explain
    explicit popup(int width, int height);

    // (dtor)
    // Destroys the popup. If it is visible, it will vanish from the screen.
    ~popup();

    // (func) show_at
    // Makes the popup visible at the specified location in screen coordinates.
    // If the popup is already visible, it is moved to the location.
    //
    // The location specified matches the top-left of the popup.
    void show_at(int x, int y);

    // (func) show_at_mouse
    // Makes the popup visible at the current location of the mouse cursor. If
    // the popup is already visible, it is moved to that location.
    //
    // The location matches the top-left corner of the popup.
    void show_at_mouse();

    // (func) hide
    // Makes the popup invisible. If it is already invisible, no change occurs.
    void hide();

    // (func) set_size
    // Sets the size of the popup, with the same behavior as the size passed to
    // the constructor.
    void set_size(int width, int height);

    // (func) get_container_handle
    // FIXME explain
    sys_handle get_container_handle() override;

    // (func) get_container_size
    // Implements gui::container::get_container_size.
    void get_container_size(int &ctn_w, int &ctn_h) override;
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
    virtual void draw_gl(int width, int height, gl::renderbuffer &rbo) = 0;

    // (func) invalidate
    // Marks the current rendered output of this widget as out-of-date or stale
    // so that draw_gl will be called again. If the widget is not currently
    // visible to the user (due to being hidden, obscured, off-screen, etc)
    // then the call to draw_gl may be deferred until this is not the case, or
    // it may never be called if the widget is destroyed before such a case
    // occurs.
    void invalidate();

public:
    // (ctor)
    // FIXME explain
    widget_gl(container &parent, layout layout);

    using widget::show;
    using widget::hide;
};

/*
 * gui::widget_2d
 *
 * FIXME explain
 */
class widget_2d : private widget {
    friend class window;

private:
    // (s-func) sigh_draw
    // Internal signal handler for GtkWidget::'draw'.
    static gboolean sigh_draw(
        GtkWidget *widget,
        cairo_t *cr,
        gpointer user_data);

protected:
    // (pure func) draw_2d
    // Draws the contents of this widget. This function will not be called on
    // every frame, only when the widget is resized or after invalidate is
    // called (see below).
    virtual void draw_2d(int width, int height, cairo_t *cr) = 0;

    // (func) invalidate
    // Marks the current rendered output of this widget as out-of-date or stale
    // so that draw_2d will be called again. If the widget is not currently
    // visible to the user (due to being hidden, obscured, off-screen, etc)
    // then the call to draw_2d may be deferred until this is not the case, or
    // it may never be called if the widget is destroyed before such a case
    // occurs.
    void invalidate();

public:
    // (ctor)
    // FIXME explain
    widget_2d(container &parent, layout layout);

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

    // (var) m_pending_time
    // The number of milliseconds passed since the last ImGui render. This
    // accumulates from successive calls to `update' (see below) and is then
    // consumed when `ImGui::NewFrame' is run.
    int m_pending_time = 0;

    // (func) draw_gl
    // FIXME explain
    void draw_gl(int width, int height, gl::renderbuffer &rbo) final override;

    // (func) mousemove
    // FIXME explain
    void mousemove(int x, int y) final override;

    // (func) mousewheel
    // FIXME explain
    void mousewheel(int delta_y) final override;

    // (func) mousebutton
    // FIXME explain
    void mousebutton(int number, bool down) final override;

    // (func) key
    // FIXME explain
    void key(int code, bool down) final override;

    // (func) text
    // FIXME explain
    void text(const char *str) final override;

    // (func) update
    // Implements `update' to handle updating ImGui on a regular interval.
    // ImGui is designed for debugging games, and its design necessitates
    // calling imgui every "frame", which is not a concept present in this
    // gui code.
    int update(int delta_ms) final override;

protected:
    // (pure func) frame
    // FIXME explain
    virtual void frame() = 0;

public:
    // (ctor)
    // FIXME explain
    widget_im(container &parent, layout layout);

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
    void draw_2d(int width, int height, cairo_t *cr) override;

public:
    // (ctor)
    // FIXME explain
    label(container &parent, layout layout, const std::string &text = "");

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
class treeview : private widget_im {
public:
    // inner class defined later in this file
    class node;

private:
    // (var) m_nodes
    // The list of pointers to the top-level nodes in the treeview.
    std::list<node *> m_nodes;

    // (var) m_selected_node
    // FIXME explain
    node *m_selected_node = nullptr;

    // (func) frame
    // FIXME explain
    void frame() final override;

public:
    // (ctor)
    // FIXME explain
    treeview(container &parent, layout layout);

    using widget_im::show;
    using widget_im::hide;
};

/*
 * gui::treeview::node
 *
 * FIXME explain
 */
class treeview::node : private util::nocopy {
    friend class treeview;

private:
    // (var) m_view
    // The treeview this node is contained within.
    treeview &m_view;

    // (var) m_parent
    // A pointer to the parent node of this node, or null if this is a
    // top-level node.
    node *m_parent;

    // (var) m_subnodes
    // A list of pointers to the child nodes of this node.
    std::list<node *> m_subnodes;

    // (var) m_iter
    // An iterator pointing to the pointer to this subnode in the parent's
    // node list, or the treeview's node list if this is a top-level node.
    std::list<node *>::iterator m_iter;

    // (var) m_expanded
    // True if this node is expanded (its child nodes are visible) or false
    // otherwise.
    bool m_expanded = false;

    // (var) m_text
    // The text displayed for this tree node.
    std::string m_text;

    // (func) run
    // Handles and displays this treenode (for ImGui).
    void run();

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
 * gui::menu
 *
 * FIXME explain
 */
class menu : private popup, private widget_2d {
    friend class menubar;

public:
    // inner class defined later in this file
    class item;

private:
    // (var) m_items
    // The list of pointers to all of the contained menu items.
    std::vector<item *> m_items;

    // (var) m_active_item
    // A pointer to the currently active item in the menu. This is null if
    // there is no such item. The active item is whatever is currently under
    // the mouse cursor, or potentially an item whose submenu is currently
    // open and active.
    item *m_active_item = nullptr;

    // (func) draw_2d
    // Implements draw_2d to draw the menu items.
    void draw_2d(int width, int height, cairo_t *cr) final override;

    // (func) mousemove
    // Implements mousemove for hovering over menu items.
    void mousemove(int x, int y) final override;

    // (func) mousebutton
    // Implements mousebutton for clicking to activate menu items.
    void mousebutton(int button, bool down) final override;

    // Solve ambiguity of popup::hide vs widget_2d::hide. The widget base
    // should never be hidden.
    using popup::hide;

public:
    // (ctor)
    // Initializes an empty menu.
    menu();
};

/*
 * gui::menu::item
 *
 * FIXME explain
 */
class menu::item : private util::nocopy {
    friend class menu;

private:
    // (var) m_menu
    // The menu this item exists in.
    menu &m_menu;

    // (var) m_text
    // The text displayed on this menu item.
    std::string m_text;

    // (var) m_enabled
    // If false, this menu item does not respond to clicks and may be displayed
    // in a disabled style, such as with gray text.
    bool m_enabled = true;

    // (func) on_activate
    // This function is called when the menu item is clicked by the mouse or
    // activated by a key press.
    //
    // The default implementation performs no action.
    virtual void on_activate() {}

public:
    // (explicit ctor)
    // Initializes a menu item with the given text, and adds it to the given
    // menu.
    explicit item(menu &menu, std::string text);

    // (dtor)
    // Removes the menu item from its associated menu.
    ~item();

    // (func) set_text
    // Sets the text of the menu item.
    void set_text(std::string text);

    // (func) set_enabled
    // Sets whether or not the menu item is enabled. Disabled items cannot be
    // activated by the user, and may appear grayed out.
    void set_enabled(bool enabled);
};

/*
 * gui::menubar
 *
 * FIXME explain
 */
class menubar : private widget_2d {
    friend class window;

public:
    // inner class defined later in this file
    class item;

private:
    // (var) m_wnd
    // The window this menubar is attached to.
    window &m_wnd;

    // (var) m_items
    // The list of pointers to all of the contained menubar items.
    std::vector<item *> m_items;

    // (var) m_hover_item
    // A pointer to the currently selected menubar item, whether by mouse-hover
    // or keyboard input. If there is no such item, this pointer is null.
    item *m_hover_item = nullptr;

    // (var) m_open_item
    // A pointer to the menubar item which has its menu open, or null if there
    // is none.
    item *m_open_item = nullptr;

    // (func) draw_2d
    // Implements draw_2d to draw the menubar items.
    void draw_2d(int width, int height, cairo_t *cr) final override;

    // (func) mousemove
    // Implements mousemove for hovering over menubar items.
    void mousemove(int x, int y) final override;

    // (func) mousebutton
    // Implements mouse click for clicking on menubar items.
    void mousebutton(int button, bool down) final override;

public:
    // (explicit ctor)
    // Initializes an empty menubar and associates it with the given window.
    // The window must not already have a menubar associated with it.
    explicit menubar(window &wnd);

    // (dtor)
    // Removes the menubar from the associated window.
    ~menubar();
};

/*
 * gui::menubar::item
 *
 * FIXME explain
 */
class menubar::item : public menu {
    friend class menubar;

private:
    // (var) m_menubar
    // The menubar this item exists in.
    menubar &m_menubar;

    // (var) m_text
    // The text displayed on this menubar item.
    std::string m_text;

public:
    // (explicit ctor)
    // Initializes a menubar item with the given text, and adds it to the end
    // of the given menubar.
    explicit item(menubar &menubar, std::string text);

    // (dtor)
    // Removes the menubar item from its associated menubar.
    ~item();
};

/*
 * gui::show_open_dialog
 *
 * FIXME explain
 */
bool show_open_dialog(std::string &path);

/*
 * gui::gl_canvas
 *
 * FIXME explain
 */
class gl_canvas : private widget_gl {
private:
    void draw_gl(int width, int height, gl::renderbuffer &rbo) final override;
    void mousemove(int x, int y) final override;
    void mousewheel(int delta_y) final override;
    void mousebutton(int number, bool down) final override;
    void key(int code, bool down) final override;
    void text(const char *str) final override;

    int update(int) final override
    {
        invalidate();
        return INT_MAX;
    }

public:
    // (explicit ctor)
    // FIXME explain
    explicit gl_canvas(container &parent, layout layout);

    // (func) invalidate
    // FIXME explain
    using widget_gl::invalidate;

    // (event) on_render
    // FIXME explain
    util::event<int, int> on_render;

    // (event) on_mousemove
    // FIXME explain
    util::event<int, int> on_mousemove;

    // (event) on_mousewheel
    // FIXME explain
    util::event<int> on_mousewheel;

    // (event) on_mousebutton
    // FIXME explain
    util::event<int, bool> on_mousebutton;

    // (event) on_key
    // FIXME explain
    util::event<int, bool> on_key;

    // (event) on_text
    // FIXME explain
    util::event<const char *> on_text;

    using widget_gl::show;
    using widget_gl::hide;
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
