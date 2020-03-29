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
#include "misc.hh"

namespace drnsf {
namespace nsf {

// declared in nsf.hh
void archive::import_file(TRANSACT, const util::blob &data)
{
    assert_alive();

    std::vector<misc::raw_data::ref> pages;

    util::binreader r;
    uint32_t offset = 0;
    while (offset < data.size()) {

        // Read the magic number for this page.
        r.begin(&data[offset], page_size);
        auto magic = r.read_u16();
        r.end_early();
        
        // Skip the page if it is compressed.
        if (magic == 0x1234) {
            auto &&page = pages.emplace_back();
           
            // Create the page asset.
            auto index = pages.size();
            page = get_name() / "page-$"_fmt(index);
            page.create(TS, get_proj());

            // Copy the page data into the asset.
            page->set_data(TS, {
                data.data() + offset,
                data.data() + offset + page_size
            });

            offset += page_size;
        }
        else if (magic == 0x1235) {
            size_t size;
            decompress_spage(&data[offset], size);
            offset += size; 
        }
    }

    // Finish importing.
    set_pages(TS, {pages.begin(), pages.end()});
}

// declared in nsf.hh
util::blob archive::export_file() const
{
    util::blob data;

    for (auto &&i : util::range_of(get_pages())) {
        auto ref = get_pages()[i];

        if (!ref)
            throw res::export_error("nsf::archive: null page ref");

        misc::raw_data::ref raw_ref = ref;
        if (raw_ref.ok()) {
            auto raw_data = raw_ref->get_data();
            data.insert(data.end(), raw_data.begin(), raw_data.end());
            continue;
        }

        spage::ref spage_ref = ref;
        if (spage_ref.ok()) {
            auto spage_data = spage_ref->export_file();
            data.insert(data.end(), spage_data.begin(), spage_data.end());
            continue;
        }

        throw res::export_error("nsf::archive: page has incompatible type");
    }

    return data;
}

util::blob archive::decompress_spage(const unsigned char *data, size_t &size) 
{
    util::binreader r(util::read_dir::ltr);
    util::binwriter w;
    
    // Read the page header.
    r.begin(data, 12);
    auto magic   = r.read_u16();
    auto padding = r.read_u16();
    auto length  = r.read_u32();
    auto skip    = r.read_u32();
    r.end();

    // Ensure the magic number is correct.
    if (magic != 0x1235)
        throw res::import_error("nsf::archive::decompress_spage: bad magic number");

    // Decompress the page.
    size = 12;
    w.begin();
    while (w.length() < length) {
        r.begin(&data[size], 2);
        auto fmt = r.read_ubits(1);
        uint32_t seek, span;
        if (fmt) {
            seek = r.read_ubits(12);
            span = r.read_ubits(3) + 3;
            r.end_early();
            if (span == 10) span = 64;    
            r.begin(w, -seek);
            size += 2;
        }
        else {
            seek = 0xFFFFFFFF;
            span = r.read_ubits(7);
            r.end_early();
            r.begin(&data[size + 1], span);
            size += (1 + span);
        }
        while (seek < span) {
            w.write_bytes(r.read_bytes(seek));
            r.end();
            span -= seek;
            r.begin(w, -seek);
        }
        w.write_bytes(r.read_bytes(span));
        r.end_early();
    }
    size += skip;

    auto remainder = page_size - length;
    r.begin(&data[size], remainder);
    w.write_bytes(r.read_bytes(remainder));
    r.end();
    size += remainder; 
    return w.end();
}

}
}
