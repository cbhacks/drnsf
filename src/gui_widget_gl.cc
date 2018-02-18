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

#if USE_X11
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <epoxy/glx.h>

namespace drnsf {
namespace gl {
// defined in gl.cc
extern Window g_wnd;

// defined in gl.cc
extern GLXContext g_ctx;

// defined in gl.cc
extern XVisualInfo *g_vi;
}
}
#endif

namespace drnsf {
namespace gui {

#if USE_X11
// defined in gui.cc
extern Display *g_display;

// defined in gui.cc
extern XContext g_ctx_ptr;

// declared in gui.hh
void widget_gl::on_draw()
{
    int width, height;
    get_real_size(width, height);

    gl::renderbuffer rbo;
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glViewport(0, 0, width, height);
    glScissor(0, 0, width, height);

    draw_gl(width, height, rbo);

    gl::framebuffer fbo;
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glFramebufferRenderbuffer(
        GL_READ_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_RENDERBUFFER,
        rbo
    );
    glXMakeCurrent(g_display, m_handle, gl::g_ctx);
    glBlitFramebuffer(
        0, 0,
        width, height,
        0, 0,
        width, height,
        GL_COLOR_BUFFER_BIT,
        GL_NEAREST
    );
    glXSwapBuffers(g_display, m_handle);
    glXMakeCurrent(g_display, gl::g_wnd, gl::g_ctx);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}
#endif

// declared in gui.hh
widget_gl::widget_gl(container &parent, layout layout) :
    widget(parent)
{
#if USE_X11
    Colormap cmap = XCreateColormap(
        g_display,
        DefaultRootWindow(g_display),
        gl::g_vi->visual,
        AllocNone
    );

    XSetWindowAttributes attr{};
    attr.event_mask =
        ButtonPressMask |
        ButtonReleaseMask |
        KeyPressMask |
        KeyReleaseMask |
        EnterWindowMask |
        LeaveWindowMask |
        PointerMotionMask |
        ExposureMask |
        StructureNotifyMask;
    attr.colormap = cmap;
    m_handle = XCreateWindow(
        g_display,
        parent.get_container_handle(),
        0, 0,
        1, 1,
        0,
        gl::g_vi->depth,
        InputOutput,
        gl::g_vi->visual,
        CWEventMask | CWColormap,
        &attr
    );
    if (!m_handle) {
        throw 0;//FIXME
    }
    // m_handle is released by the base class destructor on exception.

    XSaveContext(g_display, m_handle, g_ctx_ptr, XPointer(this));
#else
#error Unimplemented UI frontend code.
#endif

    set_layout(layout);
}

// declared in gui.hh
void widget_gl::invalidate()
{
#if USE_X11
    m_dirty = true;
#else
#error Unimplemented UI frontend code.
#endif
}

}
}
