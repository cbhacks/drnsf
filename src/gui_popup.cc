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
std::unordered_set<popup *> popup::s_all_popups;

// declared in gui.hh
popup::popup(int width, int height) :
    m_width(width),
    m_height(height)
{
#if USE_X11
    XSetWindowAttributes attr{};
    attr.background_pixel = WhitePixel(g_display, DefaultScreen(g_display));
    attr.event_mask = StructureNotifyMask;
    attr.override_redirect = true;
    m_handle = XCreateWindow(
        g_display,
        DefaultRootWindow(g_display),
        0, 0,
        width, height,
        0,
        CopyFromParent,
        InputOutput,
        CopyFromParent,
        CWBackPixel | CWOverrideRedirect | CWEventMask,
        &attr
    );
    // FIXME on error free window
    // FIXME make exempt from usual WM top-level window usage

    XSaveContext(g_display, m_handle, g_ctx_ptr, XPointer(this));
#else
#error Unimplemented UI frontend code.
#endif

    s_all_popups.insert(this);
}

// declared in gui.hh
popup::~popup()
{
    s_all_popups.erase(this);

#if USE_X11
    XDestroyWindow(g_display, m_handle);
#else
#error Unimplemented UI frontend code.
#endif
}

// declared in gui.hh
void popup::show_at(int x, int y)
{
#if USE_X11
    XMoveWindow(g_display, m_handle, x, y);
    XMapWindow(g_display, m_handle);
#else
#error Unimplemented UI frontend code.
#endif
}

// declared in gui.hh
void popup::hide()
{
#if USE_X11
    XUnmapWindow(g_display, m_handle);
#else
#error Unimplemented UI frontend code.
#endif
}

// declared in gui.hh
void popup::set_size(int width, int height)
{
#if USE_X11
    XResizeWindow(g_display, m_handle, width, height);
#else
#error Unimplemented UI frontend code.
#endif
}

// declared in gui.hh
sys_handle popup::get_container_handle()
{
    return m_handle;
}

// declared in gui.hh
void popup::get_child_area(int &ctn_x, int &ctn_y, int &ctn_w, int &ctn_h)
{
    ctn_x = 0;
    ctn_y = 0;
    ctn_w = m_width;
    ctn_h = m_height;
}

}
}
