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
#include "res.hh"

namespace drnsf {
namespace res {

class asset_move_event_op : public transact::operation {
private:
	project &m_proj;
	atom m_src;
	atom m_dest;

public:
	explicit asset_move_event_op(project &proj,atom src,atom dest) :
		m_proj(proj),
		m_src(src),
		m_dest(dest) {}

	void execute() noexcept override
	{
		m_proj.on_asset_move(m_src,m_dest);
		std::swap(m_src,m_dest);
	}
};

void asset::create_impl(TRANSACT,atom name)
{
	auto p = std::unique_ptr<asset>(this);
	TS.set(m_name,name);
	TS.set(name.get_internal_asset_ptr(),this);
	m_iter = TS.insert(
		m_proj.m_assets,
		m_proj.m_assets.end(),
		std::move(p)
	);
	TS.push_op(std::make_unique<asset_move_event_op>(
		m_proj,
		nullptr,
		m_name
	));
}

void asset::assert_alive() const
{
	if (!m_name) {
		throw 0; // FIXME
	}
}

void asset::rename(TRANSACT,atom name)
{
	assert_alive();

	if (!name) {
		throw 0; // FIXME
	}

	if (name.get()) {
		throw 0;//FIXME
	}

	auto old_name = m_name;

	TS.set(name.get_internal_asset_ptr(),this);
	TS.set(m_name.get_internal_asset_ptr(),nullptr);
	TS.set(m_name,name);
	TS.push_op(std::make_unique<asset_move_event_op>(
		m_proj,
		old_name,
		name
	));
}

void asset::destroy(TRANSACT)
{
	assert_alive();

	TS.push_op(std::make_unique<asset_move_event_op>(
		m_proj,
		m_name,
		nullptr
	));
	TS.set(m_name.get_internal_asset_ptr(),nullptr);
	TS.set(m_name,nullptr);
	TS.erase(m_proj.m_assets,m_iter);
}

const atom &asset::get_name() const
{
	assert_alive();

	return m_name;
}

project &asset::get_proj() const
{
	assert_alive();

	return m_proj;
}

}
}
