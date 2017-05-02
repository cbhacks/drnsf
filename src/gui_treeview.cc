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
void treeview::sigh_changed(GtkTreeSelection *treeselection,gpointer user_data)
{
	auto self = static_cast<treeview *>(user_data);

	GtkTreeIter iter;
	bool selection_exists = gtk_tree_selection_get_selected(
		treeselection,
		nullptr,
		&iter
	);

	if (selection_exists) {
		GValue value = G_VALUE_INIT;
		gtk_tree_model_get_value(
			GTK_TREE_MODEL(self->m_store),
			&iter,
			1,
			&value
		);
		auto np = static_cast<node *>(g_value_get_pointer(&value));
		g_value_unset(&value);

		if (np != self->m_selected_node) {
			if (self->m_selected_node) {
				self->m_selected_node->on_deselect();
			}
			self->m_selected_node = np;
			np->on_select();
		}
	} else if (self->m_selected_node) {
		self->m_selected_node->on_deselect();
		self->m_selected_node = nullptr;
	}
}

// declared in gui.hh
treeview::treeview(container &parent)
{
	m_store = gtk_tree_store_new(2,G_TYPE_STRING,G_TYPE_POINTER);
	m_tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(m_store));
	GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
	GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
		"Name",
		renderer,
		"text",
		0,
		nullptr
	);
	gtk_tree_view_append_column(GTK_TREE_VIEW(m_tree),column);
	m_scroll = gtk_scrolled_window_new(nullptr,nullptr);
	gtk_container_add(parent.get_container_handle(),m_scroll);
	gtk_container_add(GTK_CONTAINER(m_scroll),m_tree);

	auto tvs = gtk_tree_view_get_selection(GTK_TREE_VIEW(m_tree));
	g_signal_connect(tvs,"changed",G_CALLBACK(sigh_changed),this);

	gtk_widget_show(m_tree);
}

// declared in gui.hh
treeview::~treeview()
{
	gtk_widget_destroy(m_scroll);
	// FIXME - destroy tree store?
}

// declared in gui.hh
void treeview::show()
{
	gtk_widget_show(m_scroll);
}

// declared in gui.hh
treeview::node::node(treeview &parent) :
	m_view(parent),
	m_store(parent.m_store)
{
	gtk_tree_store_insert(m_store,&m_iter,nullptr,-1);
	gtk_tree_store_set(m_store,&m_iter,1,this,-1);
}

// declared in gui.hh
treeview::node::node(node &parent) :
	nocopy(),
	m_view(parent.m_view),
	m_store(parent.m_store)
{
	gtk_tree_store_insert(m_store,&m_iter,&parent.m_iter,-1);
	gtk_tree_store_set(m_store,&m_iter,1,this,-1);
}

// declared in gui.hh
treeview::node::~node()
{
	if (m_view.m_selected_node == this) {
		on_deselect();
		m_view.m_selected_node = nullptr;
	}
	gtk_tree_store_remove(m_store,&m_iter);
}

// declared in gui.hh
void treeview::node::set_text(const std::string &text)
{
	gtk_tree_store_set(m_store,&m_iter,0,text.c_str(),-1);
}

}
}
