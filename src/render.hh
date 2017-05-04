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

/*
 * render.hh
 *
 * FIXME explain
 */

#include "res.hh"
#include "gfx.hh"
#include "gui.hh"

namespace drnsf {
namespace render {

/*
 * render::camera
 *
 * A camera configuration for use in viewport widgets. Note that this is not
 * some kind of in-game camera structure, only a camera for rendering inside
 * the application.
 */
struct camera {
	static constexpr float default_yaw = 30.0f;
	static constexpr float default_pitch = 30.0f;
	static constexpr float default_zoom = 5000.0f;

	float yaw = default_yaw;
	float pitch = default_pitch;
	float zoom = default_zoom;;
};

/*
 * render::viewport
 *
 * FIXME explain
 */
class viewport : private util::nocopy {
private:
	// inner class defined in render_viewport.cc
	class impl;

	// (var) M
	// The pointer to the internal implementation object (PIMPL).
	impl *M;

public:
	// (explicit ctor)
	// Constructs an empty viewport widget and places it in the given
	// parent container.
	explicit viewport(gui::container &parent);

	// (dtor)
	// Destroys the widget, removing it from the parent container.
	~viewport();

	// (func) show
	// Shows the widget.
	void show();
};

}
}
