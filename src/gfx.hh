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

#include <vector>
#include "res.hh"

namespace gfx {

struct vertex {
	union {
		struct {
			float x;
			float y;
			float z;
		};
		float v[3];
	};
};

struct color {
	union {
		struct {
			unsigned char r;
			unsigned char g;
			unsigned char b;
		};
		unsigned char v[3];
	};
};

class frame : public res::asset {
	friend class res::asset;

private:
	std::vector<vertex> m_vertices;

	explicit frame(res::name name) :
		asset(name) {}

public:
	using ref = res::ref<frame>;

	DEFINE_APROP(vertices);
};

class anim : public res::asset {
	friend class res::asset;

private:
	std::vector<frame::ref> m_frames;

	explicit anim(res::name name) :
		asset(name) {}

public:
	using ref = res::ref<anim>;

	DEFINE_APROP(frames);
};

enum class polytype {
	tri,
	quad,
};

struct poly {
	polytype type;
	int vertices[4];
	int colors[4];
};

class mesh : public res::asset {
	friend class res::asset;

private:
	std::vector<poly> m_polys;
	std::vector<color> m_colors;

	explicit mesh(res::name name) :
		asset(name) {}

public:
	using ref = res::ref<mesh>;

	DEFINE_APROP(polys);
	DEFINE_APROP(colors);
};

class model : public res::asset {
	friend class res::asset;

private:
	anim::ref m_anim;
	mesh::ref m_mesh;

	double m_scene_x = 0.0;
	double m_scene_y = 0.0;
	double m_scene_z = 0.0;

	explicit model(res::name name) :
		asset(name) {}

public:
	using ref = res::ref<model>;

	DEFINE_APROP(anim);
	DEFINE_APROP(mesh);
	DEFINE_APROP(scene_x);
	DEFINE_APROP(scene_y);
	DEFINE_APROP(scene_z);
};

}
