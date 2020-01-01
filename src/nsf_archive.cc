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

    // Ensure the NSF size is a multiple of the page size (64K).
    if (data.size() % page_size != 0)
        throw res::import_error("nsf::archive: size not multiple of 64K");

    int page_count = data.size() / page_size;

    // Copy the data for each page as a new raw_data asset. The caller can
    // later process these into standard or texture pages if desired.
    std::vector<misc::raw_data::ref> pages(page_count);
    for (auto &&i : util::range_of(pages)) {
        auto &&page = pages[i];

        // Create the page asset.
        page = get_name() / "page-$"_fmt(i);
        page.create(TS, get_proj());

        // Copy the page data into the asset.
        page->set_data(TS, {
            data.data() + page_size * i,
            data.data() + page_size * (i + 1)
        });
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

}
}
