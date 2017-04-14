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

tabview::tabview(container &parent)
{
	M = gtk_notebook_new();
	gtk_container_add(parent.get_container_handle(),M);
}

tabview::~tabview()
{
	gtk_widget_destroy(M);
}

void tabview::show()
{
	gtk_widget_show(M);
}

tabview::page::page(tabview &view,const std::string &title) :
	m_view(view)
{
	M = gtk_vbox_new(true,0);
	gtk_widget_show(M);
	auto label = gtk_label_new(title.c_str());
	gtk_widget_show(label);
	m_pagenum = gtk_notebook_append_page(
		GTK_NOTEBOOK(view.M),
		M,
		label
	);
}

tabview::page::~page()
{
	gtk_notebook_remove_page(GTK_NOTEBOOK(m_view.M),m_pagenum);
}

GtkContainer *tabview::page::get_container_handle()
{
	return GTK_CONTAINER(M);
}

}
}
