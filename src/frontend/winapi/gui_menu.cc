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
#if USE_NATIVE_MENU
#define DRNSF_FRONTEND_IMPLEMENTATION
#include "gui.hh"

namespace drnsf {
namespace gui {

// declared in gui.hh
menu::menu()
{
    m_handle = CreateMenu();
}

// declared in gui.hh
menu::~menu()
{
    DestroyMenu(HMENU(m_handle));
}

// declared in gui.hh
void menu::item::on_command()
{
    on_activate();
}

// declared in gui.hh
menu::item::item(menu &menu, std::string text) :
    m_menu(menu),
    m_text(std::move(text))
{
    auto wtext = util::u8str_to_wstr(m_text);
    InsertMenuW(
        HMENU(m_menu.m_handle),
        -1,
        MF_BYPOSITION,
        get_command_id(),
        wtext.c_str()
    );
}

// declared in gui.hh
menu::item::~item()
{
    DeleteMenu(
        HMENU(m_menu.m_handle),
        get_command_id(),
        MF_BYCOMMAND
    );
}

// declared in gui.hh
void menu::item::set_text(std::string text)
{
    if (m_text != text) {
        auto wtext = util::u8str_to_wstr(text);
        m_text = std::move(text);

        MENUITEMINFOW info{};
        info.cbSize = sizeof(info);
        info.fMask = MIIM_STRING;
        info.dwTypeData = const_cast<LPWSTR>(wtext.c_str());
        SetMenuItemInfoW(
            HMENU(m_menu.m_handle),
            get_command_id(),
            false,
            &info
        );
    }
}

// declared in gui.hh
void menu::item::set_enabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;

        MENUITEMINFOW info{};
        info.cbSize = sizeof(info);
        info.fMask = MIIM_STATE;
        info.fState = enabled ? MFS_ENABLED : MFS_DISABLED;
        SetMenuItemInfoW(
            HMENU(m_menu.m_handle),
            get_command_id(),
            false,
            &info
        );
    }
}

}
}

#endif
