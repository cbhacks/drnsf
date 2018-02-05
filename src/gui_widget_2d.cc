//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017  DRNSF contributors
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
widget_2d::widget_2d(container &parent, layout layout) :
    widget(gtk_drawing_area_new(), parent)
{
    gtk_widget_set_can_focus(GTK_WIDGET(m_handle), true);
    auto sigh_draw =
        static_cast<gboolean (*)(GtkWidget *, cairo_t *, gpointer)>(
            [](GtkWidget *widget, cairo_t *cr, gpointer user_data) -> gboolean {
                auto self = static_cast<widget_2d *>(user_data);

                auto width = gtk_widget_get_allocated_width(widget);
                auto height = gtk_widget_get_allocated_height(widget);

                self->draw_2d(width, height, cr);

                return true;
            });
    g_signal_connect(m_handle, "draw", G_CALLBACK(sigh_draw), this);

    set_layout(layout);
}

// declared in gui.hh
void widget_2d::invalidate()
{
    gtk_widget_queue_draw_area(
        GTK_WIDGET(m_handle),
        0,
        0,
        gtk_widget_get_allocated_width(GTK_WIDGET(m_handle)),
        gtk_widget_get_allocated_height(GTK_WIDGET(m_handle))
    );
}

}
}
