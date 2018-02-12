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
#include <cairo-xlib.h>
#endif

namespace drnsf {
namespace gui {

#if USE_X11
// defined in gui.cc
extern Display *g_display;

// defined in gui.cc
extern XContext g_ctx_ptr;

// declared in gui.hh
void widget_2d::on_draw()
{
    int width, height;
    get_real_size(width, height);

    auto surface = cairo_xlib_surface_create(
        g_display,
        m_handle,
        DefaultVisual(g_display, DefaultScreen(g_display)),
        width,
        height
    );
    DRNSF_ON_EXIT { cairo_surface_destroy(surface); };

    auto cr = cairo_create(surface);
    DRNSF_ON_EXIT { cairo_destroy(cr); };

    // Clear the background. We do this here instead of using a white pixel
    // background to avoid flickering issues when resizing (especially in
    // debug builds, due to XSynchronize).
    cairo_save(cr);
    cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
    cairo_rectangle(cr, 0, 0, width, height);
    cairo_fill(cr);
    cairo_restore(cr);

    draw_2d(width, height, cr);
}
#endif

// declared in gui.hh
widget_2d::widget_2d(container &parent, layout layout) :
    widget(parent)
{
#if USE_X11
    XSetWindowAttributes attr{};
    attr.event_mask =
        ButtonPressMask |
        ButtonReleaseMask |
        KeyPressMask |
        KeyReleaseMask |
        PointerMotionMask |
        ExposureMask |
        StructureNotifyMask;
    m_handle = XCreateWindow(
        g_display,
        parent.get_container_handle(),
        0, 0,
        1, 1,
        0,
        CopyFromParent,
        InputOutput,
        CopyFromParent,
        CWEventMask,
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
void widget_2d::invalidate()
{
#if USE_X11
    m_dirty = true;
#else
#error Unimplemented UI frontend code.
#endif
}

}
}
