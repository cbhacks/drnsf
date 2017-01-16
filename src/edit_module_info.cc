//
// WILLYMOD - An unofficial Crash Bandicoot level editor
// Copyright (C) 2016  WILLYMOD project contributors
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
#include "edit.hh"

namespace edit {

std::unique_ptr<module_info::set> module_info::m_set;

module_info::module_info()
{
	// Create the module set if it doesn't exist yet.
	if (!m_set) {
		m_set = std::make_unique<set>();
	}

	m_set->insert(this);
}

module_info::~module_info()
{
	m_set->erase(this);
}

const module_info::set &module_info::get_set()
{
	// Create the module set if it doesn't exist yet.
	if (!m_set) {
		m_set = std::make_unique<set>();
	}

	return *m_set;
}

}
