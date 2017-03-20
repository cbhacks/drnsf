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

static std::set<modedef *> s_modedef_list;

modedef::modedef(std::string title) :
	m_title(title)
{
	s_modedef_list.insert(this);
}

modedef::~modedef()
{
	s_modedef_list.erase(this);
}

class mode_example : public mode {
private:
	project &m_proj;

public:
	explicit mode_example(project &proj);

	void show_gui() override;
};

static modedef_of<mode_example> g_mode_example_def("Example");

mode_example::mode_example(project &proj) :
	m_proj(proj)
{
}

void mode_example::show_gui()
{
	namespace im = gui::im;

	im::Button("slewpy");
}

const std::set<modedef*> &modedef::get_list()
{
	return s_modedef_list;
}

}
