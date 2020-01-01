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
#include "../imgui/imgui.h"
#include "edit.hh"
#include "res.hh"

namespace drnsf {
namespace edit {

main_window::main_window(context &ctx) :
    window(APP_TITLE, 1024, 768),
    m_ctx(ctx)
{
    m_mode_widget.show();
}

void main_window::on_close_request()
{
    gui::end();
}

}
}
