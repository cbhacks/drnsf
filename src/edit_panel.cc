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

panel::panel(module &mod,std::string title,panel::func_type func) :
	m_mod(mod),
	m_title(title),
	m_func(func)
{
	m_mod.m_panels.push_back(this);
}

panel::~panel()
{
	m_mod.m_panels.remove(this);
}

void panel::show() const
{
	m_func();
}

const std::string &panel::get_title() const
{
	return m_title;
}

}
