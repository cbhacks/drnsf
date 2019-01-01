//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2019  DRNSF contributors
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
void treeview::frame()
{
    for (auto &&node : m_nodes) {
        node->run();
    }
}

// declared in gui.hh
treeview::treeview(container &parent, layout layout) :
    widget_im(parent, layout)
{
}

// declared in gui.hh
void treeview::node::run()
{
    ImGui::PushID(this);

    // Display the "+" or "-" button if this node has children.
    if (!m_subnodes.empty()) {
        if (ImGui::SmallButton(m_expanded ? "-" : "+")) {
            m_expanded = !m_expanded;
        }
    } else {
        ImGui::Dummy({14, 0});
    }
    ImGui::SameLine();

    // Display the actual selectable text for this node. If the node is not
    // already selected, provide a handler for clicking which shifts the
    // selection to this node.
    if (m_view.m_selected_node == this) {
        ImGui::Selectable(m_text.c_str(), true);
    } else if (ImGui::Selectable(m_text.c_str(), false)) {
        if (m_view.m_selected_node) {
            m_view.m_selected_node->on_deselect();
        }
        m_view.m_selected_node = this;
        on_select();
    }

    // Display the child nodes if the node is expanded.
    if (m_expanded) {
        ImGui::Indent(16);
        for (auto &&subnode : m_subnodes) {
            subnode->run();
        }
        ImGui::Unindent(16);
    }

    ImGui::PopID();
}

// declared in gui.hh
treeview::node::node(treeview &parent) :
    m_view(parent),
    m_parent(nullptr)
{
    parent.m_nodes.push_front(this);
    m_iter = parent.m_nodes.begin();
}

// declared in gui.hh
treeview::node::node(node &parent) :
    nocopy(),
    m_view(parent.m_view),
    m_parent(&parent)
{
    parent.m_subnodes.push_front(this);
    m_iter = parent.m_subnodes.begin();
}

// declared in gui.hh
treeview::node::~node()
{
    if (m_view.m_selected_node == this) {
        on_deselect();
        m_view.m_selected_node = nullptr;
    }
    if (m_parent) {
        m_parent->m_subnodes.erase(m_iter);
    } else {
        m_view.m_nodes.erase(m_iter);
    }
}

// declared in gui.hh
void treeview::node::set_text(const std::string &text)
{
    m_text = text;
}

}
}
