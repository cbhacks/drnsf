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

namespace nsf {

constexpr std::size_t page_size = 65536;

class archive : public res::asset {
	friend class res::asset;

private:
	std::vector<res::anyref> m_pages;

	explicit archive(res::name name) :
		asset(name) {}

public:
	using ref = res::ref<archive>;

	DEFINE_APROP_GETTER(pages);
	DEFINE_APROP_SETTER(pages);
};

class spage : public res::asset {
	friend class res::asset;

private:
	std::vector<res::anyref> m_pagelets;
	uint16_t m_type = 0;
	uint32_t m_cid = 0;

	explicit spage(res::name name) :
		asset(name) {}

public:
	using ref = res::ref<spage>;

	DEFINE_APROP_GETTER(pagelets);
	DEFINE_APROP_SETTER(pagelets);

	DEFINE_APROP_GETTER(type);
	DEFINE_APROP_SETTER(type);

	DEFINE_APROP_GETTER(cid);
	DEFINE_APROP_SETTER(cid);
	
	void import_file(TRANSACT,const std::vector<unsigned char> &data);
};

class entry : public res::asset {
private:
	std::uint32_t m_eid = 0;

protected:
	explicit entry(res::name name) :
		asset(name) {}

public:
	using ref = res::ref<entry>;

	DEFINE_APROP_GETTER(eid);
	DEFINE_APROP_SETTER(eid);
};

using raw_item = std::vector<unsigned char>;
using raw_item_list = std::vector<raw_item>;

class raw_entry : public entry {
	friend class res::asset;

private:
	raw_item_list m_items;
	uint32_t m_type = 0;

	explicit raw_entry(res::name name) :
		entry(name) {}

public:
	using ref = res::ref<raw_entry>;

	DEFINE_APROP_GETTER(items);
	DEFINE_APROP_SETTER(items);

	DEFINE_APROP_GETTER(type);
	DEFINE_APROP_SETTER(type);

	void import_file(TRANSACT,const std::vector<unsigned char> &data);

	template <typename T>
	void process_as(TRANSACT,res::name::space &ns)
	{
		assert_alive();

		// Create the output entry under a reserved name,
		// '_PROCESS_OUTPUT'.
		res::ref<T> result(ns,"_PROCESS_OUTPUT");
		result.create(ts);

		// Process the raw items into the output entry.
		result->set_eid(ts,get_eid());
		result->import_entry(ts,get_items(),ns);

		// Delete the raw entry (this) and rename the output entry over
		// this one's name.
		auto name = get_name();
		destroy(ts);
		result->rename(ts,name);
	}

	void run_gui() override;
};

class wgeo_v2 : public entry {
	friend class res::asset;

private:
	raw_item m_item0;
	raw_item m_item4;
	raw_item m_item6;

	gfx::model::ref m_model;

	explicit wgeo_v2(res::name name) :
		entry(name) {}

public:
	using ref = res::ref<wgeo_v2>;

	DEFINE_APROP_GETTER(item0);
	DEFINE_APROP_SETTER(item0);

	DEFINE_APROP_GETTER(item4);
	DEFINE_APROP_SETTER(item4);

	DEFINE_APROP_GETTER(item6);
	DEFINE_APROP_SETTER(item6);

	DEFINE_APROP_GETTER(model);
	DEFINE_APROP_SETTER(model);

	void import_entry(TRANSACT,const raw_item_list &items,res::name::space &ns); // FIXME 80 col
};

}
