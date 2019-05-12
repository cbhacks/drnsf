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
attr_vgroup::attr_vgroup(size_t value_size, int column_id) :
    m_value_size(value_size),
    m_column_id(column_id)
{
    if (m_column_id < -1) {
        throw std::logic_error("game::attr_vgroup: invalid column id");
    }
}
// declared in game.hh
size_t attr_vgroup::value_size() const
{
    return m_value_size;
}

// declared in game.hh
int attr_vgroup::column_id() const
{
    return m_column_id;
}

// declared in game.hh
size_t attr_vgroup::count() const
{
    return m_values.size();
}

// declared in game.hh
const attr_value &attr_vgroup::get(size_t index) const
{
    if (index >= m_values.size()) {
        throw std::logic_error("game::attr_vgroup::get: out of bounds");
    }
    return m_values[index];
}

// declared in game.hh
void attr_vgroup::set(size_t index, attr_value value)
{
    if (value.size() != m_value_size) {
        throw std::logic_error(
            "game::attr_vgroup::set: value size mismatch"
        );
    }
    if (index >= m_values.size()) {
        throw std::logic_error("game::attr_vgroup::set: out of bounds");
    }
    m_values[index] = std::move(value);
}

// declared in game.hh
void attr_vgroup::insert(size_t index, attr_value value)
{
    if (value.size() != m_value_size) {
        throw std::logic_error(
            "game::attr_vgroup::insert: value size mismatch"
        );
    }
    if (index > m_values.size()) {
        throw std::logic_error("game::attr_vgroup::insert: out of bounds");
    }
    m_values.insert(m_values.begin() + index, std::move(value));
}

// declared in game.hh
void attr_vgroup::append(attr_value value)
{
    if (value.size() != m_value_size) {
        throw std::logic_error(
            "game::attr_vgroup::append: value size mismatch"
        );
    }
    m_values.push_back(std::move(value));
}

// declared in game.hh
void attr_vgroup::remove(size_t index)
{
    if (index >= m_values.size()) {
        throw std::logic_error("game::attr_vgroup::remove: out of bounds");
    }
    m_values.erase(m_values.begin() + index);
}

}
}
