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
composite::composite(container &parent) :
    widget(gtk_box_new(GTK_ORIENTATION_VERTICAL,0))
{
    gtk_container_add(parent.get_container_handle(),m_handle);
    gtk_box_set_homogeneous(GTK_BOX(m_handle),true);
}

// declared in gui.hh
GtkContainer *composite::get_container_handle()
{
    return GTK_CONTAINER(m_handle);
}

}
}
