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
void label::draw_2d(int width, int height, cairo_t *cr)
{
    cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
    cairo_select_font_face(
        cr,
        "Monospace",
        CAIRO_FONT_SLANT_NORMAL,
        CAIRO_FONT_WEIGHT_NORMAL
    );
    cairo_set_font_size(cr, 12);

    auto str = m_text.c_str();

    cairo_text_extents_t area;
    cairo_text_extents(cr, str, &area);
    cairo_move_to(
        cr,
        width / 2 - area.width / 2,
        height / 2
    );
    cairo_show_text(cr, str);
}

// declared in gui.hh
label::label(container &parent, layout layout, const std::string &text) :
    widget_2d(parent, layout),
    m_text(text)
{
}

// declared in gui.hh
void label::set_text(const std::string &text)
{
    m_text = text;
    invalidate();
}

}
}
