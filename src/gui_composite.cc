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

#if USE_GTK3
#include <gtk/gtk.h>
#endif

namespace drnsf {
namespace gui {

// declared in gui.hh
void composite::on_resize(int width, int height)
{
    apply_layouts();
}

// declared in gui.hh
composite::composite(container &parent, layout layout) :
    widget(parent)
{
    m_handle = gtk_fixed_new();
    gtk_container_add(
        GTK_CONTAINER(parent.get_container_handle()),
        GTK_WIDGET(m_handle)
    );

    // Register event handlers.
    auto sigh_size_allocate =
        static_cast<void (*)(GtkWidget *,GdkRectangle *, gpointer user_data)>(
            [](GtkWidget *wdg, GdkRectangle *allocation, gpointer user_data) {
                static_cast<composite *>(user_data)->on_resize(
                    allocation->width,
                    allocation->height
                );
            });
    g_signal_connect(
        m_handle,
        "size-allocate",
        G_CALLBACK(sigh_size_allocate),
        this
    );

    set_layout(layout);
}

// declared in gui.hh
sys_handle composite::get_container_handle()
{
    return m_handle;
}

// declared in gui.hh
void composite::get_child_area(int &ctn_x, int &ctn_y, int &ctn_w, int &ctn_h)
{
    ctn_x = 0;
    ctn_y = 0;
    get_real_size(ctn_w, ctn_h);
}

}
}
