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

#pragma once

/*
 * nsf.hh
 *
 * FIXME explain
 */

#include <vector>
#include "res.hh"
#include "gfx.hh"

namespace drnsf {
namespace nsf {

/*
 * nsf::game_ver
 *
 * FIXME explain
 */
enum class game_ver {
    crash1,
    crash2,
    crash3
};

/*
 * nsf::eid
 *
 * FIXME explain
 */
class eid {
private:
    // (var) m_value
    // FIXME explain
    std::uint32_t m_value;

public:
    // (default ctor)
    // FIXME explain
    eid() = default;

    // (conversion ctor)
    // FIXME explain
    eid(std::uint32_t value) :
        m_value(value) {}

    // (conversion operator)
    // FIXME explain
    operator std::uint32_t() const
    {
        return m_value;
    }

    // (ext-func) to_string
    // FIXME explain
    friend std::string to_string(eid value)
    {
        static const char dictionary[] =
            "0123456789"
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "_!";

        char result[5];

        result[0] = dictionary[(value >> 25) & 0x3F];
        result[1] = dictionary[(value >> 19) & 0x3F];
        result[2] = dictionary[(value >> 13) & 0x3F];
        result[3] = dictionary[(value >> 7) & 0x3F];
        result[4] = dictionary[(value >> 1) & 0x3F];

        return std::string(result, 5);
    }
};

/*
 * nsf::page_size
 *
 * FIXME explain
 */
constexpr std::size_t page_size = 65536;

/*
 * nsf::archive
 *
 * FIXME explain
 */
class archive : public res::asset {
    friend class res::asset;

private:
    // (explicit ctor)
    // FIXME explain
    explicit archive(res::project &proj) :
        asset(proj) {}

public:
    // (typedef) ref
    // FIXME explain
    using ref = res::ref<archive>;

    // (prop) pages
    // FIXME explain
    DEFINE_APROP(pages, std::vector<res::anyref>);

    // (func) import_file
    // FIXME explain
    void import_file(TRANSACT, const util::blob &data);

    // FIXME obsolete
    template <typename Reflector>
    void reflect(Reflector &rfl)
    {
        asset::reflect(rfl);
        rfl.field(p_pages, "Pages");
    }
};

/*
 * nsf::spage
 *
 * FIXME explain
 */
class spage : public res::asset {
    friend class res::asset;

private:
    // (explicit ctor)
    // FIXME explain
    explicit spage(res::project &proj) :
        asset(proj) {}

public:
    // (typedef) ref
    // FIXME explain
    using ref = res::ref<spage>;

    // (prop) pagelets
    // FIXME explain
    DEFINE_APROP(pagelets, std::vector<res::anyref>);

    // (prop) type
    // FIXME explain
    DEFINE_APROP(type, uint16_t);

    // (prop) cid
    // FIXME explain
    DEFINE_APROP(cid, uint32_t);

    // (prop) checksum
    // FIXME explain
    DEFINE_APROP(checksum, uint32_t);

    // (func) import_file
    // FIXME explain
    void import_file(TRANSACT, const util::blob &data);

    // FIXME obsolete
    template <typename Reflector>
    void reflect(Reflector &rfl)
    {
        asset::reflect(rfl);
        rfl.field(p_cid, "CID");
        rfl.field(p_type, "Type");
        rfl.field(p_pagelets, "Pagelets");
    }
};

/*
 * nsf::entry
 *
 * FIXME explain
 */
class entry : public res::asset {
protected:
    // (explicit ctor)
    // FIXME explain
    explicit entry(res::project &proj) :
        asset(proj) {}

public:
    // (typedef) ref
    // FIXME explain
    using ref = res::ref<entry>;

    // (prop) eid
    // FIXME explain
    DEFINE_APROP(eid, eid);

    // FIXME obsolete
    template <typename Reflector>
    void reflect(Reflector &rfl)
    {
        asset::reflect(rfl);
        rfl.field(p_eid, "EID");
    }
};

/*
 * nsf::raw_entry
 *
 * FIXME explain
 */
class raw_entry : public entry {
    friend class res::asset;

private:
    // (explicit ctor)
    // FIXME explain
    explicit raw_entry(res::project &proj) :
        entry(proj) {}

public:
    // (typedef) ref
    // FIXME explain
    using ref = res::ref<raw_entry>;

    // (prop) items
    // FIXME explain
    DEFINE_APROP(items, std::vector<util::blob>);

    // (prop) type
    // FIXME explain
    DEFINE_APROP(type, uint32_t);

    // (func) import_file
    // FIXME explain
    void import_file(TRANSACT, const util::blob &data);

    // (func) process_as<T>
    // FIXME explain
    template <typename T>
    void process_as(TRANSACT)
    {
        assert_alive();

        // Create the output entry under a reserved name,
        // '_PROCESS_OUTPUT'.
        res::ref<T> result = get_name();
        rename(TS, result / "_RAW");
        result.create(TS, get_proj());

        // Process the raw items into the output entry.
        result->set_eid(TS, get_eid());
        result->import_entry(TS, get_items());

        destroy(TS);
    }

    // (func) process_by_type
    // FIXME explain
    bool process_by_type(TRANSACT, game_ver ver);

    // FIXME obsolete
    template <typename Reflector>
    void reflect(Reflector &rfl)
    {
        entry::reflect(rfl);
        rfl.field(p_items, "Items");
        rfl.field(p_type, "Type");
    }
};

/*
 * nsf::wgeo_v2
 *
 * FIXME explain
 */
class wgeo_v2 : public entry {
    friend class res::asset;

private:
    // (explicit ctor)
    // FIXME explain
    explicit wgeo_v2(res::project &proj) :
        entry(proj) {}

public:
    // (typedef) ref
    // FIXME explain
    using ref = res::ref<wgeo_v2>;

    // (prop) info_unk0
    // FIXME explain
    DEFINE_APROP(info_unk0, std::uint32_t);

    // (prop) tpag_ref_count, tpag_ref*
    DEFINE_APROP(tpag_ref_count, std::uint32_t);
    DEFINE_APROP(tpag_ref0, std::uint32_t);
    DEFINE_APROP(tpag_ref1, std::uint32_t);
    DEFINE_APROP(tpag_ref2, std::uint32_t);
    DEFINE_APROP(tpag_ref3, std::uint32_t);
    DEFINE_APROP(tpag_ref4, std::uint32_t);
    DEFINE_APROP(tpag_ref5, std::uint32_t);
    DEFINE_APROP(tpag_ref6, std::uint32_t);
    DEFINE_APROP(tpag_ref7, std::uint32_t);

    // (prop) item4
    // FIXME explain
    DEFINE_APROP(item4, util::blob);

    // (prop) item6
    // FIXME explain
    DEFINE_APROP(item6, util::blob);

    // (prop) model
    // FIXME explain
    DEFINE_APROP(model, gfx::model::ref);

    // (func) import_entry
    // FIXME explain
    void import_entry(TRANSACT, const std::vector<util::blob> &items);

    // FIXME obsolete
    template <typename Reflector>
    void reflect(Reflector &rfl)
    {
        entry::reflect(rfl);
        rfl.field(p_model, "Model");
        rfl.field(p_item4, "Item 4");
        rfl.field(p_item6, "Item 6");
    }
};

}
}
