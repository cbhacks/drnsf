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
#include <iostream>
#include "gui.hh"

namespace drnsf {

// defined in gl.cc
namespace gl {
    extern GdkWindow *g_wnd;
    extern GdkGLContext *g_glctx;
}

namespace gui {

// declared in gui.hh
gboolean gl_canvas::sigh_draw(
    GtkWidget *widget,
    cairo_t *cr,
    gpointer user_data)
{
    auto self = static_cast<gl_canvas *>(user_data);

    auto width = gtk_widget_get_allocated_width(widget);
    auto height = gtk_widget_get_allocated_height(widget);

    gl::framebuffer fbo;

    gl::renderbuffer rbo_color;
    gl::renderbuffer rbo_depth;

    glBindRenderbuffer(GL_RENDERBUFFER,rbo_color);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_RGB8,width,height);
    glBindRenderbuffer(GL_RENDERBUFFER,rbo_depth);
    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH_COMPONENT16,
        width,
        height
    );
    glBindRenderbuffer(GL_RENDERBUFFER,0);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,fbo);
    glFramebufferRenderbuffer(
        GL_DRAW_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_RENDERBUFFER,
        rbo_color
    );
    glFramebufferRenderbuffer(
        GL_DRAW_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        rbo_depth
    );

    glViewport(0,0,width,height);

    self->on_render(width,height);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

    gdk_cairo_draw_from_gl(
        cr,
        gl::g_wnd,
        rbo_color,
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
gboolean gl_canvas::sigh_motion_notify_event(
    GtkWidget *widget,
    GdkEvent *event,
    gpointer user_data)
{
    static_cast<gl_canvas *>(user_data)->on_mousemove(
        event->motion.x,
        event->motion.y
    );
    return true;
}

// declared in gui.hh
gboolean gl_canvas::sigh_scroll_event(
    GtkWidget *widget,
    GdkEvent *event,
    gpointer user_data)
{
    if (event->scroll.direction != GDK_SCROLL_SMOOTH) {
        return false;
    }

    static_cast<gl_canvas *>(user_data)->on_mousewheel(
        -event->scroll.delta_y
    );
    return true;
}

// declared in gui.hh
gboolean gl_canvas::sigh_button_event(
    GtkWidget *widget,
    GdkEvent *event,
    gpointer user_data)
{
    static_cast<gl_canvas *>(user_data)->on_mousebutton(
        event->button.button,
        event->button.type == GDK_BUTTON_PRESS
    );
    return true;
}

// declared in gui.hh
gboolean gl_canvas::sigh_key_event(
    GtkWidget *widget,
    GdkEvent *event,
    gpointer user_data)
{
    auto self = static_cast<gl_canvas *>(user_data);
    self->on_key(
        event->key.keyval,
        event->key.type == GDK_KEY_PRESS
    );
    if (event->key.type == GDK_KEY_PRESS) {
        // FIXME deprecated garbage
        self->on_text(event->key.string);
    }
    return true;
}

// declared in gui.hh
GtkWidget *gl_canvas::get_handle()
{
    return M;
}

// declared in gui.hh
gl_canvas::gl_canvas(container &parent)
{
    M = gtk_drawing_area_new();
    gtk_widget_set_events(
        M,
        GDK_POINTER_MOTION_MASK |
        GDK_SMOOTH_SCROLL_MASK |
        GDK_BUTTON_PRESS_MASK |
        GDK_BUTTON_RELEASE_MASK |
        GDK_KEY_PRESS_MASK |
        GDK_KEY_RELEASE_MASK
    );
    gtk_widget_set_can_focus(M,true);
    g_signal_connect(M,"draw",G_CALLBACK(sigh_draw),this);
    g_signal_connect(
        M,
        "motion-notify-event",
        G_CALLBACK(sigh_motion_notify_event),
        this
    );
    g_signal_connect(
        M,
        "scroll-event",
        G_CALLBACK(sigh_scroll_event),
        this
    );
    g_signal_connect(
        M,
        "button-press-event",
        G_CALLBACK(sigh_button_event),
        this
    );
    g_signal_connect(
        M,
        "button-release-event",
        G_CALLBACK(sigh_button_event),
        this
    );
    g_signal_connect(
        M,
        "key-press-event",
        G_CALLBACK(sigh_key_event),
        this
    );
    g_signal_connect(
        M,
        "key-release-event",
        G_CALLBACK(sigh_key_event),
        this
    );
    gtk_container_add(parent.get_container_handle(),M);
}

// declared in gui.hh
gl_canvas::~gl_canvas()
{
    gtk_widget_destroy(M);
}

// declared in gui.hh
void gl_canvas::invalidate()
{
    gtk_widget_queue_draw_area(
        M,
        0,
        0,
        gtk_widget_get_allocated_width(M),
        gtk_widget_get_allocated_height(M)
    );
}

}
}
