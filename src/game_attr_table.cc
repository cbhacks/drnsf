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
size_t attr_table::row_count() const
{
    return m_rows.size();
}

// declared in game.hh
const attr_row &attr_table::get_row_by_index(size_t index) const
{
    if (index >= m_rows.size()) {
        throw std::logic_error(
            "game::attr_table::get_row_by_index: out of bounds"
        );
    }
    return m_rows[index];
}

// declared in game.hh
size_t attr_table::find_row_id(int id) const
{
    auto it = std::lower_bound(
        m_rows.begin(),
        m_rows.end(),
        id,
        [](const attr_row &lhs, auto rhs) -> int {
            return lhs.id() - rhs;
        }
    );
    if (it == m_rows.end() || it->id() != id) {
        return SIZE_MAX;
    } else {
        return it - m_rows.begin();
    }
}

// declared in game.hh
bool attr_table::has_row_id(int id) const
{
    return find_row_id(id) != SIZE_MAX;
}

// declared in game.hh
const attr_row &attr_table::get_row_by_id(int id) const
{
    auto index = find_row_id(id);
    if (index == SIZE_MAX) {
        throw std::logic_error("game::attr_table::get_row_by_id: no such row");
    }
    return m_rows[index];
}

// declared in game.hh
void attr_table::put_row(attr_row row)
{
    auto it = std::lower_bound(
        m_rows.begin(),
        m_rows.end(),
        row.id(),
        [](const attr_row &lhs, auto rhs) -> int {
            return lhs.id() - rhs;
        }
    );
    if (it == m_rows.end() || it->id() != row.id()) {
        m_rows.insert(it, std::move(row));
    } else {
        *it = std::move(row);
    }
}

// declared in game.hh
void attr_table::remove_row_by_index(size_t index)
{
    if (index >= m_rows.size()) {
        throw std::logic_error(
            "game::attr_table::remove_row_by_index: out of bounds"
        );
    }
    m_rows.erase(m_rows.begin() + index);
}

// declared in game.hh
void attr_table::remove_row_by_id(int id)
{
    auto index = find_row_id(id);
    if (index == SIZE_MAX) {
        throw std::logic_error(
            "game::attr_table::remove_row_by_id: no such row"
        );
    }
    m_rows.erase(m_rows.begin() + index);
}

// declared in game.hh
void attr_table::import_file(const util::blob &data)
{
    util::binreader r;

    // Ensure the absolute minimum table header is present.
    if (data.size() < 16)
        throw res::import_error("game::attr_table: bad data size");

    // Parse the start of the header.
    r.begin(data);
    auto length    = r.read_u32();
    auto zero_A    = r.read_u32();
    auto zero_B    = r.read_u32();
    auto row_count = r.read_u32();

    // The initial field should be the same as the table size.
    if (length != data.size())
        throw res::import_error("game::attr_table: bad length field");

    // Ensure the two "zero fields" are clear. These fields are only used by
    // the game engine at runtime, and should not have any other value when
    // stored on disc.
    if (zero_A != 0 || zero_B != 0)
        throw res::import_error("game::attr_table: bad zero field");

    // Ensure an at least theoretically possible row count.
    if (row_count > (INT_MAX - 16) / 8)
        throw res::import_error("game::attr_table: invalid row count");

    // Ensure there is enough data to read the row descriptors.
    if (data.size() < 16 + row_count * 8) {
        throw res::import_error(
            "game::attr_table: bad data size for row count"
        );
    }

    // Exit early for a zero-row object. From now on, we can assume there is at
    // least one row.
    if (row_count == 0)
        return;

    // Parse the row descriptors from the header.
    struct row_descriptor {
        // Descriptor fields read from file:
        uint16_t id;
        size_t offset;
        uint8_t type_flags;
        uint8_t value_size;
        uint16_t group_count;

        // Descriptor fields computed:
        size_t end_offset;

        // Offsets are file-relative, however the data stored in the file is
        // relative to 12 bytes into the file.
    };
    std::vector<row_descriptor> row_descriptors(row_count);
    for (auto &descriptor : row_descriptors) {
        descriptor.id          = r.read_u16();
        descriptor.offset      = r.read_u16() + 12;
        descriptor.type_flags  = r.read_u8();
        descriptor.value_size  = r.read_u8();
        descriptor.group_count = r.read_u16();
    }
    r.end_early();

    // Ensure the first row offset matches the end of the header.
    if (row_descriptors[0].offset != 16 + row_count * 8)
        throw res::import_error("game::attr_table: first row starts late");

    // Set up the end_offset for each descriptor.
    for (size_t i = 1; i < row_count; i++) {
        row_descriptors[i - 1].end_offset = row_descriptors[i].offset;
    }
    row_descriptors.back().end_offset = data.size();

    // Parse each row.
    for (size_t i = 0; i < row_count; i++) {
        auto &&descriptor = row_descriptors[i];

        bool is_terminator = descriptor.type_flags & 0x80;
        bool is_jagged     = descriptor.type_flags & 0x40;
        bool is_columned   = descriptor.type_flags & 0x20;
        int  type          = descriptor.type_flags & 0x1F;

        // Ensure the is_terminator flag value is correct.
        if ((i == row_count - 1) != is_terminator)
            throw res::import_error("game::attr_table: bad terminator flag");

        // Ensure the row data is entirely within the bounds of the file.
        if (descriptor.offset > data.size()) {
            throw res::import_error(
                "game::attr_table: row begins out of bounds"
            );
        }
        if (descriptor.end_offset > data.size()) {
            throw res::import_error(
                "game::attr_table: row ends out of bounds"
            );
        }
        if (descriptor.end_offset < descriptor.offset) {
            throw res::import_error(
                "game::attr_table: row ends before it begins"
            );
        }

        // Ensure a row with this property does not already exist.
        if (has_row_id(descriptor.id)) {
            throw res::import_error(
                "game::attr_table: duplicate attribute ID"
            );
        }

        attr_row row(descriptor.id, descriptor.value_size, is_columned);
        std::vector<uint16_t> group_column_ids(descriptor.group_count);
        std::vector<uint16_t> group_value_counts(descriptor.group_count);

        r.begin(
            data.data() + descriptor.offset,
            descriptor.end_offset - descriptor.offset
        );

        // Read the value counts for each value group. If the "jagged" flag is
        // set, this is one count for each value group in the row. Otherwise,
        // a single value count is used for all of the groups.
        if (is_jagged) {
            for (auto &group_value_count : group_value_counts) {
                group_value_count = r.read_u16();
            }
        } else {
            auto shared_value_count = r.read_u16();
            for (auto &group_value_count : group_value_counts) {
                group_value_count = shared_value_count;
            }
        }

        // Read the column ID's, if this row is columned.
        if (is_columned) {
            int max_id = -1;
            for (auto &group_column_id : group_column_ids) {
                group_column_id = r.read_u16();

                if (group_column_id == max_id) {
                    throw res::import_error(
                        "game::attr_table: duplicate column ID"
                    );
                } else if (group_column_id < max_id) {
                    throw res::import_error(
                        "game:attr_table: column ID's out of order"
                    );
                }

                max_id = group_column_id;
            }
        }

        // Align to a 4-byte boundary before reading the values.
        r.discard_to_align(4);

        // Read the values for each group.
        for (size_t i = 0; i < descriptor.group_count; i++) {
            attr_vgroup group(
                descriptor.value_size,
                is_columned ? group_column_ids[i] : -1
            );

            for (size_t ii = 0; ii < group_value_counts[i]; ii++) {
                group.append(r.read_bytes(descriptor.value_size));
            }

            if (is_columned) {
                row.put_vgroup(std::move(group));
            } else {
                row.append_vgroup(std::move(group));
            }
        }

        // Align to a 4-byte boundary before the next row or EOF.
        r.discard_to_align(4);
        r.end();

        put_row(std::move(row));
    }
}

// declared in game.hh
util::blob attr_table::export_file() const
{
    return {};
}

}
}
