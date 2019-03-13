//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2019  DRNSF contributors
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

#if USE_GL
#include "gl.hh"
#endif

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
    int fx;
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

#if USE_GL
    // (var) m_vertices_buffer
    // A GL buffer object containing the data for p_vertices. The gfx::frame
    // object will clear the `ok' flag on the buffer when the property is
    // changed, however it is up to users of this data to reupload to the
    // buffer object themselves when necessary.
    gl::buffer m_vertices_buffer;

    // (var) m_vertices_texture
    // A GL texture object which should be a GL_TEXTURE_BUFFER pointing to the
    // vertices buffer. gfx::frame does not set up this connection or manage
    // this object in any way. Users should check the `ok' flag and set it up if
    // necessary.
    gl::texture m_vertices_texture;

protected:
    // (func) on_prop_change
    // Reacts to property changes to invalidate any necessary GL objects.
    void on_prop_change(void *prop) noexcept override
    {
        if (prop == &p_vertices) {
            m_vertices_buffer.ok = false;
        }
    }
#endif
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

#if USE_GL
    // (var) m_triangles_buffer, m_quads_buffer, m_colors_buffer
    // GL buffer objects containing the data for p_triangles, p_quads, and
    // p_colors. The gfx::mesh object will clear the `ok' flags on each buffer
    // when its respective property is changed, however it is up to users of
    // this data to reupload it to the buffer object themselves when necessary.
    gl::buffer m_triangles_buffer;
    gl::buffer m_quads_buffer;
    gl::buffer m_colors_buffer;

    // (var) m_triangles_va, m_quads_va
    // GL vertex array objects which should be configured for the triangles and
    // quads buffers above. gfx::mesh does not set up this connection or manage
    // these objects in any way. Users should check the `ok' flags and perform
    // any initialization necessary.
    gl::vert_array m_triangles_va;
    gl::vert_array m_quads_va;

    // (var) m_colors_texture
    // A GL texture object which should be a GL_TEXTURE_BUFFER pointing to the
    // colors buffer. gfx::mesh does not set up this connection or manage this
    // object in any way. Users should check the `ok' flag and set it up if
    // necessary.
    gl::texture m_colors_texture;

protected:
    // (func) on_prop_change
    // Reacts to property changes to invalidate any necessary GL objects.
    void on_prop_change(void *prop) noexcept override
    {
        if (prop == &p_triangles) {
            m_triangles_buffer.ok = false;
        } else if (prop == &p_quads) {
            m_quads_buffer.ok = false;
        } else if (prop == &p_colors) {
            m_colors_buffer.ok = false;
        }
    }
#endif
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
    static constexpr int prop_count = 1;
};
template <>
struct asset_prop_info<gfx::frame, 0> {
    using type = std::vector<gfx::vertex>;

    static constexpr const char *name = "vertices";
    static constexpr auto ptr = &gfx::frame::p_vertices;
};

// reflection info for gfx::anim
template <>
struct asset_type_info<gfx::anim> {
    using base_type = res::asset;

    static constexpr const char *name = "gfx::anim";
    static constexpr int prop_count = 1;
};
template <>
struct asset_prop_info<gfx::anim, 0> {
    using type = std::vector<gfx::frame::ref>;

    static constexpr const char *name = "frames";
    static constexpr auto ptr = &gfx::anim::p_frames;
};

// reflection info for gfx::mesh
template <>
struct asset_type_info<gfx::mesh> {
    using base_type = res::asset;

    static constexpr const char *name = "gfx::mesh";
    static constexpr int prop_count = 3;
};
template <>
struct asset_prop_info<gfx::mesh, 0> {
    using type = std::vector<gfx::triangle>;

    static constexpr const char *name = "triangles";
    static constexpr auto ptr = &gfx::mesh::p_triangles;
};
template <>
struct asset_prop_info<gfx::mesh, 1> {
    using type = std::vector<gfx::quad>;

    static constexpr const char *name = "quads";
    static constexpr auto ptr = &gfx::mesh::p_quads;
};
template <>
struct asset_prop_info<gfx::mesh, 2> {
    using type = std::vector<gfx::color>;

    static constexpr const char *name = "colors";
    static constexpr auto ptr = &gfx::mesh::p_colors;
};

// reflection info for gfx::model
template <>
struct asset_type_info<gfx::model> {
    using base_type = res::asset;

    static constexpr const char *name = "gfx::model";
    static constexpr int prop_count = 2;
};
template <>
struct asset_prop_info<gfx::model, 0> {
    using type = gfx::anim::ref;

    static constexpr const char *name = "anim";
    static constexpr auto ptr = &gfx::model::p_anim;
};
template <>
struct asset_prop_info<gfx::model, 1> {
    using type = gfx::mesh::ref;

    static constexpr const char *name = "mesh";
    static constexpr auto ptr = &gfx::model::p_mesh;
};

// reflection info for gfx::world
template <>
struct asset_type_info<gfx::world> {
    using base_type = res::asset;

    static constexpr const char *name = "gfx::world";
    static constexpr int prop_count = 4;
};
template <>
struct asset_prop_info<gfx::world, 0> {
    using type = gfx::model::ref;

    static constexpr const char *name = "model";
    static constexpr auto ptr = &gfx::world::p_model;
};
template <>
struct asset_prop_info<gfx::world, 1> {
    using type = double;

    static constexpr const char *name = "x";
    static constexpr auto ptr = &gfx::world::p_x;
};
template <>
struct asset_prop_info<gfx::world, 2> {
    using type = double;

    static constexpr const char *name = "y";
    static constexpr auto ptr = &gfx::world::p_y;
};
template <>
struct asset_prop_info<gfx::world, 3> {
    using type = double;

    static constexpr const char *name = "z";
    static constexpr auto ptr = &gfx::world::p_z;
};

}
}
