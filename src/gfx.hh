//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2020  DRNSF contributors
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
 * gfx::rgb888
 *
 * FIXME explain
 */
struct rgb888 {
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
 * gfx::vertex
 *
 * FIXME explain
 */
struct vertex {
    union {
        struct {
            int x;
            int y;
            int z;
        };
        int v[3];
    };
    int fx;
    union {
        int color_index;
        rgb888 color;
    };
};

/*
 * gfx::texinfo
 *
 * FIXME explain
 */
struct texinfo {
    unsigned char type:1;
    unsigned char semi_trans:2;
    unsigned char y_offs:5;
    rgb888 color;
    unsigned int color_mode:2;
    unsigned int segment:2;
    unsigned int x_offs:5;
    unsigned int region_index:10;
    unsigned int clut_x:4;
    unsigned int clut_y:7;
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

    // (prop) x_scale, y_scale, z_scale
    // FIXME explain
    DEFINE_APROP(x_scale, float, 1.0f);
    DEFINE_APROP(y_scale, float, 1.0f);
    DEFINE_APROP(z_scale, float, 1.0f);

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
    unsigned int tpag_index;
    unsigned int tinf_index;
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
    unsigned int tpag_index;
    unsigned int tinf_index;
    unsigned int unk0;
    unsigned int unk1;
};

/*
 * gfx::texture
 *
 * FIXME explain
 */
class texture : public res::asset {
    friend class res::asset;    

private:
    // (explicit ctor)
    // FIXME explain
    explicit texture(res::project &proj) :
        asset(proj) {}

public:
    // (typedef) ref
    // FIXME explain
    using ref = res::ref<texture>;

    // (prop) texels
    // FIXME explain
    DEFINE_APROP(texels, util::blob);

#if USE_GL
    // (var) m_texture
    // FIXME explain
    gl::texture m_texture;
#endif
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
    DEFINE_APROP(colors, std::vector<rgb888>);

    // (prop) texinfos
    // FIXME explain
    DEFINE_APROP(texinfos, std::vector<texinfo>);

    // (prop) textures
    // FIXME explain
    DEFINE_APROP(textures, std::vector<texture::ref>);

#if USE_GL
    // (var) m_triangles_buffer, m_quads_buffer, m_colors_buffer, m_texinfos_buffer
    // GL buffer objects containing the data for p_triangles, p_quads, and
    // p_colors. The gfx::mesh object will clear the `ok' flags on each buffer
    // when its respective property is changed, however it is up to users of
    // this data to reupload it to the buffer object themselves when necessary.
    gl::buffer m_triangles_buffer;
    gl::buffer m_quads_buffer;
    gl::buffer m_colors_buffer;
    gl::buffer m_texinfos_buffer;

    // (var) m_triangles_va, m_quads_va
    // GL vertex array objects which should be configured for the triangles and
    // quads buffers above. gfx::mesh does not set up this connection or manage
    // these objects in any way. Users should check the `ok' flags and perform
    // any initialization necessary.
    gl::vert_array m_triangles_va;
    gl::vert_array m_quads_va;

    // (var) m_colors_texture, m_texinfos_texture
    // GL texture objects which should be GL_TEXTURE_BUFFERs pointing to the
    // colors and texinfos buffers. gfx::mesh does not set up this connection 
    // or manage these objects in any way. Users should check the `ok' flag and 
    // set it up if necessary.
    gl::texture m_colors_texture;
    gl::texture m_texinfos_texture;

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
    DEFINE_APROP(x, int, 0);
    DEFINE_APROP(y, int, 0);
    DEFINE_APROP(z, int, 0);
};

}

namespace reflect {

// reflection info for gfx::frame
template <>
struct asset_type_info<gfx::frame> {
    using base_type = res::asset;

    static constexpr const char *name = "gfx::frame";
    static constexpr int prop_count = 4;
};
template <>
struct asset_prop_info<gfx::frame, 0> {
    using type = std::vector<gfx::vertex>;

    static constexpr const char *name = "vertices";
    static constexpr auto ptr = &gfx::frame::p_vertices;
};
template <>
struct asset_prop_info<gfx::frame, 1> {
    using type = float;

    static constexpr const char *name = "x_scale";
    static constexpr auto ptr = &gfx::frame::p_x_scale;
};
template <>
struct asset_prop_info<gfx::frame, 2> {
    using type = float;

    static constexpr const char *name = "y_scale";
    static constexpr auto ptr = &gfx::frame::p_y_scale;
};
template <>
struct asset_prop_info<gfx::frame, 3> {
    using type = float;

    static constexpr const char *name = "z_scale";
    static constexpr auto ptr = &gfx::frame::p_z_scale;
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
    static constexpr int prop_count = 5;
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
    using type = std::vector<gfx::rgb888>;

    static constexpr const char *name = "colors";
    static constexpr auto ptr = &gfx::mesh::p_colors;
};
template <>
struct asset_prop_info<gfx::mesh, 3> {
    using type = std::vector<gfx::texinfo>;

    static constexpr const char *name = "texinfos";
    static constexpr auto ptr = &gfx::mesh::p_texinfos;
};
template <>
struct asset_prop_info<gfx::mesh, 4> {
    using type = std::vector<gfx::texture::ref>;

    static constexpr const char *name = "textures";
    static constexpr auto ptr = &gfx::mesh::p_textures;
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

// reflection info for gfx::texture
template <>
struct asset_type_info<gfx::texture> {
    using base_type = res::asset;

    static constexpr const char *name = "gfx::texture";
    static constexpr int prop_count = 1;
};
template <>
struct asset_prop_info<gfx::texture, 0> {
    using type = util::blob;

    static constexpr const char *name = "texels";
    static constexpr auto ptr = &gfx::texture::p_texels;
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
    using type = int;

    static constexpr const char *name = "x";
    static constexpr auto ptr = &gfx::world::p_x;
};
template <>
struct asset_prop_info<gfx::world, 2> {
    using type = int;

    static constexpr const char *name = "y";
    static constexpr auto ptr = &gfx::world::p_y;
};
template <>
struct asset_prop_info<gfx::world, 3> {
    using type = int;

    static constexpr const char *name = "z";
    static constexpr auto ptr = &gfx::world::p_z;
};

}
}
