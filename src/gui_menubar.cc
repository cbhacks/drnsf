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

const int item_width = 64;
const int item_font_size = 12;

// declared in gui.hh
void menubar::draw_2d(int width, int height, cairo_t *cr)
{
    int x = 0;
    cairo_select_font_face(
        cr,
        "Monospace",
        CAIRO_FONT_SLANT_NORMAL,
        CAIRO_FONT_WEIGHT_NORMAL
    );
    cairo_set_font_size(cr, item_font_size);
    for (auto item : m_items) {
        /*if (!item->m_enabled) {
            cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
        } else */if (item == m_hover_item) {
            cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
            cairo_rectangle(cr, x, 0, item_width, height);
            cairo_fill(cr);
            cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        } else if (item == m_open_item) {
            cairo_set_source_rgb(cr, 0.4, 0.4, 0.4);
            cairo_rectangle(cr, x, 0, item_width, height);
            cairo_fill(cr);
            cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
        } else {
            cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
        }
        cairo_text_extents_t area;
        cairo_text_extents(cr, item->m_text.c_str(), &area);
        cairo_move_to(
            cr,
            x + item_width / 2 - area.width / 2,
            height - area.height / 2
        );
        cairo_show_text(cr, item->m_text.c_str());
        x += item_width;
    }
}

// declared in gui.hh
void menubar::mousemove(int x, int y)
{
    item *selection;
    if (x < 0) {
        selection = nullptr;
    } else {
        int i = x / item_width;
        if (i < 0 || i >= static_cast<int>(m_items.size())) {
            selection = nullptr;
        } else {
            selection = m_items[i];
        }
    }
    if (selection != m_hover_item) {
        m_hover_item = selection;
        if (selection && m_open_item) {
            m_open_item->hide();
            m_open_item = selection;
            int scr_x, scr_y;
            get_screen_pos(scr_x, scr_y);
            auto it = find(m_items.begin(), m_items.end(), m_open_item);
            m_open_item->show_at(
                scr_x + item_width * (it - m_items.begin()),
                scr_y + 20
            );
        }
        invalidate();
    }
}

// declared in gui.hh
void menubar::mouseleave()
{
    if (m_hover_item) {
        m_hover_item = nullptr;
        invalidate();
    }
}

// declared in gui.hh
void menubar::mousebutton(mousebtn btn, bool down)
{
    if (btn != mousebtn::left || !down) return;
    if (m_hover_item && !m_open_item) {
        m_open_item = m_hover_item;
        int scr_x, scr_y;
        get_screen_pos(scr_x, scr_y);
        auto it = find(m_items.begin(), m_items.end(), m_open_item);
        m_open_item->show_at(
            scr_x + item_width * (it - m_items.begin()),
            scr_y + 20
        );
    } else if (m_open_item) {
        m_open_item->hide();
        m_open_item = nullptr;
    }
}

// declared in gui.hh
menubar::menubar(window &wnd) :
    widget_2d(wnd, {
        {{ 0.0f, 0 }, { 1.0f, 0 }},
        {{ 0.0f, -20 }, { 0.0f, 0 }}
    }),
    m_wnd(wnd)
{
    if (m_wnd.m_menubar) {
        throw std::logic_error("gui::menubar: window already has menubar");
    }
    m_wnd.m_menubar = this;
    widget_2d::show();
}

// declared in gui.hh
menubar::~menubar()
{
    m_wnd.m_menubar = nullptr;
}

// declared in gui.h
void menubar::item::close()
{
    if (m_menubar.m_open_item == this) {
        m_menubar.m_open_item = nullptr;
        m_menubar.invalidate();
        hide();
    }
}

// declared in gui.hh
menubar::item::item(menubar &menubar, std::string text) :
    m_menubar(menubar),
    m_text(std::move(text))
{
    m_menubar.m_items.push_back(this);
    m_menubar.invalidate();
}

// declared in gui.hh
menubar::item::~item()
{
    m_menubar.m_items.erase(
        std::find(m_menubar.m_items.begin(), m_menubar.m_items.end(), this)
    );
    if (m_menubar.m_hover_item == this) {
        m_menubar.m_hover_item = nullptr;
    }
    if (m_menubar.m_open_item == this) {
        m_menubar.m_open_item = nullptr;
    }
    m_menubar.invalidate();
}

}
}

#endif
