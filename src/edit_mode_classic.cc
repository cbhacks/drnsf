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

namespace {

class tree_pane : public pane {
public:
	explicit tree_pane(editor &ed) :
		pane(ed,"classic_tree") {}

	void show() override;

	std::string get_title() const override;
};

class asset_pane : public pane {
public:
	explicit asset_pane(editor &ed) :
		pane(ed,"classic_asset") {}

	void show() override;

	std::string get_title() const override;
};

}

class mode_classic : public mode {
private:
	editor &m_ed;
	tree_pane m_tree;
	asset_pane m_asset;

public:
	explicit mode_classic(editor &ed) :
		m_ed(ed),
		m_tree(ed),
		m_asset(ed) {}
};

static modedef_of<mode_classic> g_mode_classic_def("Classic");

void tree_pane::show()
{
	namespace im = gui::im;
	auto &&proj = m_ed.get_project();

	for (auto &&name : proj.get_asset_ns().get_asset_names()) {
		im::TextUnformatted(name.c_str());
	}
}

std::string tree_pane::get_title() const
{
	return "Asset Tree";
}

void asset_pane::show()
{
	namespace im = gui::im;

	im::Text("Nothing to see here.");
}

std::string asset_pane::get_title() const
{
	return "Asset: [No selection]";
}

}
