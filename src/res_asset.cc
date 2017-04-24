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

class asset_appear_event_op : public transact::operation {
private:
	project &m_proj;
	asset &m_asset;
	bool m_appear;

public:
	explicit asset_appear_event_op(project &proj,asset &asset,bool appear) :
		m_proj(proj),
		m_asset(asset),
		m_appear(appear) {}

	void execute() noexcept override
	{
		if (m_appear) {
			m_proj.on_asset_appear(m_asset);
		} else {
			m_proj.on_asset_disappear(m_asset);
		}
		m_appear = !m_appear;
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
	TS.push_op(std::make_unique<asset_appear_event_op>(
		m_proj,
		*this,
		true
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

	TS.push_op(std::make_unique<asset_appear_event_op>(
		m_proj,
		*this,
		false
	));
	TS.set(name.get_internal_asset_ptr(),this);
	TS.set(m_name.get_internal_asset_ptr(),nullptr);
	TS.set(m_name,name);
	TS.push_op(std::make_unique<asset_appear_event_op>(
		m_proj,
		*this,
		true
	));
}

void asset::destroy(TRANSACT)
{
	assert_alive();

	TS.push_op(std::make_unique<asset_appear_event_op>(
		m_proj,
		*this,
		false
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
