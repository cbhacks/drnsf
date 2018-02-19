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
#include "gl.hh"

#if USE_X11
#include <X11/Xlib.h>
#include <epoxy/glx.h>

namespace drnsf {
namespace gui {
// defined in gui.cc
extern Display *g_display;
}
}
#endif

namespace drnsf {
namespace gl {

#if USE_X11
// (var) g_wnd
// The background window which the GL context is bound to.
Window g_wnd;

// (var) g_ctx
// The GL context.
GLXContext g_ctx;

// (var) g_vi
// XVisualInfo pointer used for creating GLX-capable windows.
XVisualInfo *g_vi;

// (s-var) s_cmap
// Colormap for the GLX background window.
static Colormap s_cmap;
#endif

// declared in gl.hh
void init()
{
#if USE_X11
    using gui::g_display;

    int glx_attrs[] = { GLX_RGBA, GLX_DOUBLEBUFFER, None };
    g_vi = glXChooseVisual(g_display, DefaultScreen(g_display), glx_attrs);
    if (!g_vi) {
        throw std::runtime_error("gl::init: failed to choose X visual");
    }
    // FIXME on error release g_glx_vi

    s_cmap = XCreateColormap(
        g_display,
        DefaultRootWindow(g_display),
        g_vi->visual,
        AllocNone
    );

    XSetWindowAttributes x_attrs{};
    x_attrs.colormap = s_cmap;
    g_wnd = XCreateWindow(
        g_display,
        DefaultRootWindow(g_display),
        0, 0,
        1, 1,
        0,
        g_vi->depth,
        InputOutput,
        g_vi->visual,
        CWColormap,
        &x_attrs
    );

    g_ctx = glXCreateContext(g_display, g_vi, nullptr, true);
    if (!g_ctx) {
        // NOTE: glXCreateContext may also fail on the X server side, which may
        // not return NULL.

        throw std::runtime_error("gl::init: failed to create context");
    }
    // FIXME release context on error

    if (!glXMakeCurrent(g_display, g_wnd, g_ctx)) {
        throw std::runtime_error("gl::init: failed to activate context");
    }
#else
#error Unimplemented UI frontend code.
#endif
}

// declared in gl.hh
void shutdown()
{
#if USE_X11
    any_object::reset_all();

    using gui::g_display;
    glXMakeCurrent(g_display, None, nullptr);
    glXDestroyContext(g_display, g_ctx);
    XDestroyWindow(g_display, g_wnd);
#else
#error Unimplemented UI frontend code.
#endif
}

// declared in gl.hh
std::unordered_set<any_object *> &any_object::get_all_objects()
{
    static std::unordered_set<any_object *> s_all_objects;
    return s_all_objects;
}

// declared in gl.hh
void any_object::reset_all()
{
    for (auto &&p : get_all_objects()) {
        p->reset();
    }
}

}
}
