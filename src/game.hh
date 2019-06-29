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

#pragma once

/*
 * game.hh
 *
 * FIXME explain
 */

#include <vector>
#include "res.hh"

#if USE_GL
#include "gl.hh"
#endif

namespace drnsf {
namespace game {

/*
 * game::attr_value
 *
 * A single, manually specified value within an attribute value group. See
 * `attr_table' for more information.
 */
using attr_value = util::blob;

/*
 * game::attr_vgroup
 *
 * A group of values within an attribute row. See `attr_table' for more
 * information.
 */
class attr_vgroup {
private:
    // (var) m_value_size
    // The size, in bytes, of each value contained within this group.
    size_t m_value_size;

    // (var) m_column_id
    // The column ID of this group, or -1 if there is none.
    int m_column_id;

    // (var) m_values
    // The values contained within this group.
    std::vector<attr_value> m_values;

public:
    // (explicit ctor)
    // Constructs a new value group with the specified value size and column
    // ID.
    //
    // If the specified ID is -1 (the default), the value group is considered
    // not to have a column ID. Otherwise, the ID must be non-negative or an
    // exception is thrown.
    explicit attr_vgroup(size_t value_size, int column_id = -1);

    // (func) value_size
    // Returns the group's value size (in bytes).
    size_t value_size() const;

    // (func) column_id
    // Returns the group's column ID, or -1 if there is no column ID.
    int column_id() const;

    // (func) count
    // Returns the number of values in the group.
    size_t count() const;

    // (func) get
    // Returns the value at the specified index in the group. If the index is
    // outside the bounds of the group, an exception is thrown.
    const attr_value &get(size_t index) const;

    // (func) set
    // Sets the value at the specified index in the group. If the index is
    // out of bounds, or if the value's size does not match the group's value
    // size, an exception is thrown.
    void set(size_t index, attr_value value);

    // (func) insert
    // Inserts a value at the specified index in the group. If the index is
    // inside the bounds of the array, the value is inserted before the value
    // at that index. If the index is the size of the array, the value is
    // appended to the group. Otherwise, an exception is thrown.
    //
    // If the value's size does not match the group's value size, an exception
    // is thrown.
    void insert(size_t index, attr_value value);

    // (func) append
    // Appends the given value to the end of the group. If the value's size
    // does not match the group's value size, an exception is thrown.
    void append(attr_value value);

    // (func) remove
    // Removes the value at the specified index from the group. If the index is
    // out of bounds, an exception is thrown.
    void remove(size_t index);

    // (func) begin, end
    // Functions for iterating over the values in the group.
    auto begin() const {
        return m_values.begin();
    }
    auto end() const {
        return m_values.end();
    }
};

/*
 * game::attr_row
 *
 * A row within an attribute table. See `attr_table' for more information.
 */
class attr_row {
private:
    // (var) m_id
    // The attribute ID of this row.
    int m_id;

    // (var) m_type
    // The attribute type of attribute this row. This is a 5-bit value which
    // determines how the value data should be interpreted.
    int m_type;

    // (var) m_value_size
    // The size, in bytes, of each value contained within the value groups in
    // this row.
    size_t m_value_size;

    // (var) m_columned
    // True if this row is a "columned" row, false otherwise. If true, all of
    // the value groups in this row must have a column ID. If false, none of
    // the value groups may have a column ID.
    bool m_columned;

    // (var) m_vgroups
    // The value groups within this row. If this row is "columned", the groups
    // are sorted by column ID (ascending).
    std::vector<attr_vgroup> m_vgroups;

public:
    // (explicit ctor)
    // Constructs a new attribute row of the specified ID, type, value size, and
    // columned status.
    //
    // The given type must be a 5-bit unsigned value or an exception is thrown.
    explicit attr_row(int id, int type, size_t value_size, bool columned);

    // (func) id
    // Returns the attribute ID of this row.
    int id() const;

    // (func) type
    // Returns the attribute type of this row.
    int type() const;

    // (func) value_size
    // Returns the value size of this row.
    size_t value_size() const;

    // (func) is_columned
    // Returns true if the row is a "columned" row, false otherwise.
    bool is_columned() const;

    // (func) vgroup_count
    // Returns the number of value groups in the row.
    size_t vgroup_count() const;

    // (func) get_vgroup_by_index
    // Returns the value group at the specified index. The index must be less
    // than `vgroup_count()' or an exception is thrown.
    const attr_vgroup &get_vgroup_by_index(size_t index) const;

    // (func) find_vgroup_id
    // Returns the index of the first value group with the specified column ID,
    // or the index of the first value group with a greater column ID if there
    // is none. If all existing value groups have a lesser column ID, the count
    // (`vgroup_count()') is returned.
    //
    // (i.e.) This function returns the position at which front-insertions or
    // searches should start for the specified column ID.
    //
    // If the row is not columned, an exception is thrown.
    size_t find_vgroup_id(int id) const;

    // (func) has_vgroup_id
    // Returns true if a value group with the specified column ID exists in
    // the row, false otherwise. If the row is not columned, an exception is
    // thrown.
    bool has_vgroup_id(int id) const;

    // (func) get_vgroups_by_id
    // Returns the value groups with the specified column ID, in the order in
    // which they appear in the row. If the row does not contain any vgroups
    // with the specified ID, an empty list is returned.
    //
    // If the row is not columned, an exception is thrown.
    std::vector<attr_vgroup> get_vgroups_by_id(int id) const;

    // (func) insert_vgroup
    // Inserts a value group at the specified index in the row.
    //
    // For non-columned rows:
    //
    //   If the value group has a column ID, an exception is thrown.
    //
    //   If the index is inside the bounds of the array, the value group is
    //   inserted before the value group at the specified index. If the index
    //   is the size of the array, the value group is appended to the row.
    //   Otherwise, an exception is thrown.
    //
    // For columned rows:
    //
    //   If the value group does not have a column ID, an exception is thrown.
    //
    //   If the index refers to a value group with an equal column ID to the
    //   given value group, the given group is inserted before the group at the
    //   specified index.
    //
    //   If the index refers to a value group with a lesser column ID than the
    //   given value group, the given group is inserted as the first value group
    //   of its column ID.
    //
    //   If the index refers to a value group with a greater column ID than the
    //   given value group, or if the index is equal to the size of the array,
    //   the given group is inserted as the last value group of its column ID.
    //
    //   If the index is out of bounds and is not equal to the size of the
    //   value group array, an exception is thrown.
    //
    //   In any case, if insertion is successful, the value group array order
    //   is preserved such that no value group precedes a value group of lesser
    //   column ID.
    //
    // For both columned and non-columned rows, if the value group's value size
    // does not match the row's value size, an exception is thrown.
    void insert_vgroup(size_t index, attr_vgroup vgroup);

    // (func) append_vgroup
    // Appends a value group to the row.
    //
    // For non-columned rows:
    //
    //   If the value group has a column ID, an exception is thrown.
    //
    //   The value group is inserted at the end of the row.
    //
    // For columned rows:
    //
    //   If the value group does not have a column ID, an exception is thrown.
    //
    //   The value group is inserted into the row such that it is the last
    //   group of its column ID.
    //
    // For both columned and non-columned rows, if the value group's value size
    // does not match the row's value size, an exception is thrown.
    void append_vgroup(attr_vgroup vgroup);

    // (func) remove_vgroup_by_index
    // Removes the value group at the specified index. The index must be within
    // the bounds of the row's value group array or an exception is thrown.
    void remove_vgroup_by_index(size_t index);

    // (func) begin, end
    // Functions for iterating over the value groups in the row.
    auto begin() const {
        return m_vgroups.begin();
    }
    auto end() const {
        return m_vgroups.end();
    }
};

/*
 * game::attr_table
 *
 * An attribute table.
 *
 * This structure is used to define an object (1 table per object) or camera
 * rail segment (3 tables per segment) in Crash 2 and Crash 3.
 *
 * The structure is a table of rows and columns. Each row is defined by an
 * `attr_row', and corresponds to a type of attribute, such as name, ID, type,
 * path, scale, etc. If the table has columns, these columns correspond to
 * the individual positions within the path of the defined entity. Otherwise,
 * the table has no defined columns, and is more like a one-dimensional list of
 * rows.
 *
 * In Crash 2 and Crash 3, columns are only used when defining cameras, never
 * when defining objects.
 *
 * A row consists of one or more "value groups" (`attr_vgroup'). A value group
 * is a set of "values" (`attr_value'). Values are an array of bytes of some
 * unspecified interpretation. All values within a given row must have the same
 * size, however different rows within a single table may have different value
 * sizes.
 *
 * A row may be "columned" or "non-columned". In a "columned" row, each value
 * group maps to a specific column of the table (i.e. a specific position of
 * the defined entity). This is used for camera rail segment attribute tables
 * to specify values which apply to specific rail positions, such as entry
 * load lists or object spawn lists ("draw lists").
 *
 * In a "non-columned" row, no column organization appears, and each value
 * group exists independent of any path positions. Typically, a non-columned
 * row will only have a single value group.
 *
 * The rows contained within this table are sorted by attribute ID, ascending.
 */
class attr_table {
private:
    // (var) m_rows
    // The rows within this table. No two rows within the table may have the
    // same attribute ID.
    //
    // This list is sorted by ID ascending.
    std::vector<attr_row> m_rows;

public:
    // (func) row_count
    // Returns the number of rows in the table.
    size_t row_count() const;

    // (func) get_row_by_index
    // Returns the row at the specified index. The index must be less than
    // `row_count()' or an exception is thrown.
    const attr_row &get_row_by_index(size_t index) const;

    // (func) find_row_id
    // Returns the index of the row with the specified ID. If no such row is
    // present in the table, SIZE_MAX is returned.
    size_t find_row_id(int id) const;

    // (func) has_row_id
    // Returns true if a row with the specified ID exists in the table.
    bool has_row_id(int id) const;

    // (func) get_row_by_id
    // Returns the row with the specified ID. If no such row is present in the
    // table, an exception is thrown.
    const attr_row &get_row_by_id(int id) const;

    // (func) put_row
    // Inserts the row into the table based on its ID. If a row already exists
    // in the table with the given ID, it is overwritten.
    void put_row(attr_row row);

    // (func) remove_row_by_index
    // Removes the row at the specified index. The index must be less than
    // `row_count()' or an exception is thrown.
    void remove_row_by_index(size_t index);

    // (func) remove_row_by_id
    // Removes the row with the specified ID. If no such row is present in the
    // table, an exception is thrown.
    void remove_row_by_id(int id);

    // (func) import_file
    // FIXME explain
    void import_file(const util::blob &data);

    // (func) export_file
    // FIXME explain
    util::blob export_file() const;

    // (func) begin, end
    // Functions for iterating over the row collection.
    auto begin() const {
        return m_rows.begin();
    }
    auto end() const {
        return m_rows.end();
    }
};

/*
 * game::zone
 *
 * FIXME explain
 */
class zone : public res::asset {
    friend class res::asset;

private:
    // (explicit ctor)
    // FIXME explain
    explicit zone(res::project &proj) :
        asset(proj) {}

public:
    // (typedef) ref
    // FIXME explain
    using ref = res::ref<zone>;

    // (const) octree_header_size
    // The number of bytes in a ZDAT entry's second item before the octree. The
    // octree's non-leaf nodes use offsets relative to the start of the item,
    // so this must be subtracted out to retrieve a proper index.
    static constexpr size_t octree_header_size = 36;

    // (prop) x, y, z
    // The position of this zone's origin, in map units.
    DEFINE_APROP(x, int32_t, 0);
    DEFINE_APROP(y, int32_t, 0);
    DEFINE_APROP(z, int32_t, 0);

    // (prop) x_size, y_size, z_size
    // The size of this zone, in map units. The space occupied by the zone is
    // an axis-aligned 3D rectangle from (x, y, z) to (x + x_size, y + y_size,
    // z + z_size), i.e. the smallest axis-aligned bounding box containing
    // these two points.
    DEFINE_APROP(x_size, int32_t, 0);
    DEFINE_APROP(y_size, int32_t, 0);
    DEFINE_APROP(z_size, int32_t, 0);

    // (prop) x_res, y_res, z_res
    // The resolution of this zone's octree data for each axis. See `octree'
    // below for more details.
    DEFINE_APROP(x_res, int, 0);
    DEFINE_APROP(y_res, int, 0);
    DEFINE_APROP(z_res, int, 0);

    // (prop) octree_root
    // The root node of the octree (see below).
    DEFINE_APROP(octree_root, uint16_t, 0);

    // (prop) octree
    // A complex tree structure which defines this zone's static collidable
    // geometry. For a given axis N, the space occupied by the zone is bisected
    // up to `N_res' times (into up to `2 ** N_res' sized subregions). Each
    // subregion ---- FIXME
    //
    // Each tree node may have any of the following values:
    //
    // 00h:
    //   Leaf node; no collision (i.e. air, underwater, etc)
    //
    // Not 00h, with low bit set:
    //   Leaf node; the value determines how collision with this space works.
    //
    // Not 00h, with low bit clear:
    //   Non-leaf node; the value rightshifted by one is the index of a list of
    //   the child nodes (either 1, 2, 4, or 8 child nodes)
    //
    // FIXME explain
    //
    // Some game levels have additional static collidable geometry (e.g. VCOL)
    // which is handled separately from the zone octree structure.
    DEFINE_APROP(octree, std::vector<uint16_t>);

    // (func) import_item1
    // Imports the second-item data for a ZDAT entry into this zone asset.
    void import_item1(TRANSACT, const util::blob &data);

    // (func) export_item1
    // Exports the second-item data for a ZDAT entry from this zone asset.
    util::blob export_item1() const;

#if USE_GL
    // (var) m_octree_texture
    // A GL texture object which should be a GL_TEXTURE_3D of a flattened (3D,
    // not 2D; but not in a tree format) representation of the zone's collision
    // octree. The game::zone object will clear the `ok' flag on this texture
    // whenever the octree's data or resolution is changed, however it does not
    // build or upload any image to this texture object. Users must provide the
    // texture object's configuration and image data.
    gl::texture m_octree_texture;

protected:
    // (func) on_prop_change
    // Reacts to property changes to invalidate any necessary GL objects.
    void on_prop_change(void *prop) noexcept override
    {
        if (prop == &p_octree) {
            m_octree_texture.ok = false;
        }
    }
#endif
};

}

namespace reflect {

// reflection info for game::zone
template <>
struct asset_type_info<game::zone> {
    using base_type = res::asset;

    static constexpr const char *name = "game::zone";
    static constexpr int prop_count = 1;
};
template <>
struct asset_prop_info<game::zone, 0> {
    using type = int32_t;

    static constexpr const char *name = "x";
    static constexpr auto ptr = &game::zone::p_x;
};
template <>
struct asset_prop_info<game::zone, 1> {
    using type = int32_t;

    static constexpr const char *name = "y";
    static constexpr auto ptr = &game::zone::p_y;
};
template <>
struct asset_prop_info<game::zone, 2> {
    using type = int32_t;

    static constexpr const char *name = "z";
    static constexpr auto ptr = &game::zone::p_z;
};
template <>
struct asset_prop_info<game::zone, 3> {
    using type = int32_t;

    static constexpr const char *name = "x_size";
    static constexpr auto ptr = &game::zone::p_x_size;
};
template <>
struct asset_prop_info<game::zone, 4> {
    using type = int32_t;

    static constexpr const char *name = "y_size";
    static constexpr auto ptr = &game::zone::p_y_size;
};
template <>
struct asset_prop_info<game::zone, 5> {
    using type = int32_t;

    static constexpr const char *name = "z_size";
    static constexpr auto ptr = &game::zone::p_z_size;
};
template <>
struct asset_prop_info<game::zone, 6> {
    using type = int;

    static constexpr const char *name = "x_res";
    static constexpr auto ptr = &game::zone::p_x_res;
};
template <>
struct asset_prop_info<game::zone, 7> {
    using type = int;

    static constexpr const char *name = "y_res";
    static constexpr auto ptr = &game::zone::p_y_res;
};
template <>
struct asset_prop_info<game::zone, 8> {
    using type = int;

    static constexpr const char *name = "z_res";
    static constexpr auto ptr = &game::zone::p_z_res;
};
template <>
struct asset_prop_info<game::zone, 9> {
    using type = uint16_t;

    static constexpr const char *name = "octree_root";
    static constexpr auto ptr = &game::zone::p_octree_root;
};
template <>
struct asset_prop_info<game::zone, 10> {
    using type = std::vector<uint16_t>;

    static constexpr const char *name = "octree";
    static constexpr auto ptr = &game::zone::p_octree;
};

}
}
