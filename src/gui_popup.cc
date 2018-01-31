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
std::unordered_map<sys_handle, popup *> popup::s_all_popups;

// declared in gui.hh
popup::popup(int width, int height) :
    m_width(width),
    m_height(height)
{
    m_handle = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_window_set_default_size(GTK_WINDOW(m_handle), width, height);
    g_signal_connect(m_handle, "delete-event", G_CALLBACK(gtk_true), nullptr);

    try {
        s_all_popups.insert({m_handle, this});
    } catch (...) {
        gtk_widget_destroy(GTK_WIDGET(m_handle));
    }
}

// declared in gui.hh
popup::~popup()
{
    s_all_popups.erase(m_handle);
    gtk_widget_destroy(GTK_WIDGET(m_handle));
}

// declared in gui.hh
void popup::show_at(int x, int y)
{
    gtk_window_move(GTK_WINDOW(m_handle), x, y);
    gtk_widget_show(GTK_WIDGET(m_handle));
}

// declared in gui.hh
void popup::show_at_mouse()
{
    GdkScreen *screen = gdk_screen_get_default();
    GdkDisplay *display = gdk_screen_get_display(screen);
    int x, y;
    gdk_display_get_pointer(display, NULL, &x, &y, NULL);
    show_at(x, y);
}

// declared in gui.hh
void popup::hide()
{
    gtk_widget_hide(GTK_WIDGET(m_handle));
}

// declared in gui.hh
void popup::set_size(int width, int height)
{
    gtk_window_resize(GTK_WINDOW(m_handle), width, height);
    m_width = width;
    m_height = height;
    // FIXME these should be set in an event handler instead
}

// declared in gui.hh
sys_handle popup::get_container_handle()
{
    return m_handle;
}

// declared in gui.hh
void popup::get_container_size(int &ctn_w, int &ctn_h)
{
    ctn_w = m_width;
    ctn_h = m_height;
}

}
}
