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
void menu_item::sigh_activate(GtkMenuItem *menuitem,gpointer user_data)
{
	static_cast<menu_item *>(user_data)->on_click();
}

// declared in gui.hh
menu_item::menu_item(menu &parent,const std::string &text)
{
	M = gtk_menu_item_new_with_label(text.c_str());
	g_signal_connect(M,"activate",G_CALLBACK(sigh_activate),this);
	gtk_menu_shell_append(GTK_MENU_SHELL(parent.m_menu),M);
	gtk_widget_show(M);
}

// declared in gui.hh
menu_item::~menu_item()
{
	gtk_widget_destroy(M);
}

}
}
