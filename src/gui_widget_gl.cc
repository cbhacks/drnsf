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

// defined in gl.cc
namespace gl {
    extern GdkWindow *g_wnd;
    extern GdkGLContext *g_glctx;
}

namespace gui {

// declared in gui.hh
gboolean widget_gl::sigh_draw(
    GtkWidget *widget,
    cairo_t *cr,
    gpointer user_data)
{
    auto self = static_cast<widget_gl *>(user_data);

    auto width = gtk_widget_get_allocated_width(widget);
    auto height = gtk_widget_get_allocated_height(widget);

    gl::renderbuffer rbo;

    glBindRenderbuffer(GL_RENDERBUFFER,rbo);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_RGB8,width,height);
    glBindRenderbuffer(GL_RENDERBUFFER,0);

    glViewport(0,0,width,height);
    glScissor(0,0,width,height);

    self->draw_gl(width,height,rbo);

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
}

// declared in gui.hh
gboolean widget_gl::sigh_motion_notify_event(
    GtkWidget *widget,
    GdkEvent *event,
    gpointer user_data)
{
    static_cast<widget_gl *>(user_data)->mousemove(
        event->motion.x,
        event->motion.y
    );
    return true;
}

// declared in gui.hh
gboolean widget_gl::sigh_scroll_event(
    GtkWidget *widget,
    GdkEvent *event,
    gpointer user_data)
{
    if (event->scroll.direction != GDK_SCROLL_SMOOTH) {
        return false;
    }

    static_cast<widget_gl *>(user_data)->mousewheel(
        -event->scroll.delta_y
    );
    return true;
}

// declared in gui.hh
gboolean widget_gl::sigh_button_event(
    GtkWidget *widget,
    GdkEvent *event,
    gpointer user_data)
{
    static_cast<widget_gl *>(user_data)->mousebutton(
        event->button.button,
        event->button.type == GDK_BUTTON_PRESS
    );
    return true;
}

// declared in gui.hh
gboolean widget_gl::sigh_key_event(
    GtkWidget *widget,
    GdkEvent *event,
    gpointer user_data)
{
    auto self = static_cast<widget_gl *>(user_data);
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
widget_gl::widget_gl(container &parent) :
    widget(gtk_drawing_area_new(),parent)
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
void widget_gl::invalidate()
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
