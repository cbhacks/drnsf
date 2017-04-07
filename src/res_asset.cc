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

	TS.push_op([this,name]{
		using std::swap;
		swap(m_name.get_internal_asset_ptr(),name.get_internal_asset_ptr());
	});
	TS.set(m_name,name);
}

void asset::destroy(TRANSACT)
{
	assert_alive();

	TS.set(m_name.get_internal_asset_ptr(),nullptr);
	TS.set(m_name,nullptr);
}

const atom &asset::get_name() const
{
	assert_alive();

	return m_name;
}

}
}
