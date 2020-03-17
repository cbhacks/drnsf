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
void tabview::bar::draw_2d(int width, int height, cairo_t *cr)
{
    int i = 0;
    for (auto &&page : m_view.m_pages) {
        if (!page->get_visible())
            continue;

        int x1 = 100 * i;
        int x2 = 100 * (i + 1);

        cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);

        if (page == m_view.m_active_page) {
            cairo_move_to(cr, x1, height - 1);
            cairo_line_to(cr, x1 + 2, height - 1);
            cairo_line_to(cr, x1 + 2, 2);
            cairo_line_to(cr, x2 - 2, 2);
            cairo_line_to(cr, x2 - 2, height - 1);
            cairo_line_to(cr, x2, height - 1);
            cairo_stroke(cr);
        } else {
            cairo_rectangle(cr, x1 + 4, 6, x2 - x1 - 8, height - 12);
            cairo_stroke(cr);

            cairo_move_to(cr, x1, height - 1);
            cairo_line_to(cr, x2, height - 1);
            cairo_stroke(cr);
        }

        cairo_select_font_face(
            cr,
            "Monospace",
            CAIRO_FONT_SLANT_NORMAL,
            CAIRO_FONT_WEIGHT_NORMAL
        );
        cairo_set_font_size(cr, 12);

        auto text = page->get_text();

        cairo_text_extents_t text_area;
        cairo_text_extents(cr, text.c_str(), &text_area);
        cairo_move_to(
            cr,
            (x1 + x2) / 2 - text_area.width / 2,
            height / 2 + text_area.height / 2
        );
        cairo_show_text(cr, text.c_str());

        i++;
    }

    int tail_x = 100 * i;
    if (tail_x < width) {
        cairo_move_to(cr, tail_x, height - 1);
        cairo_line_to(cr, width, height - 1);
        cairo_stroke(cr);
    }
}

// declared in gui.hh
void tabview::bar::mousemove(int x, int y)
{
    m_mouse_x = x;
    invalidate();
}

// declared in gui.hh
void tabview::bar::mouseleave()
{
    m_mouse_x = -1;
    invalidate();
}

// declared in gui.hh
void tabview::bar::mousebutton(mousebtn btn, bool down, keymods mods)
{
    if (!down)
        return;

    if (btn != mousebtn::left)
        return;

    int i = 0;
    for (auto &&page : m_view.m_pages) {
        if (!page->get_visible())
            continue;

        int x1 = 100 * i;
        int x2 = 100 * (i + 1);

        if (m_mouse_x >= x1 && m_mouse_x < x2) {
            if (page != m_view.m_active_page) {
                m_view.m_active_page->m_composite.hide();
                m_view.m_active_page = page;
                m_view.m_active_page->m_composite.show();
                invalidate();
            }

            return;
        }

        i++;
    }
}

// declared in gui.hh
tabview::tabview(container &parent, layout layout) :
    composite(parent, layout),
    m_bar(*this, {
        {{ 0.0f, 0 }, { 1.0f, 0 }},
        {{ 0.0f, 0 }, { 0.0f, 32 }}
    })
{
    m_bar.show();
}

// declared in gui.hh
tabview::page::page(tabview &view) :
    m_view(view),
    m_composite(view, {
        {{ 0.0f, 0 }, { 1.0f, 0 }},
        {{ 0.0f, 32 }, { 1.0f, 0 }}
    })
{
    m_view.m_pages.push_back(this);

    // Invalidate the tab bar, as the tab list has changed.
    m_view.m_bar.invalidate();
}

// declared in gui.hh
tabview::page::~page()
{
    // Force the current page to become non-active, so another tab becomes
    // the active tab.
    set_visible(false);

    // Erase this tab from the tabview's list.
    m_view.m_pages.erase(std::find(
        m_view.m_pages.begin(),
        m_view.m_pages.end(),
        this
    ));

    // Invalidate the tab bar, as the tab list has changed.
    m_view.m_bar.invalidate();
}

// declared in gui.hh
bool tabview::page::get_visible() const
{
    return m_visible;
}

// declared in gui.hh
void tabview::page::set_visible(bool visible)
{
    if (visible == m_visible)
        return;

    // Set the new visibility status.
    m_visible = visible;

    // Switch the active tab to another one if this page is the current one and
    // it is becoming invisible.
    if (!m_visible && m_view.m_active_page == this) {
        m_composite.hide();

        // Find the index of this tab in the tab list.
        size_t i;
        for (i = 0; m_view.m_pages[i] != this; i++);

        // Switch to the closest previous tab which is visible.
        for (size_t j = i - 1; j != SIZE_MAX; j--) {
            if (!m_view.m_pages[j]->m_visible)
                continue;

            m_view.m_active_page = m_view.m_pages[j];
            m_view.m_active_page->m_composite.show();
            m_view.m_bar.invalidate();
            return;
        }

        // If none, switch to the closest next tab which is visible.
        for (size_t j = i + 1; j < m_view.m_pages.size(); j++) {
            if (!m_view.m_pages[j]->m_visible)
                continue;

            m_view.m_active_page = m_view.m_pages[j];
            m_view.m_active_page->m_composite.show();
            m_view.m_bar.invalidate();
            return;
        }

        // If none, there is no new active tab.
        m_view.m_active_page = nullptr;
        m_view.m_bar.invalidate();
        return;
    }

    // Switch the active tab to this one if no page is active and this one is
    // becoming visible.
    if (m_visible && !m_view.m_active_page) {
        m_composite.show();
        m_view.m_active_page = this;

        // Invalidate the tab bar, as the active tab changed.
        m_view.m_bar.invalidate();
    }
}

// declared in gui.hh
std::string tabview::page::get_text() const
{
    return m_text;
}

// declared in gui.hh
void tabview::page::set_text(std::string text)
{
    m_text = std::move(text);
    m_view.m_bar.invalidate();
}

}
}
