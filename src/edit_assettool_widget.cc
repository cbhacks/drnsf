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
#include "edit.hh"
#include "edit_assettool_new.hh"
#include "edit_assettool_main.hh"
#include "edit_assettool_3d.hh"
#include "edit_assettool_props.hh"

namespace drnsf {
namespace edit {

// declared in edit.hh
template <typename T>
void assettool_widget::add_tool()
{
    auto tool_tab = std::make_unique<gui::tabview::page>(m_tabview);
    auto tool = std::unique_ptr<assettool>(
        new T(*tool_tab, gui::layout::fill(), m_ctx)
    );

    tool->on_availability_change <<= [&tab = *tool_tab](bool available) {
        tab.set_visible(available);
    };

    tool->show();
    tool->set_name(m_name);
    tool_tab->set_text(tool->get_title());

    m_tools.push_back(std::move(tool));
    try {
        m_tool_tabs.push_back(std::move(tool_tab));
    } catch (...) {
        m_tools.pop_back();
        throw;
    }
}

// declared in edit.hh
assettool_widget::assettool_widget(
    gui::container &parent,
    gui::layout layout,
    context &ctx) :
    composite(parent, layout),
    m_ctx(ctx),
    m_tabview(*this, gui::layout::fill())
{
    m_tabview.show();
    add_tool<assettool_new::tool>();
    add_tool<assettool_main::tool>();
    add_tool<assettool_3d::tool>();
    add_tool<assettool_props::tool>();
}

// declared in edit.hh
res::atom assettool_widget::get_name() const
{
    return m_name;
}

// declared in edit.hh
void assettool_widget::set_name(res::atom name)
{
    if (m_name == name)
        return;

    if (name && name.get_proj() != m_ctx.get_proj().get())
        throw std::logic_error("assettool_widget::set_name: project mismatch");

    for (auto &&tool : m_tools) {
        tool->set_name(name);
    }
}

}
}
