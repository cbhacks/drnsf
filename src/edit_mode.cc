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

const std::set<modedef*> &modedef::get_list()
{
	return s_modedef_list;
}


/* *** EXAMPLE CODE BELOW *** FIXME */
namespace {

class example_pane : public pane {
public:
	explicit example_pane(editor &ed,std::string id) :
		pane(ed,id) {}

	void show() override
	{
		namespace im = gui::im;

		auto &&proj = m_ed.get_project();
		for (auto &&name : proj.get_asset_ns().get_asset_names()) {
			im::TextUnformatted(name.c_str());
		}
	}

	std::string get_title() const override
	{
		return "Example pane";
	}
};

}

class mode_example : public mode {
private:
	editor &m_ed;
	example_pane m_pane;
	example_pane m_pane2;

public:
	explicit mode_example(editor &ed) :
		m_ed(ed),
		m_pane(ed,"example"),
		m_pane2(ed,"example2") {}
};

static modedef_of<mode_example> g_mode_example_def("Example");

/* *** END EXAMPLE *** FIXME */

}
