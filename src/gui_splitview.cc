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
splitview::splitview(container &parent) :
    widget(gtk_paned_new(GTK_ORIENTATION_HORIZONTAL))
{
    gtk_container_add(parent.get_container_handle(),m_handle);

    m_left.M = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_box_set_homogeneous(GTK_BOX(m_left.M),true);
    gtk_widget_show(m_left.M);
    gtk_paned_add1(GTK_PANED(m_handle),m_left.M);

    m_right.M = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_box_set_homogeneous(GTK_BOX(m_right.M),true);
    gtk_widget_show(m_right.M);
    gtk_paned_add2(GTK_PANED(m_handle),m_right.M);
}

// declared in gui.hh
container &splitview::get_left()
{
    return m_left;
}

// declared in gui.hh
container &splitview::get_right()
{
    return m_right;
}

// declared in gui.hh
GtkContainer *splitview::side::get_container_handle()
{
    return GTK_CONTAINER(M);
}

}
}
