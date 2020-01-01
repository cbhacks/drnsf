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
#include "game.hh"

namespace drnsf {
namespace game {

// declared in game.hh
void zone::import_item1(TRANSACT, const util::blob &data)
{
    assert_alive();

    util::binreader r;

    // Ensure we have a valid item size.
    if (data.size() < octree_header_size)
        throw res::import_error("game::zone: bad item1 size");
    if (data.size() % 2 != 0)
        throw res::import_error("game::zone: bad item1 size");

    r.begin(data);
    set_x(TS, r.read_s32());
    set_z(TS, r.read_s32());
    set_y(TS, r.read_s32());
    set_x_size(TS, r.read_s32());
    set_z_size(TS, r.read_s32());
    set_y_size(TS, r.read_s32());
    auto node_count = r.read_u32() + 2;
    set_octree_root(TS, r.read_u16());
    set_x_res(TS, r.read_u16());
    set_z_res(TS, r.read_u16());
    set_y_res(TS, r.read_u16());

    std::vector<uint16_t> octree;
    for (size_t i = 0; i < node_count; i++) {
        octree.push_back(r.read_u16());
    }
    set_octree(TS, std::move(octree));

    r.end();
}

// declared in game.hh
util::blob zone::export_item1() const
{
    assert_alive();

    util::binwriter w;
    w.begin();

    w.write_s32(get_x());
    w.write_s32(get_z());
    w.write_s32(get_y());
    w.write_s32(get_x_size());
    w.write_s32(get_z_size());
    w.write_s32(get_y_size());

    auto octree_size = get_octree().size();
    if (octree_size < 2)
        throw res::export_error("game::zone: octree too short");
    if (octree_size - 2 > 0xFFFFFFFFUL)
        throw res::export_error("game::zone: octree too long");
    w.write_u32(octree_size - 2);

    w.write_u16(get_octree_root());

    auto x_res = get_x_res();
    auto y_res = get_y_res();
    auto z_res = get_z_res();
    if (x_res > 0xFFFF || y_res > 0xFFFF || z_res > 0xFFFF) {
        throw res::export_error("game::zone: octree resolution too high");
    }
    if (x_res < 0 || y_res < 0 || z_res < 0) {
        throw res::export_error("game::zone: octree resolution is negative");
    }
    w.write_u16(x_res);
    w.write_u16(z_res);
    w.write_u16(y_res);

    for (const auto &node : get_octree()) {
        w.write_u16(node);
    }

    return w.end();
}

}
}
