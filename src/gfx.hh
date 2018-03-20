//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2018  DRNSF contributors
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
 * gfx.hh
 *
 * FIXME explain
 */

#include <vector>
#include "res.hh"

namespace drnsf {
namespace gfx {

/*
 * gfx::vertex
 *
 * FIXME explain
 */
struct vertex {
    union {
        struct {
            float x;
            float y;
            float z;
        };
        float v[3];
    };
    unsigned int fx;
    int color_index;
};

/*
 * gfx::color
 *
 * FIXME explain
 */
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

/*
 * gfx::frame
 *
 * FIXME explain
 */
class frame : public res::asset {
    friend class res::asset;

private:
    // (explicit ctor)
    // FIXME explain
    explicit frame(res::project &proj) :
        asset(proj) {}

public:
    // (typedef) ref
    // FIXME explain
    using ref = res::ref<frame>;

    // (prop) vertices
    // FIXME explain
    DEFINE_APROP(vertices, std::vector<vertex>);

    // FIXME obsolete
    template <typename Reflector>
    void reflect(Reflector &rfl)
    {
        asset::reflect(rfl);
        rfl.field(p_vertices, "Vertices");
    }
};

/*
 * gfx::anim
 *
 * FIXME explain
 */
class anim : public res::asset {
    friend class res::asset;

private:
    // (explicit ctor)
    // FIXME explain
    explicit anim(res::project &proj) :
        asset(proj) {}

public:
    // (typedef) ref
    // FIXME explain
    using ref = res::ref<anim>;

    // (prop) frames
    // FIXME explain
    DEFINE_APROP(frames, std::vector<frame::ref>);

    // FIXME obsolete
    template <typename Reflector>
    void reflect(Reflector &rfl)
    {
        asset::reflect(rfl);
        rfl.field(p_frames, "Frames");
    }
};

/*
 * gfx::corner
 *
 * FIXME explain
 */
struct corner {
    int vertex_index;
    int color_index;
};

/*
 * gfx::triangle
 *
 * FIXME explain
 */
struct triangle {
    corner v[3];
    unsigned int unk0;
    unsigned int unk1;
};

/*
 * gfx::quad
 *
 * FIXME explain
 */
struct quad {
    corner v[4];
    unsigned int unk0;
    unsigned int unk1;
};

/*
 * gfx::mesh
 *
 * FIXME explain
 */
class mesh : public res::asset {
    friend class res::asset;

private:
    // (explicit ctor)
    // FIXME explain
    explicit mesh(res::project &proj) :
        asset(proj) {}

public:
    // (typedef) ref
    // FIXME explain
    using ref = res::ref<mesh>;

    // (prop) triangles
    // FIXME explain
    DEFINE_APROP(triangles, std::vector<triangle>);

    // (prop) quads
    // FIXME explain
    DEFINE_APROP(quads, std::vector<quad>);

    // (prop) colors
    // FIXME explain
    DEFINE_APROP(colors, std::vector<color>);

    // FIXME obsolete
    template <typename Reflector>
    void reflect(Reflector &rfl)
    {
        asset::reflect(rfl);
        rfl.field(p_triangles, "Triangles");
        rfl.field(p_quads, "Quads");
        rfl.field(p_colors, "Colors");
    }
};

/*
 * gfx::model
 *
 * FIXME explain
 */
class model : public res::asset {
    friend class res::asset;

private:
    // (explicit ctor)
    // FIXME explain
    explicit model(res::project &proj) :
        asset(proj) {}

public:
    // (typedef) ref
    // FIXME explain
    using ref = res::ref<model>;

    // (prop) anim
    // FIXME explain
    DEFINE_APROP(anim, anim::ref);

    // (prop) mesh
    // FIXME explain
    DEFINE_APROP(mesh, mesh::ref);

    // FIXME obsolete
    template <typename Reflector>
    void reflect(Reflector &rfl)
    {
        asset::reflect(rfl);
        rfl.field(p_anim, "Animation");
        rfl.field(p_mesh, "Mesh");
    }
};

/*
 * gfx::world
 *
 * FIXME explain
 */
class world : public res::asset {
    friend class res::asset;

private:
    // (explicit ctor)
    // FIXME explain
    explicit world(res::project &proj) :
        asset(proj) {}

public:
    // (typedef) ref
    // FIXME explain
    using ref = res::ref<world>;

    // (prop) model
    // FIXME explain
    DEFINE_APROP(model, model::ref);

    // (prop) x, y, z
    // FIXME explain
    DEFINE_APROP(x, double, 0.0);
    DEFINE_APROP(y, double, 0.0);
    DEFINE_APROP(z, double, 0.0);
};

}

namespace reflect {

// reflection info for gfx::frame
template <>
struct asset_type_info<gfx::frame> {
    using base_type = res::asset;

    static constexpr const char *name = "gfx::frame";
};

// reflection info for gfx::anim
template <>
struct asset_type_info<gfx::anim> {
    using base_type = res::asset;

    static constexpr const char *name = "gfx::anim";
};

// reflection info for gfx::mesh
template <>
struct asset_type_info<gfx::mesh> {
    using base_type = res::asset;

    static constexpr const char *name = "gfx::mesh";
};

// reflection info for gfx::model
template <>
struct asset_type_info<gfx::model> {
    using base_type = res::asset;

    static constexpr const char *name = "gfx::model";
};

// reflection info for gfx::world
template <>
struct asset_type_info<gfx::world> {
    using base_type = res::asset;

    static constexpr const char *name = "gfx::world";
};

}
}
