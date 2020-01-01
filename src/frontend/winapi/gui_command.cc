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
#define DRNSF_FRONTEND_IMPLEMENTATION
#include "gui.hh"
#include <vector>
#include <unordered_set>

namespace drnsf {
namespace gui {

// (s-var) s_active_ids
// A map of the ID's currently in use and their assigned command objects.
static std::unordered_map<int, command *> s_active_ids;

// (s-var) s_free_ids
// A pool of available command ID's for reuse.
static std::vector<int> s_free_ids;

// (s-var) s_next_id
// The next ID to allocate if no free ID's remain in the reuse pool.
static int s_next_id = 0xDEAD;

// declared in gui.hh
command::command()
{
    if (!s_free_ids.empty()) {
        m_id = s_free_ids.back();
        s_active_ids.insert({m_id, this});
        s_free_ids.pop_back();
    } else {
        m_id = s_next_id;
        s_active_ids.insert({m_id, this});
        s_next_id++;
    }
}

// declared in gui.hh
command::~command()
{
    try {
        s_free_ids.push_back(m_id);
    } catch (std::bad_alloc &) {
        // TODO - log error
    }
}

// declared in gui.hh
void command::dispatch(int id)
{
    auto it = s_active_ids.find(id);
    if (it != s_active_ids.end()) {
        it->second->on_command();
    } else {
        // TODO - log rogue dispatch
    }
}

}
}
