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
namespace gui {

// declared in gui.hh
// FIXME move to gui_container.cc ?
void container::apply_layouts(GtkAllocation &alloc)
{
    for (auto &&widget : m_widgets) {
        widget->apply_layout(alloc);
    }
}

// declared in gui.hh
void widget::apply_layout(GtkAllocation &ctn_alloc)
{
    // Calculate the real positions according to the widget layout.
    int x1 =
        ctn_alloc.x +
        ctn_alloc.width *
        m_layout.h.left.factor +
        m_layout.h.left.offset;
    int x2 =
        ctn_alloc.x +
        ctn_alloc.width *
        m_layout.h.right.factor +
        m_layout.h.right.offset;
    int y1 =
        ctn_alloc.y +
        ctn_alloc.height *
        m_layout.v.top.factor +
        m_layout.v.top.offset;
    int y2 =
        ctn_alloc.y +
        ctn_alloc.height *
        m_layout.v.bottom.factor +
        m_layout.v.bottom.offset;

    // Submit the new size and position.
    GtkAllocation rect = { x1, y1, x2 - x1, y2 - y1 };
    gtk_widget_size_allocate(m_handle, &rect);
}

// declared in gui.hh
widget::widget(sys_handle &&handle, container &parent, layout layout) try :
    m_handle(handle),
    m_parent(parent),
    m_layout(layout)
{
    // Add the underlying GTK widget to the parent container.
    gtk_container_add(parent.get_container_handle(), m_handle);

    // Apply the layout to the widget now if the container is already realized.
    // If not, then the container will call this later on once it does become
    // realized.
    GtkWidget *ctn_handle = GTK_WIDGET(parent.get_container_handle());
    if (gtk_widget_get_realized(ctn_handle)) {
        GtkAllocation ctn_alloc;
        gtk_widget_get_allocation(ctn_handle, &ctn_alloc);
        apply_layout(ctn_alloc);
    }

    parent.m_widgets.insert(this);
} catch (...) {
    // Destroy the handle (GTK widget) given to us if an exception is thrown.
    gtk_widget_destroy(handle);
    throw;
}

// declared in gui.hh
widget::~widget()
{
    m_parent.m_widgets.erase(this);
    gtk_widget_destroy(m_handle);
}

// declared in gui.hh
void widget::show()
{
    gtk_widget_show(m_handle);
}

// declared in gui.hh
void widget::hide()
{
    gtk_widget_hide(m_handle);
}

}
}
