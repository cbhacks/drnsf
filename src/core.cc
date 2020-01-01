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
#include "core.hh"
#include <thread>

namespace drnsf {
namespace core {

// (s-var) s_abandon
// True if `core::update' should be abandoned, false otherwise.
bool s_abandon;

// declared in core.hh
bool is_main_thread() noexcept
{
    static std::thread::id main_thread_id;
    static std::once_flag flag;
    std::call_once(flag, [] {
        main_thread_id = std::this_thread::get_id();
    });
    return std::this_thread::get_id() == main_thread_id;
}

// declared in core.hh
worker *worker::s_head = nullptr;
worker *worker::s_tail = nullptr;

// declared in core.hh
worker::worker() noexcept
{
    if (s_tail) {
        m_prev = s_tail;
        m_prev->m_next = this;
        m_next = nullptr;
        s_tail = this;
    } else {
        m_prev = nullptr;
        s_head = this;
        m_next = nullptr;
        s_tail = this;
    }
    s_abandon = true;
}

// declared in core.hh
worker::~worker() noexcept
{
    if (m_next) {
        m_next->m_prev = m_prev;
    } else {
        s_tail = m_prev;
    }
    if (m_prev) {
        m_prev->m_next = m_next;
    } else {
        s_head = m_next;
    }
    s_abandon = true;
}

// declared in core.hh
int update() noexcept
{
    if (!is_main_thread()) {
        // TODO - log error
        return INT_MAX;
    }

    s_abandon = false;
    DRNSF_ON_EXIT { s_abandon = true; };

    // Run each of the available workers.
    int min_time = INT_MAX;
    for (auto it = worker::s_head; it; it = it->m_next) {
        int time = it->work();

        // If s_abandon was set during the `work' call (i.e. from a worker being
        // created or destroyed, or by a recursive update call), abandon this
        // update and return a 0ms timeout.
        if (s_abandon) {
            return 0;
        }

        if (time < min_time) {
            min_time = time;
        }
    }
    return min_time;
}

}
}
