//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2020  DRNSF contributors
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
treeview::node::node(
    treeview &parent,
    std::function<bool(const node *)> predicate) :
    m_view(parent),
    m_parent(nullptr)
{
    if (predicate) {
        auto it = std::find_if_not(
            parent.m_nodes.begin(),
            parent.m_nodes.end(),
            predicate
        );
        parent.m_nodes.insert(it, this);
    } else {
        parent.m_nodes.push_back(this);
    }
}

// declared in gui.hh
treeview::node::node(
    node &parent,
    std::function<bool(const node *)> predicate) :
    nocopy(),
    m_view(parent.m_view),
    m_parent(&parent)
{
    if (predicate) {
        auto it = std::find_if_not(
            parent.m_subnodes.begin(),
            parent.m_subnodes.end(),
            predicate
        );
        parent.m_subnodes.insert(it, this);
    } else {
        parent.m_subnodes.push_back(this);
    }
}

// declared in gui.hh
treeview::node::~node()
{
    if (m_view.m_selected_node == this) {
        on_deselect();
        m_view.m_selected_node = nullptr;
    }

    // Remove the node pointer from the list of itself and its siblings. If
    // this is a top-level node, the list is held in the treeview. For child
    // nodes, the list is held in the parent node.
    //
    // The list is searched backwards to optimize for the case where nodes with
    // a shorter lifetime are placed at the end. This occurs especially for
    // nodes which represent indices of an array, where later nodes will never
    // outlive earlier ones.
    auto &nodes = m_parent ? m_parent->m_subnodes : m_view.m_nodes;
    auto it = std::find(nodes.rbegin(), nodes.rend(), this);
    nodes.erase(it.base() - 1);
}

// declared in gui.hh
const std::string &treeview::node::get_text() const
{
    return m_text;
}

// declared in gui.hh
void treeview::node::set_text(const std::string &text)
{
    m_text = text;
}

// declared in gui.hh
bool treeview::node::is_selected() const
{
    return m_view.m_selected_node == this;
}

}
}
