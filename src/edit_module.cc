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
#include "edit.hh"

namespace edit {

module::module(core &core) :
	m_core(core)
{
}

void module::hook(std::string name,std::function<void()> f)
{
	m_hooks.insert(
		std::pair<std::string,std::function<void()>>(name,f)
	);
}

void module::raise(std::string name)
{
	for (auto &&kv : m_core.m_modules) {
		auto &&mod = kv.second;
		auto it = mod->m_hooks.find(name);
		if (it == mod->m_hooks.end())
			continue;
		it->second();
	}
}

cam &module::cam()
{
	return m_core.m_cam;
}

const std::list<panel *> &module::get_panels() const
{
	return m_panels;
}

}
