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
#include "nsf.hh"

namespace drnsf {
namespace nsf {

// declared in nsf.hh
util::blob entry::export_file() const
{
    assert_alive();

    util::binwriter w;
    w.begin();

    uint32_t type;
    auto items = export_entry(type);

    // Write the entry header.
    w.write_u32(0x100FFFF);
    w.write_u32(get_eid());
    w.write_u32(type);
    w.write_u32(items.size());

    // Calculate and write the item offsets.
    uint32_t item_offset = 20 + items.size() * 4;
    for (auto &&item : items) {
        w.write_u32(item_offset);
        item_offset += item.size();
    }
    w.write_u32(item_offset);

    auto data = w.end();

    // Write the items themselves.
    for (auto &&item : items) {
        data.insert(data.end(), item.begin(), item.end());
    }

    return data;
}

}
}
