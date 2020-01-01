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
#if !USE_NATIVE_MENU
#include <algorithm>
#include "gui.hh"

namespace drnsf {
namespace gui {

const int item_width = 192;
const int item_height = 20;
const int item_pad_outer = 4;
const int item_pad_inner = 4;
const int item_font_size = 12;

// declared in gui.hh
void menu::draw_2d(int width, int height, cairo_t *cr)
{
    int x = item_pad_outer;
    int y = item_pad_outer;
    cairo_select_font_face(
        cr,
        "Monospace",
        CAIRO_FONT_SLANT_NORMAL,
        CAIRO_FONT_WEIGHT_NORMAL
    );
    cairo_set_font_size(cr, item_font_size);
    for (auto item : m_items) {
        if (!item->m_enabled) {
            cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
        } else if (item == m_active_item) {
            cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
            cairo_rectangle(cr, x, y, item_width, item_height);
            cairo_fill(cr);
            cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        } else {
            cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
        }
        cairo_text_extents_t area;
        cairo_text_extents(cr, item->m_text.c_str(), &area);
        cairo_move_to(
            cr,
            x + 8,
            y + item_height - area.height / 2
        );
        cairo_show_text(cr, item->m_text.c_str());
        y += item_pad_inner + item_height;
    }
}

// declared in gui.hh
void menu::mousemove(int x, int y)
{
    item *selection;
    if (
        x < item_pad_outer ||
        x >= item_pad_outer + item_width ||
        y < item_pad_outer) {
        selection = nullptr;
    } else {
        // note - Division will round up to zero if y is less than
        // item_pad_outer, so keep the (y < item_pad_outer) check present
        // above.
        int i = (y - item_pad_outer) / (item_pad_inner + item_height);
        int o = (y - item_pad_outer) % (item_pad_inner + item_height);
        if (i < 0 || i >= static_cast<int>(m_items.size())) {
            selection = nullptr;
        } else if (o >= item_height) {
            selection = nullptr;
        } else {
            selection = m_items[i];
        }
    }
    if (selection != m_active_item) {
        m_active_item = selection;
        invalidate();
    }
}

// declared in gui.hh
void menu::mouseleave()
{
    if (m_active_item) {
        m_active_item = nullptr;
        invalidate();
    }
}

// declared in gui.hh
void menu::mousebutton(mousebtn btn, bool down)
{
    if (btn != mousebtn::left || !down) return;
    if (m_active_item) {
        close();
        m_active_item->on_activate();
    }
}

// declared in gui.hh
menu::menu() :
    popup(item_width + item_pad_outer * 2, item_pad_outer * 2),
    widget_2d(*this, layout::fill())
{
    widget_2d::show();
}

// declared in gui.hh
menu::item::item(menu &menu, std::string text) :
    m_menu(menu),
    m_text(std::move(text))
{
    m_menu.m_items.push_back(this);
    m_menu.invalidate();
    m_menu.set_size(
        item_pad_outer * 2 + item_width,
        item_pad_outer * 2 +
            m_menu.m_items.size() * (item_height + item_pad_inner) -
            item_pad_inner
    );
}

// declared in gui.hh
menu::item::~item()
{
    m_menu.m_items.erase(
        std::find(m_menu.m_items.begin(), m_menu.m_items.end(), this)
    );
    if (m_menu.m_active_item == this) {
        m_menu.m_active_item = nullptr;
    }
    m_menu.invalidate();
    m_menu.set_size(
        item_pad_outer * 2 + item_width,
        item_pad_outer * 2 +
            m_menu.m_items.size() * (item_height + item_pad_inner)
    );
}

// declared in gui.hh
void menu::item::set_text(std::string text)
{
    if (m_text != text) {
        m_text = std::move(text);
        m_menu.invalidate();
    }
}

// declared in gui.hh
void menu::item::set_enabled(bool enabled)
{
    if (m_enabled != enabled) {
        m_enabled = enabled;
        m_menu.invalidate();
    }
}

}
}

#endif
