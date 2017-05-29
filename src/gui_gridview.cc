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
gridview::gridview(container &parent,int cols,int rows,bool absolute) :
    widget(gtk_grid_new(),parent)
{
    if (absolute) {
        gtk_grid_set_column_homogeneous(GTK_GRID(m_handle),true);
        gtk_grid_set_row_homogeneous(GTK_GRID(m_handle),true);
    }

    // Add the (empty) rows and columns.
    for (int i = 0; i < cols; i++) {
        gtk_grid_insert_column(GTK_GRID(m_handle),0);
    }
    for (int i = 0; i < rows; i++) {
        gtk_grid_insert_row(GTK_GRID(m_handle),0);
    }
}

// declared in gui.hh
container &gridview::make_slot(int col,int row,int col_count,int row_count)
{
    // TODO some kind of error checking

    m_slots.emplace_front();
    GtkWidget *&box = m_slots.front().m_box;

    box = gtk_box_new(GTK_ORIENTATION_VERTICAL,0);
    gtk_box_set_homogeneous(GTK_BOX(box),true);
    gtk_widget_show(box);
    gtk_grid_attach(GTK_GRID(m_handle),box,col,row,col_count,row_count);

    return m_slots.front();
}

}
}
