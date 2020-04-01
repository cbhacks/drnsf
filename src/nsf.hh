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
 * nsf::charset
 *
 * The 64-char charset used in Crash. This is used for entry ID's (`nsf::eid')
 * as well as level ID's (e.g. Snow Go (0x0E) as "levele").
 */
static const char charset[] =
    "0123456789"
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "_!";

/*
 * nsf::eid
 *
 * FIXME explain
 */
class eid {
private:
    // (var) m_value
    // FIXME explain
    uint32_t m_value;

public:
    // (default ctor)
    // FIXME explain
    eid() = default;

    // (conversion ctor)
    // FIXME explain
    eid(uint32_t value) :
        m_value(value) {}

    // (conversion operator)
    // FIXME explain
    operator uint32_t() const
    {
        return m_value;
    }

    // (func) is_valid
    // Returns true if the EID is valid, false otherwise. An EID is only valid
    // if its least-significant bit is set and its most-significant bit is
    // clear.
    bool is_valid() const
    {
        return (m_value & 1) && (~m_value & 0x80000000);
    }

    // (func) str
    // Returns the EID in string form. This is usually 5 characters in length,
    // but could be 6 or 7 if the EID value has invalid high or low bits.
    std::string str() const;

    // (func) try_parse
    // Attempts to parse the given string as an EID in the same format returned
    // by `str' and `to_string'. If parsing is successful, the value of `this'
    // is changed to that EID and the function returns true. Otherwise, the
    // function returns false and there is no change to the value of `this'.
    bool try_parse(const std::string &str);

    // (ext-func) to_string
    // This function provides a non-member `to_string' implementation of EID.
    // See `util::to_string' for more information.
    friend std::string to_string(const eid &value)
    {
        return value.str();
    }
};

/*
 * nsf::page_size
 *
 * FIXME explain
 */
constexpr size_t page_size = 65536;

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

    // (func) export_file
    // FIXME explain
    util::blob export_file() const;

    // (func) decompress_spage
    // FIXME explain
    static util::blob decompress_spage(const unsigned char *data, size_t &size);
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

    // (func) export_file
    // FIXME explain
    util::blob export_file() const;
};

/*
 * nsf::tpage
 *
 * FIXME explain
 */
class tpage : public res::asset {
    friend class res::asset;

private:
    // (explicit ctor)
    // FIXME explain
    explicit tpage(res::project &proj) :
        asset(proj) {}

public: 
    // (typedef) ref
    // FIXME explain
    using ref = res::ref<tpage>;
 
    // (prop) type
    // FIXME explain
    DEFINE_APROP(type, uint16_t);

    // (prop) eid
    // FIXME explain
    DEFINE_APROP(eid, eid);

    // (prop) entry_type
    // FIXME explain
    DEFINE_APROP(entry_type, uint32_t);

    // (prop) texture
    // FIXME explain
    DEFINE_APROP(texture, gfx::texture::ref);

    // (func) import_file
    // FIXME explain
    void import_file(TRANSACT, const util::blob &data);
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

    // (func) export_file
    // FIXME explain
    util::blob export_file() const;

    // (pure func) export_entry
    // FIXME explain
    virtual std::vector<util::blob> export_entry(
        uint32_t &out_type) const = 0;
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

    // (func) export_entry
    // FIXME explain
    std::vector<util::blob> export_entry(
        uint32_t &out_type) const final override;

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
};

/*
 * nsf::wgeo_v1
 *
 * FIXME explain
 */
class wgeo_v1 : public entry {
    friend class res::asset;

private:
    // (explicit ctor)
    // FIXME explain
    explicit wgeo_v1(res::project &proj) :
        entry(proj) {}

public:
    // (typedef) ref
    // FIXME explain
    using ref = res::ref<wgeo_v1>;

    // (prop) tpag_ref_count, tpag_ref*
    DEFINE_APROP(tpag_ref_count, uint32_t);
    DEFINE_APROP(tpag_ref0, uint32_t);
    DEFINE_APROP(tpag_ref1, uint32_t);
    DEFINE_APROP(tpag_ref2, uint32_t);
    DEFINE_APROP(tpag_ref3, uint32_t);
    DEFINE_APROP(tpag_ref4, uint32_t);
    DEFINE_APROP(tpag_ref5, uint32_t);
    DEFINE_APROP(tpag_ref6, uint32_t);
    DEFINE_APROP(tpag_ref7, uint32_t);

    // (prop) world
    // FIXME explain
    DEFINE_APROP(world, gfx::world::ref);

    // (func) import_entry
    // FIXME explain
    void import_entry(TRANSACT, const std::vector<util::blob> &items);

    // (func) export_entry
    // FIXME explain
    std::vector<util::blob> export_entry(
        uint32_t &out_type) const final override;
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
    DEFINE_APROP(info_unk0, uint32_t);

    // (prop) tpag_ref_count, tpag_ref*
    DEFINE_APROP(tpag_ref_count, uint32_t);
    DEFINE_APROP(tpag_ref0, uint32_t);
    DEFINE_APROP(tpag_ref1, uint32_t);
    DEFINE_APROP(tpag_ref2, uint32_t);
    DEFINE_APROP(tpag_ref3, uint32_t);
    DEFINE_APROP(tpag_ref4, uint32_t);
    DEFINE_APROP(tpag_ref5, uint32_t);
    DEFINE_APROP(tpag_ref6, uint32_t);
    DEFINE_APROP(tpag_ref7, uint32_t);

    // (prop) item4
    // FIXME explain
    DEFINE_APROP(item4, util::blob);

    // (prop) item6
    // FIXME explain
    DEFINE_APROP(item6, util::blob);

    // (prop) world
    // FIXME explain
    DEFINE_APROP(world, gfx::world::ref);

    // (func) import_entry
    // FIXME explain
    void import_entry(TRANSACT, const std::vector<util::blob> &items);

    // (func) export_entry
    // FIXME explain
    std::vector<util::blob> export_entry(
        uint32_t &out_type) const final override;
};

}

namespace reflect {

// reflection info for nsf::archive
template <>
struct asset_type_info<nsf::archive> {
    using base_type = res::asset;

    static constexpr const char *name = "nsf::archive";
    static constexpr int prop_count = 1;
};
template <>
struct asset_prop_info<nsf::archive, 0> {
    using type = std::vector<res::anyref>;

    static constexpr const char *name = "pages";
    static constexpr auto ptr = &nsf::archive::p_pages;
};

// reflection info for nsf::spage
template <>
struct asset_type_info<nsf::spage> {
    using base_type = res::asset;

    static constexpr const char *name = "nsf::spage";
    static constexpr int prop_count = 4;
};
template <>
struct asset_prop_info<nsf::spage, 0> {
    using type = std::vector<res::anyref>;

    static constexpr const char *name = "pagelets";
    static constexpr auto ptr = &nsf::spage::p_pagelets;
};
template <>
struct asset_prop_info<nsf::spage, 1> {
    using type = uint16_t;

    static constexpr const char *name = "type";
    static constexpr auto ptr = &nsf::spage::p_type;
};
template <>
struct asset_prop_info<nsf::spage, 2> {
    using type = uint32_t;

    static constexpr const char *name = "cid";
    static constexpr auto ptr = &nsf::spage::p_cid;
};
template <>
struct asset_prop_info<nsf::spage, 3> {
    using type = uint32_t;

    static constexpr const char *name = "checksum";
    static constexpr auto ptr = &nsf::spage::p_checksum;
};

// reflection info for nsf::entry
template <>
struct asset_type_info<nsf::entry> {
    using base_type = res::asset;

    static constexpr const char *name = "nsf::entry";
    static constexpr int prop_count = 1;
};
template <>
struct asset_prop_info<nsf::entry, 0> {
    using type = nsf::eid;

    static constexpr const char *name = "eid";
    static constexpr auto ptr = &nsf::entry::p_eid;
};

// reflection info for nsf::raw_entry
template <>
struct asset_type_info<nsf::raw_entry> {
    using base_type = nsf::entry;

    static constexpr const char *name = "nsf::raw_entry";
    static constexpr int prop_count = 2;
};
template <>
struct asset_prop_info<nsf::raw_entry, 0> {
    using type = std::vector<util::blob>;

    static constexpr const char *name = "items";
    static constexpr auto ptr = &nsf::raw_entry::p_items;
};
template <>
struct asset_prop_info<nsf::raw_entry, 1> {
    using type = uint32_t;

    static constexpr const char *name = "type";
    static constexpr auto ptr = &nsf::raw_entry::p_type;
};

// reflection info for nsf::tpage
template <>
struct asset_type_info<nsf::tpage> {
    using base_type = res::asset;

    static constexpr const char *name = "nsf::tpage";
    static constexpr int prop_count = 4;
};
template <>
struct asset_prop_info<nsf::tpage, 0> {
    using type = uint16_t;

    static constexpr const char *name = "type";
    static constexpr auto ptr = &nsf::tpage::p_type;
};
template <>
struct asset_prop_info<nsf::tpage, 1> {
    using type = nsf::eid;

    static constexpr const char *name = "eid";
    static constexpr auto ptr = &nsf::tpage::p_eid;
};
template <>
struct asset_prop_info<nsf::tpage, 2> {
    using type = uint32_t;

    static constexpr const char *name = "entry_type";
    static constexpr auto ptr = &nsf::tpage::p_entry_type;
};
template <>
struct asset_prop_info<nsf::tpage, 3> {
    using type = gfx::texture::ref;

    static constexpr const char *name = "texture";
    static constexpr auto ptr = &nsf::tpage::p_texture;
};

// reflection info for nsf::wgeo_v1
template <>
struct asset_type_info<nsf::wgeo_v1> {
    using base_type = nsf::entry;

    static constexpr const char *name = "nsf::wgeo_v1";
    static constexpr int prop_count = 11;
};
template <>
struct asset_prop_info<nsf::wgeo_v1, 0> {
    using type = uint32_t;

    static constexpr const char *name = "tpag_ref_count";
    static constexpr auto ptr = &nsf::wgeo_v1::p_tpag_ref_count;
};
template <>
struct asset_prop_info<nsf::wgeo_v1, 1> {
    using type = uint32_t;

    static constexpr const char *name = "tpag_ref0";
    static constexpr auto ptr = &nsf::wgeo_v1::p_tpag_ref0;
};
template <>
struct asset_prop_info<nsf::wgeo_v1, 3> {
    using type = uint32_t;

    static constexpr const char *name = "tpag_ref1";
    static constexpr auto ptr = &nsf::wgeo_v1::p_tpag_ref1;
};
template <>
struct asset_prop_info<nsf::wgeo_v1, 4> {
    using type = uint32_t;

    static constexpr const char *name = "tpag_ref2";
    static constexpr auto ptr = &nsf::wgeo_v1::p_tpag_ref2;
};
template <>
struct asset_prop_info<nsf::wgeo_v1, 5> {
    using type = uint32_t;

    static constexpr const char *name = "tpag_ref3";
    static constexpr auto ptr = &nsf::wgeo_v1::p_tpag_ref3;
};
template <>
struct asset_prop_info<nsf::wgeo_v1, 6> {
    using type = uint32_t;

    static constexpr const char *name = "tpag_ref4";
    static constexpr auto ptr = &nsf::wgeo_v1::p_tpag_ref4;
};
template <>
struct asset_prop_info<nsf::wgeo_v1, 7> {
    using type = uint32_t;

    static constexpr const char *name = "tpag_ref5";
    static constexpr auto ptr = &nsf::wgeo_v1::p_tpag_ref5;
};
template <>
struct asset_prop_info<nsf::wgeo_v1, 8> {
    using type = uint32_t;

    static constexpr const char *name = "tpag_ref6";
    static constexpr auto ptr = &nsf::wgeo_v1::p_tpag_ref6;
};
template <>
struct asset_prop_info<nsf::wgeo_v1, 9> {
    using type = uint32_t;

    static constexpr const char *name = "tpag_ref7";
    static constexpr auto ptr = &nsf::wgeo_v1::p_tpag_ref7;
};
template <>
struct asset_prop_info<nsf::wgeo_v1, 10> {
    using type = gfx::world::ref;

    static constexpr const char *name = "world";
    static constexpr auto ptr = &nsf::wgeo_v1::p_world;
};

// reflection info for nsf::wgeo_v2
template <>
struct asset_type_info<nsf::wgeo_v2> {
    using base_type = nsf::entry;

    static constexpr const char *name = "nsf::wgeo_v2";
    static constexpr int prop_count = 13;
};
template <>
struct asset_prop_info<nsf::wgeo_v2, 0> {
    using type = uint32_t;

    static constexpr const char *name = "info_unk0";
    static constexpr auto ptr = &nsf::wgeo_v2::p_info_unk0;
};
template <>
struct asset_prop_info<nsf::wgeo_v2, 1> {
    using type = uint32_t;

    static constexpr const char *name = "tpag_ref_count";
    static constexpr auto ptr = &nsf::wgeo_v2::p_tpag_ref_count;
};
template <>
struct asset_prop_info<nsf::wgeo_v2, 2> {
    using type = uint32_t;

    static constexpr const char *name = "tpag_ref0";
    static constexpr auto ptr = &nsf::wgeo_v2::p_tpag_ref0;
};
template <>
struct asset_prop_info<nsf::wgeo_v2, 3> {
    using type = uint32_t;

    static constexpr const char *name = "tpag_ref1";
    static constexpr auto ptr = &nsf::wgeo_v2::p_tpag_ref1;
};
template <>
struct asset_prop_info<nsf::wgeo_v2, 4> {
    using type = uint32_t;

    static constexpr const char *name = "tpag_ref2";
    static constexpr auto ptr = &nsf::wgeo_v2::p_tpag_ref2;
};
template <>
struct asset_prop_info<nsf::wgeo_v2, 5> {
    using type = uint32_t;

    static constexpr const char *name = "tpag_ref3";
    static constexpr auto ptr = &nsf::wgeo_v2::p_tpag_ref3;
};
template <>
struct asset_prop_info<nsf::wgeo_v2, 6> {
    using type = uint32_t;

    static constexpr const char *name = "tpag_ref4";
    static constexpr auto ptr = &nsf::wgeo_v2::p_tpag_ref4;
};
template <>
struct asset_prop_info<nsf::wgeo_v2, 7> {
    using type = uint32_t;

    static constexpr const char *name = "tpag_ref5";
    static constexpr auto ptr = &nsf::wgeo_v2::p_tpag_ref5;
};
template <>
struct asset_prop_info<nsf::wgeo_v2, 8> {
    using type = uint32_t;

    static constexpr const char *name = "tpag_ref6";
    static constexpr auto ptr = &nsf::wgeo_v2::p_tpag_ref6;
};
template <>
struct asset_prop_info<nsf::wgeo_v2, 9> {
    using type = uint32_t;

    static constexpr const char *name = "tpag_ref7";
    static constexpr auto ptr = &nsf::wgeo_v2::p_tpag_ref7;
};
template <>
struct asset_prop_info<nsf::wgeo_v2, 10> {
    using type = util::blob;

    static constexpr const char *name = "item4";
    static constexpr auto ptr = &nsf::wgeo_v2::p_item4;
};
template <>
struct asset_prop_info<nsf::wgeo_v2, 11> {
    using type = util::blob;

    static constexpr const char *name = "item6";
    static constexpr auto ptr = &nsf::wgeo_v2::p_item6;
};
template <>
struct asset_prop_info<nsf::wgeo_v2, 12> {
    using type = gfx::world::ref;

    static constexpr const char *name = "world";
    static constexpr auto ptr = &nsf::wgeo_v2::p_world;
};

}
}
