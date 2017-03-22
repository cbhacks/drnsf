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

namespace misc {

class raw_data : public res::asset {
	friend class res::asset;

private:
	std::vector<unsigned char> m_data;

	explicit raw_data(res::name name) :
		asset(name) {}

public:
	using ref = res::ref<raw_data>;

	DEFINE_APROP(data);

	template <typename Reflector>
	void reflect(Reflector &rfl)
	{
		asset::reflect(rfl);
		rfl.property(p_data,"Data");
	}
};

}
