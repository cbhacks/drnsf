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
#include "nsf.hh"

namespace drnsf {
namespace nsf {

// declared in res.hh
void zdat_v2::import_entry(TRANSACT, const std::vector<util::blob> &items)
{
    assert_alive();

    util::binreader r;

    // Ensure we have a valid number of items (2+).
    if (items.size() < 2)
        throw res::import_error("nsf::zdat_v2: bad item count");

    // Create the zone for this entry.
    game::zone::ref zone = get_proj().get_asset_root() / "zones" / get_eid().str();
    zone.create(TS, get_proj());
    set_zone(TS, zone);

    set_item0(TS, items[0]);

    // Import the zone position, size, and collision octree.
    zone->import_item1(TS, items[1]);

    std::vector<game::attr_table> member_tables;

    for (size_t i = 2; i < items.size(); i++) {
        member_tables.emplace_back();
        member_tables.back().import_file(items[i]);
    }
    set_member_tables(TS, std::move(member_tables));
}

// declared in nsf.hh
std::vector<util::blob> zdat_v2::export_entry(uint32_t &out_type) const
{
    assert_alive();

    out_type = 7;
    std::vector<util::blob> items(2);

    items[0] = get_item0();
    items[1] = get_zone()->export_item1();

    for (auto &&member_table : get_member_tables()) {
        items.push_back(member_table.export_file());
    }

    return items;
}

}
}
