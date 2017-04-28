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

namespace drnsf {
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
	explicit frame(res::project &proj) :
		asset(proj) {}

public:
	using ref = res::ref<frame>;

	DEFINE_APROP(vertices,std::vector<vertex>);

	template <typename Reflector>
	void reflect(Reflector &rfl)
	{
		asset::reflect(rfl);
		rfl.field(p_vertices,"Vertices");
	}
};

class anim : public res::asset {
	friend class res::asset;

private:
	explicit anim(res::project &proj) :
		asset(proj) {}

public:
	using ref = res::ref<anim>;

	DEFINE_APROP(frames,std::vector<frame::ref>);

	template <typename Reflector>
	void reflect(Reflector &rfl)
	{
		asset::reflect(rfl);
		rfl.field(p_frames,"Frames");
	}
};

struct poly_vert {
	int vertex_index;
	int color_index;
};

using triangle = std::array<poly_vert,3>;
using quad = std::array<poly_vert,4>;

class mesh : public res::asset {
	friend class res::asset;

private:
	explicit mesh(res::project &proj) :
		asset(proj) {}

public:
	using ref = res::ref<mesh>;

	DEFINE_APROP(triangles,std::vector<triangle>);
	DEFINE_APROP(quads,std::vector<quad>);
	DEFINE_APROP(colors,std::vector<color>);

	template <typename Reflector>
	void reflect(Reflector &rfl)
	{
		asset::reflect(rfl);
		rfl.field(p_triangles,"Triangles");
		rfl.field(p_quads,"Quads");
		rfl.field(p_colors,"Colors");
	}
};

class model : public res::asset {
	friend class res::asset;

private:
	explicit model(res::project &proj) :
		asset(proj) {}

public:
	using ref = res::ref<model>;

	DEFINE_APROP(anim,anim::ref);
	DEFINE_APROP(mesh,mesh::ref);
	DEFINE_APROP(scene_x,double,0.0);
	DEFINE_APROP(scene_y,double,0.0);
	DEFINE_APROP(scene_z,double,0.0);

	template <typename Reflector>
	void reflect(Reflector &rfl)
	{
		asset::reflect(rfl);
		rfl.field(p_anim,"Animation");
		rfl.field(p_mesh,"Mesh");
		rfl.field(p_scene_x,"Scene X");
		rfl.field(p_scene_y,"Scene Y");
		rfl.field(p_scene_z,"Scene Z");
	}
};

}
}
