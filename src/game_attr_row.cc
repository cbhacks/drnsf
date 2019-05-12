//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2019  DRNSF contributors
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
#include "game.hh"

namespace drnsf {
namespace game {

// declared in game.hh
int attr_row::id() const
{
    return m_id;
}

// declared in game.hh
size_t attr_row::value_size() const
{
    return m_value_size;
}

// declared in game.hh
bool attr_row::is_columned() const
{
    return m_columned;
}

// declared in game.hh
size_t attr_row::vgroup_count() const
{
    return m_vgroups.size();
}

// declared in game.hh
const attr_vgroup &attr_row::get_vgroup_by_index(size_t index) const
{
    if (index >= m_vgroups.size()) {
        throw std::logic_error(
            "game::attr_row::get_vgroup_by_index: out of bounds"
        );
    }
    return m_vgroups[index];
}

// declared in game.hh
size_t attr_row::find_vgroup_id(int id) const
{
    if (!m_columned) {
        throw std::logic_error(
            "game::attr_row::find_vgroup_id: row is not columned"
        );
    }
    auto it = std::lower_bound(
        m_vgroups.begin(),
        m_vgroups.end(),
        id,
        [](const attr_vgroup &lhs, auto rhs) -> int {
            return lhs.column_id() - rhs;
        }
    );
    if (it == m_vgroups.end() || it->column_id() != id) {
        return SIZE_MAX;
    } else {
        return it - m_vgroups.begin();
    }
}

// declared in game.hh
bool attr_row::has_vgroup_id(int id) const
{
    if (!m_columned) {
        throw std::logic_error(
            "game::attr_row::has_vgroup_id: row is not columned"
        );
    }
    return find_vgroup_id(id) != SIZE_MAX;
}

// declared in game.hh
const attr_vgroup &attr_row::get_vgroup_by_id(int id) const
{
    if (!m_columned) {
        throw std::logic_error(
            "game::attr_row::get_vgroup_by_id: row is not columned"
        );
    }
    auto index = find_vgroup_id(id);
    if (index == SIZE_MAX) {
        throw std::logic_error(
            "game::attr_row::get_vgroup_by_id: no such value group"
        );
    }
    return m_vgroups[index];
}

// declared in game.hh
void attr_row::put_vgroup(attr_vgroup vgroup)
{
    if (!m_columned) {
        throw std::logic_error(
            "game::attr_row::put_vgroup: row is not columned"
        );
    }
    if (vgroup.column_id() == -1) {
        throw std::logic_error(
            "game::attr_row::put_vgroup: value group has no column id"
        );
    }
    if (vgroup.value_size() != m_value_size) {
        throw std::logic_error(
            "game::attr_row::put_vgroup: value size mismatch"
        );
    }
    auto it = std::lower_bound(
        m_vgroups.begin(),
        m_vgroups.end(),
        vgroup.column_id(),
        [](const attr_vgroup &lhs, auto rhs) -> int {
            return lhs.column_id() - rhs;
        }
    );
    if (it == m_vgroups.end() || it->column_id() != vgroup.column_id()) {
        m_vgroups.insert(it, std::move(vgroup));
    } else {
        *it = std::move(vgroup);
    }
}

// declared in game.hh
void attr_row::insert_vgroup(size_t index, attr_vgroup vgroup)
{
    if (m_columned) {
        throw std::logic_error(
            "game::attr_row::insert_vgroup: row is columned"
        );
    }
    if (index > m_vgroups.size()) {
        throw std::logic_error("game::attr_row::insert_vgroup: out of bounds");
    }
    if (vgroup.column_id() != -1) {
        throw std::logic_error(
            "game::attr_row::insert_vgroup: value group has column id"
        );
    }
    if (vgroup.value_size() != m_value_size) {
        throw std::logic_error(
            "game::attr_row::insert_vgroup: value size mismatch"
        );
    }
    m_vgroups.insert(m_vgroups.begin() + index, std::move(vgroup));
}

// declared in game.hh
void attr_row::append_vgroup(attr_vgroup vgroup)
{
    if (m_columned) {
        throw std::logic_error(
            "game::attr_row::append_vgroup: row is columned"
        );
    }
    if (vgroup.column_id() != -1) {
        throw std::logic_error(
            "game::attr_row::append_vgroup: value group has column id"
        );
    }
    if (vgroup.value_size() != m_value_size) {
        throw std::logic_error(
            "game::attr_row::append_vgroup: value size mismatch"
        );
    }
    m_vgroups.push_back(std::move(vgroup));
}

// declared in game.hh
void attr_row::remove_vgroup_by_index(size_t index)
{
    if (index >= m_vgroups.size()) {
        throw std::logic_error(
            "game::attr_row::remove_vgroup_by_index: out of bounds"
        );
    }
    m_vgroups.erase(m_vgroups.begin() + index);
}

// declared in game.hh
void attr_row::remove_vgroup_by_id(int id)
{
    if (!m_columned) {
        throw std::logic_error(
            "game::attr_row::remove_vgroup_by_id: row is not columned"
        );
    }
    auto index = find_vgroup_id(id);
    if (index == SIZE_MAX) {
        throw std::logic_error(
            "game::attr_row::remove_vgroup_by_id: no such value group"
        );
    }
    m_vgroups.erase(m_vgroups.begin() + index);
}

}
}
