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

// defined in gl.cc
namespace gl {
    extern GdkWindow *g_wnd;
    extern GdkGLContext *g_glctx;
}

namespace gui {

// declared in gui.hh
widget_gl::widget_gl(container &parent, layout layout) :
    widget(parent)
{
    m_handle = gtk_drawing_area_new();
    gtk_container_add(
        GTK_CONTAINER(parent.get_container_handle()),
        GTK_WIDGET(m_handle)
    );
    gtk_widget_set_can_focus(GTK_WIDGET(m_handle), true);

    // Register event handlers.
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
                static_cast<widget_gl *>(user_data)->mousemove(
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

                static_cast<widget_gl *>(user_data)->mousewheel(
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
                static_cast<widget_gl *>(user_data)->mousebutton(
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
                auto self = static_cast<widget_gl *>(user_data);
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
                static_cast<widget_gl *>(user_data)->on_resize(
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

    auto sigh_draw =
        static_cast<gboolean (*)(GtkWidget *, cairo_t *, gpointer)>(
            [](GtkWidget *widget, cairo_t *cr, gpointer user_data) -> gboolean {
                auto self = static_cast<widget_gl *>(user_data);

                auto width = gtk_widget_get_allocated_width(widget);
                auto height = gtk_widget_get_allocated_height(widget);

                gl::renderbuffer rbo;

                glBindRenderbuffer(GL_RENDERBUFFER, rbo);
                glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB8, width, height);
                glBindRenderbuffer(GL_RENDERBUFFER, 0);

                glViewport(0, 0, width, height);
                glScissor(0, 0, width, height);

                self->draw_gl(width, height, rbo);

                gdk_cairo_draw_from_gl(cr,
                    gl::g_wnd,
                    rbo,
                    GL_RENDERBUFFER,
                    1,
                    0,
                    0,
                    width,
                    height
                );

                gdk_gl_context_make_current(gl::g_glctx);

                return true;
            });
    g_signal_connect(m_handle, "draw", G_CALLBACK(sigh_draw), this);

    set_layout(layout);
}

// declared in gui.hh
void widget_gl::invalidate()
{
    gtk_widget_queue_draw_area(
        GTK_WIDGET(m_handle),
        0,
        0,
        gtk_widget_get_allocated_width(GTK_WIDGET(m_handle)),
        gtk_widget_get_allocated_height(GTK_WIDGET(m_handle))
    );
}

}
}
