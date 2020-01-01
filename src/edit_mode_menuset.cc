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

#include "edit_mode_classic.hh"
#include "edit_mode_assets.hh"
#include "edit_mode_map.hh"

namespace drnsf {
namespace edit {

namespace {

// (internal class) mode_menuitem
// FIXME explain
template <typename T>
class mode_menuitem : private gui::menu::item {
private:
    // (var) m_wdg
    // A reference to the mode_widget this menu item interacts with.
    mode_widget &m_wdg;

public:
    // (explicit ctor)
    // FIXME explain
    explicit mode_menuitem(
        gui::menu &menu,
        mode_widget &wdg,
        std::string title) :
        item(menu, "Mode: $"_fmt(title)),
        m_wdg(wdg)
    {
        // TODO - put a checkmark on the menuitem if this is the current mode
        // TODO - hook an event on the widget to update the checkmark later
    }

    // (func) on_activate
    // Called when the menu item is clicked. This changes the widget to use the
    // specified mode. If that mode is already current, then the widget is set
    // to use no mode at all.
    void on_activate() override
    {
        if (m_wdg.template is_mode<T>()) {
            m_wdg.unset_mode();
        } else {
            m_wdg.template set_mode<T>();
        }
    }
};

}

// declared in edit.hh
struct mode_menuset::impl {
    mode_menuitem<mode_classic::handler> classic;
    mode_menuitem<mode_assets::handler> assets;
    mode_menuitem<mode_map::handler> map;

    impl(gui::menu &menu, mode_widget &wdg) :
        classic(menu, wdg, "Classic"),
        assets(menu, wdg, "Assets"),
        map(menu, wdg, "Map") {}
};

// declared in edit.hh
mode_menuset::mode_menuset(gui::menu &menu, mode_widget &wdg)
{
    M = new impl(menu, wdg);
}

// declared in edit.hh
mode_menuset::~mode_menuset()
{
    delete M;
}

}
}
