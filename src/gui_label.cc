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
GtkWidget *label::get_handle()
{
    return M;
}

// declared in gui.hh
label::label(container &parent,const std::string &text)
{
    M = gtk_label_new(text.c_str());
    gtk_container_add(parent.get_container_handle(),M);
}

// declared in gui.hh
label::~label()
{
    gtk_widget_destroy(M);
}

// declared in gui.hh
void label::set_text(const std::string &text)
{
    gtk_label_set_text(GTK_LABEL(M),text.c_str());
}

}
}
