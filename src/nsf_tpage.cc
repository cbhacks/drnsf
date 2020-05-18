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

// declared in res.hh
void tpage::import_file(TRANSACT, const util::blob &data)
{
    assert_alive();

    util::binreader r;
    r.begin(data);

    // Ensure the page data is the correct size (64K).
    if (data.size() != page_size)
        throw res::import_error("nsf::tpage: not 64K");

    // Read the texture page header.
    auto magic      = r.read_u16();
    auto type       = r.read_u16();
    auto eid        = r.read_u32();
    auto entry_type = r.read_u32();
    r.end_early();

    // Ensure the magic number is correct.
    if (magic != 0x1234)
        throw res::import_error("nsf::tpage: bad magic number");

    // Create the texture asset.
    res::atom atom = get_proj().get_asset_root()
        / "textures"
        / "$"_fmt(nsf::eid(eid));

    gfx::texture::ref texture = atom;
    texture.create(TS, get_proj());
    texture->set_texels(TS, data);

    // Finish importing.
    set_type(TS, type);
    set_eid(TS, eid);
    set_entry_type(TS, entry_type);
    set_texture(TS, texture);
}

/*
// declared in nsf.hh
util::blob tpage::export_file() const
{
    assert_alive();

    util::binwriter w;
    w.begin();

    // Write the page header.
    w.write_u16(0x1234);
    w.write_u16(get_type());
    w.write_u32(get_eid());
    w.write_u32(get_entry_type());
        
    // TODO: write texels here

    return data;
}
*/

}
}
