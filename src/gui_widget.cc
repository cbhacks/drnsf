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
#endif

namespace drnsf {
namespace gui {

#if USE_X11
// defined in gui.cc
extern Display *g_display;
#endif

// declared in gui.hh
std::unordered_set<widget *> widget::s_all_widgets;

// declared in gui.hh
// FIXME move to gui_container.cc ?
void container::apply_layouts()
{
    int x, y, w, h;
    get_child_area(x, y, w, h);
    for (auto &&widget : m_widgets) {
        widget->apply_layout(x, y, w, h);
    }
}

// declared in gui.hh
void widget::apply_layout(int ctn_x, int ctn_y, int ctn_w, int ctn_h)
{
    // Calculate the real positions according to the widget layout.
    int x1 =
        ctn_x +
        ctn_w *
        m_layout.h.left.factor +
        m_layout.h.left.offset;
    int x2 =
        ctn_x +
        ctn_w *
        m_layout.h.right.factor +
        m_layout.h.right.offset;
    int y1 =
        ctn_y +
        ctn_h *
        m_layout.v.top.factor +
        m_layout.v.top.offset;
    int y2 =
        ctn_y +
        ctn_h *
        m_layout.v.bottom.factor +
        m_layout.v.bottom.offset;

    // Submit the new size and position.
    m_real_width = x2 - x1;
    m_real_height = y2 - y1;

#if USE_X11
    XMoveResizeWindow(g_display, m_handle, x1, y1, x2 - x1, y2 - y1);
#else
#error Unimplemented UI frontend code.
#endif
}

// declared in gui.hh
widget::widget(container &parent) :
    m_parent(parent)
{
    parent.m_widgets.insert(this);
    try {
        s_all_widgets.insert(this);
    } catch (...) {
        parent.m_widgets.erase(this);
        throw;
    }
}

// declared in gui.hh
widget::~widget()
{
    m_parent.m_widgets.erase(this);
    s_all_widgets.erase(this);

    // The handle is set by the derived class, but is released by the base
    // destructor. A derived type could release it manually and set m_handle
    // null if necessary.
    if (m_handle) {
#if USE_X11
        XDestroyWindow(g_display, m_handle);
#else
#error Unimplemented UI frontend code.
#endif
    }
}

// declared in gui.hh
void widget::show()
{
#if USE_X11
    XMapWindow(g_display, m_handle);
#else
#error Unimplemented UI frontend code.
#endif
}

// declared in gui.hh
void widget::hide()
{
#if USE_X11
    XUnmapWindow(g_display, m_handle);
#else
#error Unimplemented UI frontend code.
#endif
}

// declared in gui.hh
const layout &widget::get_layout() const
{
    return m_layout;
}

// declared in gui.hh
void widget::set_layout(layout layout)
{
    m_layout = layout;

    // Reconfigure the widget according to the new layout.
    int ctn_x;
    int ctn_y;
    int ctn_w;
    int ctn_h;
    m_parent.get_child_area(ctn_x, ctn_y, ctn_w, ctn_h);
    apply_layout(ctn_x, ctn_y, ctn_w, ctn_h);
}

// declared in gui.hh
void widget::get_real_size(int &width, int &height)
{
    width = m_real_width;
    height = m_real_height;
}

}
}
