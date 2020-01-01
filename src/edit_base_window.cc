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
void base_window::on_close_request()
{
    if (m_owned_by_context) {
        close();
    }
}

// declared in edit.hh
base_window::base_window(
    const std::string &title,
    int width,
    int height,
    context &ctx) :
    window(title, width, height),
    m_ctx(ctx)
{
    ctx.m_windows.push_back(this);
}

// declared in edit.hh
base_window::~base_window()
{
    m_ctx.m_windows.erase(std::find(
        m_ctx.m_windows.begin(),
        m_ctx.m_windows.end(),
        this
    ));
}

// declared in edit.hh
void base_window::close()
{
    if (!m_owned_by_context) {
        throw std::runtime_error("edit::base_window::close: not owned by ctx");
    }

    delete this;
}

}
}
