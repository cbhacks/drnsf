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
#include "misc.hh"

namespace drnsf {
namespace nsf {

// declared in res.hh
void spage::import_file(TRANSACT, const util::blob &data)
{
    assert_alive();

    util::binreader r;
    r.begin(data);

    // Ensure the page data is the correct size (64K).
    if (data.size() != page_size)
        throw 0; // FIXME

    // Read the page header.
    r.expect_u16(0x1234);
    auto type          = r.read_u16();
    auto cid           = r.read_u32();
    auto pagelet_count = r.read_u32();
    auto checksum      = r.read_u32();

    // Read the pagelet offsets. Pagelets should be entries, but we treat
    // them as blobs instead so they can be totally unprocessed, etc.
    std::vector<std::uint32_t> pagelet_offsets(pagelet_count + 1);
    for (auto &&pagelet_offset : pagelet_offsets) {
        pagelet_offset = r.read_u32();
    }
    r.end_early();

    // Copy the data for each pagelet as a new raw_data asset. The caller
    // can later process these into entries if desired.
    std::vector<misc::raw_data::ref> pagelets(pagelet_count);
    for (auto &&i : util::range_of(pagelets)) {
        auto &&pagelet = pagelets[i];
        auto &&pagelet_start_offset = pagelet_offsets[i];
        auto &&pagelet_end_offset = pagelet_offsets[i + 1];

        // Ensure the pagelet data doesn't overlap the page header.
        if (pagelet_start_offset < 20 + pagelet_count * 4)
            throw 0; // FIXME

        // Ensure the pagelet data doesn't extend past the end of the
        // page (overrun).
        if (pagelet_end_offset > page_size)
            throw 0; // FIXME

        // Ensure the pagelet doesn't end before it begins (negative
        // pagelet size).
        if (pagelet_end_offset < pagelet_start_offset)
            throw 0; // FIXME

        // Create the pagelet asset.
        pagelet = get_name() / "pagelet-$"_fmt(i);
        pagelet.create(TS, get_proj());

        // Copy the pagelet data into the asset.
        pagelet->set_data(TS,
            {&data[pagelet_start_offset], &data[pagelet_end_offset]}
        );
    }

    // Finish importing.
    set_type(TS, type);
    set_cid(TS, cid);
    set_pagelets(TS, {pagelets.begin(), pagelets.end()});
}

}
}
