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

#include "common.hh"
#include "gui.hh"

#if USE_GTK3
#include <gtk/gtk.h>
#endif

namespace drnsf {
namespace gui {

// declared in gui.hh
std::unordered_map<sys_handle, widget *> widget::s_all_widgets;

// declared in gui.hh
// FIXME move to gui_container.cc ?
void container::apply_layouts()
{
    int x, y, w, h;
    get_child_area(x, y, w, h);
    for (auto &&widget : m_widgets) {
        widget->apply_layout(x, y, w, h);
    }
}

// declared in gui.hh
void widget::apply_layout(int ctn_x, int ctn_y, int ctn_w, int ctn_h)
{
    // Offset the specified region according to the parent container's own
    // allocation. Our input values ctn_x and ctn_y are relative to the parent,
    // but gtk_widget_size_allocate expects top-level window-relative units.
    GtkAllocation parent_rect;
    gtk_widget_get_allocation(
        GTK_WIDGET(m_parent.get_container_handle()),
        &parent_rect
    );
    ctn_x += parent_rect.x;
    ctn_y += parent_rect.y;

    // Calculate the real positions according to the widget layout.
    int x1 =
        ctn_x +
        ctn_w *
        m_layout.h.left.factor +
        m_layout.h.left.offset;
    int x2 =
        ctn_x +
        ctn_w *
        m_layout.h.right.factor +
        m_layout.h.right.offset;
    int y1 =
        ctn_y +
        ctn_h *
        m_layout.v.top.factor +
        m_layout.v.top.offset;
    int y2 =
        ctn_y +
        ctn_h *
        m_layout.v.bottom.factor +
        m_layout.v.bottom.offset;

    // Submit the new size and position.
    m_real_width = x2 - x1;
    m_real_height = y2 - y1;
    GtkAllocation rect = { x1, y1, x2 - x1, y2 - y1 };
    gtk_widget_size_allocate(GTK_WIDGET(m_handle), &rect);
}

// declared in gui.hh
widget::widget(sys_handle &&handle, container &parent) try :
    m_handle(handle),
    m_parent(parent)
{
    // Add the underlying GTK widget to the parent container.
    gtk_container_add(
        GTK_CONTAINER(parent.get_container_handle()),
        GTK_WIDGET(m_handle)
    );

    // Register event handlers. "draw" is handled by derived types such as
    // "widget_gl" and "widget_2d".
    gtk_widget_add_events(
        GTK_WIDGET(m_handle),
        GDK_POINTER_MOTION_MASK |
        GDK_SMOOTH_SCROLL_MASK |
        GDK_BUTTON_PRESS_MASK |
        GDK_BUTTON_RELEASE_MASK |
        GDK_KEY_PRESS_MASK |
        GDK_KEY_RELEASE_MASK
    );
    auto sigh_motion_notify_event =
        static_cast<gboolean (*)(GtkWidget *,GdkEvent *, gpointer user_data)>(
            [](GtkWidget *wdg, GdkEvent *event, gpointer user_data) -> gboolean {
                static_cast<widget *>(user_data)->mousemove(
                    event->motion.x,
                    event->motion.y
                );
                return false;
            });
    g_signal_connect(
        m_handle,
        "motion-notify-event",
        G_CALLBACK(sigh_motion_notify_event),
        this
    );
    auto sigh_scroll_event =
        static_cast<gboolean (*)(GtkWidget *,GdkEvent *, gpointer user_data)>(
            [](GtkWidget *wdg, GdkEvent *event, gpointer user_data) -> gboolean {
                if (event->scroll.direction != GDK_SCROLL_SMOOTH) {
                    return false;
                }

                static_cast<widget *>(user_data)->mousewheel(
                    -event->scroll.delta_y
                );
                return false;
            });
    g_signal_connect(
        m_handle,
        "scroll-event",
        G_CALLBACK(sigh_scroll_event),
        this
    );
    auto sigh_button_event =
        static_cast<gboolean (*)(GtkWidget *,GdkEvent *, gpointer user_data)>(
            [](GtkWidget *wdg, GdkEvent *event, gpointer user_data) -> gboolean {
                static_cast<widget *>(user_data)->mousebutton(
                    event->button.button,
                    event->button.type == GDK_BUTTON_PRESS
                );
                return false;
            });
    g_signal_connect(
        m_handle,
        "button-press-event",
        G_CALLBACK(sigh_button_event),
        this
    );
    g_signal_connect(
        m_handle,
        "button-release-event",
        G_CALLBACK(sigh_button_event),
        this
    );
    auto sigh_key_event =
        static_cast<gboolean (*)(GtkWidget *,GdkEvent *, gpointer user_data)>(
            [](GtkWidget *wdg, GdkEvent *event, gpointer user_data) -> gboolean {
                auto self = static_cast<widget *>(user_data);
                self->key(
                    event->key.keyval,
                    event->key.type == GDK_KEY_PRESS
                );
                if (event->key.type == GDK_KEY_PRESS) {
                    // FIXME deprecated garbage
                    self->text(event->key.string);
                }
                return false;
            });
    g_signal_connect(
        m_handle,
        "key-press-event",
        G_CALLBACK(sigh_key_event),
        this
    );
    g_signal_connect(
        m_handle,
        "key-release-event",
        G_CALLBACK(sigh_key_event),
        this
    );
    auto sigh_size_allocate =
        static_cast<void (*)(GtkWidget *,GdkRectangle *, gpointer user_data)>(
            [](GtkWidget *wdg, GdkRectangle *allocation, gpointer user_data) {
                static_cast<widget *>(user_data)->on_resize(
                    allocation->width,
                    allocation->height
                );
            });
    g_signal_connect(
        m_handle,
        "size-allocate",
        G_CALLBACK(sigh_size_allocate),
        this
    );

    parent.m_widgets.insert(this);
    try {
        s_all_widgets.insert({m_handle, this});
    } catch (...) {
        parent.m_widgets.erase(this);
        throw;
    }
} catch (...) {
    // Destroy the handle (GTK widget) given to us if an exception is thrown.
    gtk_widget_destroy(GTK_WIDGET(handle));
    throw;
}

// declared in gui.hh
widget::~widget()
{
    m_parent.m_widgets.erase(this);
    s_all_widgets.erase(m_handle);
    gtk_widget_destroy(GTK_WIDGET(m_handle));
}

// declared in gui.hh
void widget::show()
{
    gtk_widget_show(GTK_WIDGET(m_handle));
}

// declared in gui.hh
void widget::hide()
{
    gtk_widget_hide(GTK_WIDGET(m_handle));
}

// declared in gui.hh
const layout &widget::get_layout() const
{
    return m_layout;
}

// declared in gui.hh
void widget::set_layout(layout layout)
{
    m_layout = layout;

    // Reconfigure the widget according to the new layout.
    int ctn_x;
    int ctn_y;
    int ctn_w;
    int ctn_h;
    m_parent.get_child_area(ctn_x, ctn_y, ctn_w, ctn_h);
    apply_layout(ctn_x, ctn_y, ctn_w, ctn_h);
}

// declared in gui.hh
void widget::get_real_size(int &width, int &height)
{
    width = m_real_width;
    height = m_real_height;
}

}
}
