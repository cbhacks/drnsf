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

pane::pane(editor &ed,std::string id) :
	m_id(id),
	m_ed(ed)
{
	m_ed.m_panes.push_front(this);
	m_iter = m_ed.m_panes.begin();
}

pane::~pane()
{
	m_ed.m_panes.erase(m_iter);
}

const std::string &pane::get_id() const
{
	return m_id;
}

}
