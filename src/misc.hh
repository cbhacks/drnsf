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

#pragma once

/*
 * misc.hh
 *
 * Not to be confused with `util.hh'.
 *
 * This header defines miscellaneous asset types which don't fit into any other
 * existing category.
 *
 * For more information on the asset system, see `asset.hh'.
 */

#include <vector>
#include "res.hh"

namespace drnsf {
namespace misc {

/*
 * misc::raw_data
 *
 * This asset type represents a series of bytes of no particular purpose or no
 * known purpose.
 *
 * For example, when importing a file for processing, this type is likely to be
 * the interrim format after importing the data but before parsing it into its
 * actual intended asset type.
 *
 * A real asset which does not yet have an internal supported format may be held
 * as a `misc::raw_data' asset as this also fits that purpose.
 */
class raw_data : public res::asset {
    friend class res::asset;

private:
    // (explicit ctor)
    // FIXME explain
    explicit raw_data(res::project &proj) :
        asset(proj) {}

public:
    // (typedef) ref
    // FIXME explain
    using ref = res::ref<raw_data>;

    // (prop) data
    // FIXME explain
    DEFINE_APROP(data, util::blob);
};

}

namespace reflect {

// reflection info for misc::raw_data
template <>
struct asset_type_info<misc::raw_data> {
    using base_type = res::asset;

    static constexpr const char *name = "misc::raw_data";
    static constexpr int prop_count = 1;
};
template <>
struct asset_prop_info<misc::raw_data, 0> {
    using type = util::blob;

    static constexpr const char *name = "data";
    static constexpr auto ptr = &misc::raw_data::p_data;
};

}
}
