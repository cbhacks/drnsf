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

#if USE_GTK3
#include <gtk/gtk.h>
#endif

namespace drnsf {
namespace gui {

// declared in gui.hh
void init(int &argc, char **&argv)
{
    gtk_init(&argc, &argv);
}

// declared in gui.hh
void run()
{
    g_idle_add([](gpointer) -> gboolean {
        static long last_update = LONG_MAX;
        long current_time = g_get_monotonic_time() / 1000;
        if (current_time > last_update) {
            int delta_time = current_time - last_update;
            int min_delay = INT_MAX;
            for (auto &&w : widget::s_all_widgets) {
                int delay = w->update(delta_time);
                if (delay < min_delay) {
                    min_delay = delay;
                }
            }
        }
        last_update = current_time;
        return G_SOURCE_CONTINUE;
    }, nullptr);
    gtk_main();
}

}
}
