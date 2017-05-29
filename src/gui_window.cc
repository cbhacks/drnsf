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
window::window(const std::string &title,int width,int height)
{
    M = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(M),title.c_str());
    gtk_window_set_default_size(GTK_WINDOW(M),width,height);
    g_signal_connect(M,"delete-event",G_CALLBACK(gtk_true),nullptr);

    m_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_container_add(GTK_CONTAINER(M),m_vbox);
    gtk_widget_show(m_vbox);

    m_menubar = gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(m_vbox),m_menubar,false,false,0);
    gtk_widget_show(m_menubar);

    m_content = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_box_set_homogeneous(GTK_BOX(m_content),true);
    gtk_box_pack_start(GTK_BOX(m_vbox),m_content,true,true,0);
    gtk_widget_show(m_content);
}

// declared in gui.hh
window::~window()
{
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
