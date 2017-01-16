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

#pragma once

#include "common.hh"
#include "edit.hh"
#include "gui.hh"

#define BEGIN_MODULE(modname) \
	namespace { namespace mod_##modname { \
	class mod : public edit::module { \
	public: \
		mod(edit::core &core) : \
			module(core) {} \
\
		constexpr static const char *name = #modname;

#define END_MODULE \
	private: \
	}; \
\
	edit::module_info_impl<mod> info; \
	} }

#define BEGIN_TRANSACTION \
	nx << [&](TRANSACT) {

#define END_TRANSACTION \
	};

#define BEGIN_MODE(id,title) \
	edit::mode id = {*this,title,[this]() {

#define END_MODE }};

#define BEGIN_PANEL(id,title) \
	edit::panel id = {*this,title,[this]() {

#define END_PANEL }};
