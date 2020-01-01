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
#include "util.hh"

namespace drnsf {
namespace util {

// declared in util.hh
stopwatch::stopwatch()
{
    m_last_time = get_time();
}

// declared in util.hh
long stopwatch::lap()
{
    long current_time = get_time();
    long delta_time = current_time - m_last_time;
    m_last_time = current_time;

    if (delta_time < 0) {
        return 0;
    } else {
        return delta_time;
    }
}

}
}
