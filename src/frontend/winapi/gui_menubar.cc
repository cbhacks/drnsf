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
menubar::menubar(window &wnd) :
    m_wnd(wnd)
{
    // Ensure the window does not already have a menubar.
    if (m_wnd.m_menubar) {
        throw std::logic_error("gui::menubar: window already has menubar");
    }

    // Create the menubar system object.
    m_handle = CreateMenu();

    // Attach the menubar to the window.
    m_wnd.m_menubar = this;
    SetMenu(HWND(m_wnd.m_handle), HMENU(m_handle));

    // Update the system regarding the menubar change.
    DrawMenuBar(HWND(m_wnd.m_handle));
}

// declared in gui.hh
menubar::~menubar()
{
    // Detach the menubar from the window.
    m_wnd.m_menubar = nullptr;
    SetMenu(HWND(m_wnd.m_handle), HMENU(m_handle));

    // Destroy the menubar system object.
    DestroyMenu(HMENU(m_handle));

    // Update the system regarding the menubar change.
    DrawMenuBar(HWND(m_wnd.m_handle));
}

// declared in gui.hh
menubar::item::item(menubar &menubar, std::string text) :
    m_menubar(menubar),
    m_text(std::move(text))
{
    auto wtext = util::u8str_to_wstr(m_text);

    // Insert the menubar item into the menubar.
    InsertMenuW(
        HMENU(m_menubar.m_handle),
        -1,
        MF_BYPOSITION,
        get_command_id(),
        wtext.c_str()
    );

    // Attach the menubar item to its menu.
    MENUITEMINFOW info{};
    info.cbSize = sizeof(info);
    info.fMask = MIIM_SUBMENU;
    info.hSubMenu = HMENU(m_handle);
    SetMenuItemInfoW(
        HMENU(m_menubar.m_handle),
        get_command_id(),
        false,
        &info
    );

    // Update the system regarding the menubar change.
    DrawMenuBar(HWND(m_menubar.m_wnd.m_handle));
}

// declared in gui.hh
menubar::item::~item()
{
    // Detach the menubar item from its menu.
    MENUITEMINFOW info{};
    info.cbSize = sizeof(info);
    info.fMask = MIIM_SUBMENU;
    info.hSubMenu = nullptr;
    SetMenuItemInfoW(
        HMENU(m_menubar.m_handle),
        get_command_id(),
        false,
        &info
    );

    // Remove the menubar item from the menubar.
    DeleteMenu(
        HMENU(m_menubar.m_handle),
        get_command_id(),
        MF_BYCOMMAND
    );

    // Update the system regarding the menubar change.
    DrawMenuBar(HWND(m_menubar.m_wnd.m_handle));
}

}
}

#endif
