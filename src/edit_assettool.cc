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

namespace drnsf {
namespace edit {

// declared in edit.hh
void assettool::set_available(bool available)
{
    if (m_available == available)
        return;

    m_available = available;
    on_availability_change(available);
}

// declared in edit.hh
assettool::assettool(
    gui::container &parent,
    gui::layout layout,
    context &ctx) :
    composite(parent, layout),
    m_ctx(ctx)
{
    h_context_project_change <<= [this](const std::shared_ptr<res::project> &) {
        set_name(nullptr);
    };
    h_context_project_change.bind(ctx.on_project_change);
}

// declared in edit.hh
res::atom assettool::get_name() const
{
    return m_name;
}

// declared in edit.hh
void assettool::set_name(res::atom name)
{
    if (m_name == name)
        return;

    if (name && name.get_proj() != m_ctx.get_proj().get())
        throw std::logic_error("assettool::set_name: project mismatch");

    m_name = name;
    on_name_change(name);
}

}
}
