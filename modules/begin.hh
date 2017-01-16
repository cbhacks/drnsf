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

#include "module.hh"
#include <GL/gl.h>
#include "edit.hh"
#include "gui.hh"

#define DEFINE_SHARED(type,var) type &var = share<type>(#var)
#define DEFINE_SHARED_V(type,var,...) type &var = share<type>(#var,__VA_ARGS__)

#define HOOK_EVENT(name) \
	struct ev_handler_class__##name { \
		ev_handler_class__##name(mod &m) \
		{ \
			m.hook(#name,[&m](){ m.on_##name(); }); \
		} \
	} ev_handler_class_inst__##name{*this}

BEGIN_MODULE(muh_module)
