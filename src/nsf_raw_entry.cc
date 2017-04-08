//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017  DRNSF contributors
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
#include "gui.hh"

namespace drnsf {
namespace nsf {

void raw_entry::import_file(TRANSACT,const std::vector<unsigned char> &data)
{
	assert_alive();

	util::binreader r;
	r.begin(data);

	// Read the entry header.
	r.expect_u32(0x100FFFF);
	auto eid        = r.read_u32();
	auto type       = r.read_u32();
	auto item_count = r.read_u32();

	// Read the item offsets.
	std::vector<std::uint32_t> item_offsets(item_count + 1);
	for (auto &&item_offset : item_offsets) {
		item_offset = r.read_u32();
	}
	r.end_early();

	// Copy the data for each item.
	std::vector<raw_item> items(item_count);
	for (auto &&i : util::range_of(items)) {
		auto &&item_start_offset = item_offsets[i];
		auto &&item_end_offset = item_offsets[i + 1];

		// Ensure the item data doesn't overlap the entry header.
		if (item_start_offset < 20 + item_count * 4)
			throw 0; // FIXME

		// Ensure the item data doesn't extend past the end of the
		// entry (overflow).
		if (item_end_offset > data.size())
			throw 0; // FIXME

		// Ensure the item doesn't end before it begins (negative
		// item size).
		if (item_end_offset < item_start_offset)
			throw 0; // FIXME

		// Extract the item's data.
		items[i] = {&data[item_start_offset],&data[item_end_offset]};
	}

	// Finish importing.
	set_eid(TS,eid);
	set_type(TS,type);
	set_items(TS,std::move(items));
}

bool raw_entry::process_by_type(TRANSACT,game_ver ver)
{
	assert_alive();

	switch (ver) {
	case game_ver::crash1:
		break;
	case game_ver::crash2:
		switch (get_type()) {
		case 3:
			process_as<wgeo_v2>(TS);
			return true;
		}
		break;
	case game_ver::crash3:
		break;
	}

	return false;
}

}
}
