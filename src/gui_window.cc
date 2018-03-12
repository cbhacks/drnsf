//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2018  DRNSF contributors
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
#endif

namespace drnsf {
namespace gui {

#if USE_X11
// defined in gui.cc
extern Display *g_display;

// defined in gui.cc
extern XContext g_ctx_ptr;
#endif

// declared in gui.hh
std::unordered_set<window *> window::s_all_windows;

// declared in gui.hh
window::window(const std::string &title, int width, int height) :
    m_width(width),
    m_height(height)
{
#if USE_X11
    XSetWindowAttributes attr{};
    attr.background_pixel = WhitePixel(g_display, DefaultScreen(g_display));
    attr.event_mask = StructureNotifyMask;
    m_handle = XCreateWindow(
        g_display,
        DefaultRootWindow(g_display),
        0, 0,
        width, height,
        0,
        CopyFromParent,
        InputOutput,
        CopyFromParent,
        CWBackPixel | CWEventMask,
        &attr
    );
    // FIXME on error free window

    XStoreName(g_display, m_handle, title.c_str());
    XSaveContext(g_display, m_handle, g_ctx_ptr, XPointer(this));
#else
#error Unimplemented UI frontend code.
#endif

    s_all_windows.insert(this);
}

// declared in gui.hh
window::~window()
{
    s_all_windows.erase(this);

#if USE_X11
    XDestroyWindow(g_display, m_handle);
#else
#error Unimplemented UI frontend code.
#endif
}

// declared in gui.hh
void window::show()
{
#if USE_X11
    XMapWindow(g_display, m_handle);
#else
#error Unimplemented UI frontend code.
#endif
}

// declared in gui.hh
void window::show_dialog()
{
#if USE_X11
    XMapWindow(g_display, m_handle);
    // FIXME modal window
    run();
    XUnmapWindow(g_display, m_handle);
#endif
}

// declared in gui.hh
sys_handle window::get_container_handle()
{
    return m_handle;
}

// declared in gui.hh
void window::get_child_area(int &ctn_x, int &ctn_y, int &ctn_w, int &ctn_h)
{
    ctn_x = 0;
    ctn_y = 0;
    ctn_w = m_width;
    ctn_h = m_height;

    // Adjust the area for a non-native menubar if present. The menubar's own
    // layout places it above the top of the child area.
    if (m_menubar) {
        ctn_y += 20;
        ctn_h -= 20;
    }
}

}
}
