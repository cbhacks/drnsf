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

#include <vector>
#include "res.hh"
#include "gfx.hh"

namespace drnsf {
namespace nsf {

enum class game_ver {
	crash1,
	crash2,
	crash3
};

class eid {
private:
	std::uint32_t m_value;

public:
	eid() = default;

	eid(std::uint32_t value) :
		m_value(value) {}

	operator std::uint32_t() const
	{
		return m_value;
	}

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

		return std::string(result,5);
	}
};

constexpr std::size_t page_size = 65536;

class archive : public res::asset {
	friend class res::asset;

private:
	std::vector<res::anyref> m_pages;

	explicit archive(res::project &proj) :
		asset(proj) {}

public:
	using ref = res::ref<archive>;

	DEFINE_APROP(pages);

	void import_file(TRANSACT,const std::vector<unsigned char> &data);

	template <typename Reflector>
	void reflect(Reflector &rfl)
	{
		asset::reflect(rfl);
		rfl.field(p_pages,"Pages");
	}
};

class spage : public res::asset {
	friend class res::asset;

private:
	std::vector<res::anyref> m_pagelets;
	uint16_t m_type = 0;
	uint32_t m_cid = 0;

	explicit spage(res::project &proj) :
		asset(proj) {}

public:
	using ref = res::ref<spage>;

	DEFINE_APROP(pagelets);
	DEFINE_APROP(type);
	DEFINE_APROP(cid);

	void import_file(TRANSACT,const std::vector<unsigned char> &data);

	template <typename Reflector>
	void reflect(Reflector &rfl)
	{
		asset::reflect(rfl);
		rfl.field(p_cid,"CID");
		rfl.field(p_type,"Type");
		rfl.field(p_pagelets,"Pagelets");
	}
};

class entry : public res::asset {
private:
	eid m_eid = 0;

protected:
	explicit entry(res::project &proj) :
		asset(proj) {}

public:
	using ref = res::ref<entry>;

	DEFINE_APROP(eid);

	template <typename Reflector>
	void reflect(Reflector &rfl)
	{
		asset::reflect(rfl);
		rfl.field(p_eid,"EID");
	}
};

using raw_item = std::vector<unsigned char>;
using raw_item_list = std::vector<raw_item>;

class raw_entry : public entry {
	friend class res::asset;

private:
	raw_item_list m_items;
	uint32_t m_type = 0;

	explicit raw_entry(res::project &proj) :
		entry(proj) {}

public:
	using ref = res::ref<raw_entry>;

	DEFINE_APROP(items);
	DEFINE_APROP(type);

	void import_file(TRANSACT,const std::vector<unsigned char> &data);

	template <typename T>
	void process_as(TRANSACT)
	{
		assert_alive();

		// Create the output entry under a reserved name,
		// '_PROCESS_OUTPUT'.
		res::ref<T> result = get_name() / "_PROCESS_OUTPUT";
		result.create(TS,get_proj());

		// Process the raw items into the output entry.
		result->set_eid(TS,get_eid());
		result->import_entry(TS,get_items());

		// Delete the raw entry (this) and rename the output entry over
		// this one's name.
		auto name = get_name();
		destroy(TS);
		result->rename(TS,name);
	}

	bool process_by_type(TRANSACT,game_ver ver);

	template <typename Reflector>
	void reflect(Reflector &rfl)
	{
		entry::reflect(rfl);
		rfl.field(p_items,"Items");
		rfl.field(p_type,"Type");
	}
};

class wgeo_v2 : public entry {
	friend class res::asset;

private:
	raw_item m_item0;
	raw_item m_item4;
	raw_item m_item6;

	gfx::model::ref m_model;

	explicit wgeo_v2(res::project &proj) :
		entry(proj) {}

public:
	using ref = res::ref<wgeo_v2>;

	DEFINE_APROP(item0);
	DEFINE_APROP(item4);
	DEFINE_APROP(item6);
	DEFINE_APROP(model);

	void import_entry(TRANSACT,const raw_item_list &items);

	template <typename Reflector>
	void reflect(Reflector &rfl)
	{
		entry::reflect(rfl);
		rfl.field(p_model,"Model");
		rfl.field(p_item0,"Item 0");
		rfl.field(p_item4,"Item 4");
		rfl.field(p_item6,"Item 6");
	}
};

}
}
