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
std::unordered_map<sys_handle, window *> window::s_all_windows;

// declared in gui.hh
void window::exit_dialog()
{
    gtk_main_quit();
}

// declared in gui.hh
window::window(const std::string &title, int width, int height) :
    m_width(width),
    m_height(height)
{
    m_handle = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(m_handle), title.c_str());
    gtk_window_set_default_size(GTK_WINDOW(m_handle), width, height);
    g_signal_connect(m_handle, "delete-event", G_CALLBACK(gtk_true), nullptr);

    m_content = gtk_fixed_new();
    auto sigh_size_allocate =
        static_cast<void (*)(GtkWidget *, GdkRectangle *, gpointer)>(
            [](GtkWidget *, GdkRectangle *allocation, gpointer user_data) {
                auto self = static_cast<window *>(user_data);
                auto alloc = *allocation;
                self->m_width = alloc.width;
                self->m_height = alloc.height;
                self->apply_layouts();
            });
    g_signal_connect(
        m_content,
        "size-allocate",
        G_CALLBACK(sigh_size_allocate),
        this
    );
    gtk_container_add(GTK_CONTAINER(m_handle), GTK_WIDGET(m_content));
    gtk_widget_show(GTK_WIDGET(m_content));

    try {
        s_all_windows.insert({m_handle, this});
    } catch (...) {
        gtk_widget_destroy(GTK_WIDGET(m_handle));
        throw;
    }
}

// declared in gui.hh
window::~window()
{
    s_all_windows.erase(m_handle);
    gtk_widget_destroy(GTK_WIDGET(m_handle));
}

// declared in gui.hh
void window::show()
{
    gtk_widget_show(GTK_WIDGET(m_handle));
}

// declared in gui.hh
void window::show_dialog()
{
    gtk_window_set_modal(GTK_WINDOW(m_handle), true);
    gtk_widget_show(GTK_WIDGET(m_handle));
    gtk_main();
    gtk_widget_hide(GTK_WIDGET(m_handle));
    gtk_window_set_modal(GTK_WINDOW(m_handle), false);
}

// declared in gui.hh
sys_handle window::get_container_handle()
{
    return m_content;
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
