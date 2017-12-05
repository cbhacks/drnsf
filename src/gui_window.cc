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
window::window(const std::string &title, int width, int height)
{
    M = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(M), title.c_str());
    gtk_window_set_default_size(GTK_WINDOW(M), width, height);
    g_signal_connect(M, "delete-event", G_CALLBACK(gtk_true), nullptr);

    m_content = gtk_fixed_new();
    auto sigh_size_allocate =
        static_cast<void (*)(GtkWidget *, GdkRectangle *, gpointer)>(
            [](GtkWidget *, GdkRectangle *allocation, gpointer user_data) {
                auto self = static_cast<window *>(user_data);
                auto alloc = *allocation;
                if (self->m_menubar) {
                    alloc.height -= 20;
                    alloc.y += 20;
                }
                self->apply_layouts(
                    alloc.x,
                    alloc.y,
                    alloc.width,
                    alloc.height
                );
                if (self->m_menubar) {
                    self->m_menubar->apply_layout(
                        alloc.x,
                        alloc.y - 20,
                        alloc.width,
                        20
                    );
                }
            });
    g_signal_connect(
        m_content,
        "size-allocate",
        G_CALLBACK(sigh_size_allocate),
        this
    );
    gtk_container_add(GTK_CONTAINER(M), m_content);
    gtk_widget_show(m_content);

    try {
        g_windows.insert({M, this});
    } catch (...) {
        gtk_widget_destroy(M);
        throw;
    }
}

// declared in gui.hh
window::~window()
{
    g_windows.erase(M);
    gtk_widget_destroy(M);
}

// declared in gui.hh
void window::show()
{
    gtk_widget_show(M);
}

// declared in gui.hh
sys_container_handle window::get_container_handle()
{
    return GTK_CONTAINER(m_content);
}

}
}
