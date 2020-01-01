//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2020  DRNSF contributors
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
#define DRNSF_FRONTEND_IMPLEMENTATION
#include "gl.hh"
#include "gui.hh"

namespace drnsf {
namespace gl {

// declared in gl.hh
Window g_wnd;

// declared in gl.hh
GLXContext g_ctx;

// declared in gl.hh
XVisualInfo *g_vi;

// (s-var) s_cmap
// Colormap for the GLX background window.
static Colormap s_cmap;

namespace impl {

// declared in gl.cc
void init()
{
    using gui::g_display;

    int glx_attrs[] = {
        GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
        GLX_RENDER_TYPE,   GLX_RGBA_BIT,
        GLX_DOUBLEBUFFER,  True,
        None
    };
    int fbc_count;
    GLXFBConfig *fbcs = glXChooseFBConfig(
        g_display,
        DefaultScreen(g_display),
        glx_attrs,
        &fbc_count
    );
    DRNSF_ON_EXIT { XFree(fbcs); };

    if (fbc_count <= 0) {
        throw error("gl::init: failed to choose GLX FB config");
    }

    auto fbc = fbcs[0];

    g_vi = glXGetVisualFromFBConfig(g_display, fbc);
    if (!g_vi) {
        throw error("gl::init: failed to choose X visual");
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

        throw error("gl::init: failed to create basic context");
    }
    // FIXME release context on error

    if (!glXMakeCurrent(g_display, g_wnd, g_ctx)) {
        throw error("gl::init: failed to activate basic context");
    }

    auto glXCreateContextAttribs =
        reinterpret_cast<PFNGLXCREATECONTEXTATTRIBSARBPROC>(glXGetProcAddress(
            reinterpret_cast<const unsigned char *>(
                &"glXCreateContextAttribsARB"[0]
            )
        ));

    if (!glXCreateContextAttribs) {
        throw error("gl::init: failed to find glXCreateContextAttribsARB");
    }

    int core_attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 2,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        0
    };
    auto core_ctx = glXCreateContextAttribs(g_display, fbc, 0, true /* ??? */, core_attribs); //FIXME
    if (core_ctx) {
        glXMakeCurrent(g_display, None, nullptr);
        glXDestroyContext(g_display, g_ctx);
        g_ctx = core_ctx;

        if (!glXMakeCurrent(g_display, g_wnd, g_ctx)) {
            throw error("gl::init: failed to activate core context");
        }

        if (epoxy_gl_version() < 32) {
            throw error("gl::init: core context version less than 3.2");
        }

        return;
    }

    int fwd_attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 1,
        GLX_CONTEXT_FLAGS_ARB, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        0
    };
    auto fwd_ctx = glXCreateContextAttribs(g_display, fbc, 0, true /* ??? */, fwd_attribs); //FIXME
    if (fwd_ctx) {
        glXMakeCurrent(g_display, None, nullptr);
        glXDestroyContext(g_display, g_ctx);
        g_ctx = fwd_ctx;

        if (!glXMakeCurrent(g_display, g_wnd, g_ctx)) {
            throw error("gl::init: failed to activate forward context");
        }

        if (epoxy_gl_version() < 31) {
            throw error("gl::init: forward context version less than 3.1");
        }

        return;
    }
    throw error("gl::init failed to create a 3.1 forward or 3.2 core context");
}

// declared in gl.cc
void shutdown() noexcept
{
    using gui::g_display;
    glXMakeCurrent(g_display, None, nullptr);
    glXDestroyContext(g_display, g_ctx);
    XDestroyWindow(g_display, g_wnd);
}

}

}
}
