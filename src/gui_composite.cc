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
void composite::on_resize(int width, int height)
{
    GtkAllocation alloc;
    gtk_widget_get_allocation(m_handle, &alloc);
    apply_layouts(alloc.x, alloc.y, width, height);
}

// declared in gui.hh
composite::composite(container &parent, layout layout) :
    widget(gtk_fixed_new(), parent, layout)
{
}

// declared in gui.hh
sys_container_handle composite::get_container_handle()
{
    return GTK_CONTAINER(m_handle);
}

}
}
