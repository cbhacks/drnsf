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

namespace drnsf {

namespace gui {

// declared in gui.hh
gboolean widget_2d::sigh_draw(
    GtkWidget *widget,
    cairo_t *cr,
    gpointer user_data)
{
    auto self = static_cast<widget_2d *>(user_data);

    auto width = gtk_widget_get_allocated_width(widget);
    auto height = gtk_widget_get_allocated_height(widget);

    self->draw_2d(width,height,cr);

    return true;
}

// declared in gui.hh
gboolean widget_2d::sigh_motion_notify_event(
    GtkWidget *widget,
    GdkEvent *event,
    gpointer user_data)
{
    static_cast<widget_2d *>(user_data)->mousemove(
        event->motion.x,
        event->motion.y
    );
    return true;
}

// declared in gui.hh
gboolean widget_2d::sigh_scroll_event(
    GtkWidget *widget,
    GdkEvent *event,
    gpointer user_data)
{
    if (event->scroll.direction != GDK_SCROLL_SMOOTH) {
        return false;
    }

    static_cast<widget_2d *>(user_data)->mousewheel(
        -event->scroll.delta_y
    );
    return true;
}

// declared in gui.hh
gboolean widget_2d::sigh_button_event(
    GtkWidget *widget,
    GdkEvent *event,
    gpointer user_data)
{
    static_cast<widget_2d *>(user_data)->mousebutton(
        event->button.button,
        event->button.type == GDK_BUTTON_PRESS
    );
    return true;
}

// declared in gui.hh
gboolean widget_2d::sigh_key_event(
    GtkWidget *widget,
    GdkEvent *event,
    gpointer user_data)
{
    auto self = static_cast<widget_2d *>(user_data);
    self->key(
        event->key.keyval,
        event->key.type == GDK_KEY_PRESS
    );
    if (event->key.type == GDK_KEY_PRESS) {
        // FIXME deprecated garbage
        self->text(event->key.string);
    }
    return true;
}

// declared in gui.hh
widget_2d::widget_2d(container &parent,layout layout) :
    widget(gtk_drawing_area_new(),parent,layout)
{
    gtk_widget_add_events(
        m_handle,
        GDK_POINTER_MOTION_MASK |
        GDK_SMOOTH_SCROLL_MASK |
        GDK_BUTTON_PRESS_MASK |
        GDK_BUTTON_RELEASE_MASK |
        GDK_KEY_PRESS_MASK |
        GDK_KEY_RELEASE_MASK
    );
    gtk_widget_set_can_focus(m_handle,true);
    g_signal_connect(m_handle,"draw",G_CALLBACK(sigh_draw),this);
    g_signal_connect(
        m_handle,
        "motion-notify-event",
        G_CALLBACK(sigh_motion_notify_event),
        this
    );
    g_signal_connect(
        m_handle,
        "scroll-event",
        G_CALLBACK(sigh_scroll_event),
        this
    );
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
}

// declared in gui.hh
void widget_2d::invalidate()
{
    gtk_widget_queue_draw_area(
        m_handle,
        0,
        0,
        gtk_widget_get_allocated_width(m_handle),
        gtk_widget_get_allocated_height(m_handle)
    );
}

}
}
