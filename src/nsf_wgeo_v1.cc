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

#include "common.hh"
#include "nsf.hh"

namespace drnsf {
namespace nsf {

// declared in res.hh
void wgeo_v1::import_entry(TRANSACT, const std::vector<util::blob> &items)
{
    assert_alive();

    util::binreader r;

    // Ensure we have the correct number of items (3).
    if (items.size() != 3)
        throw res::import_error("nsf::wgeo_v1: wrong item count");

    auto &item_info      = items[0];
    auto &item_triangles = items[1];
    auto &item_vertices  = items[2];

    // Parse the info item (0).
    r.begin(item_info);
    auto world_x        = r.read_s32();
    auto world_z        = r.read_s32();
    auto world_y        = r.read_s32();
    auto triangle_count = r.read_u32();
    auto vertex_count   = r.read_u32();
    auto texinfo_count  = r.read_u32();
    auto tpag_ref_count = r.read_u32();
    auto is_backdrop    = r.read_u32();

    uint32_t tpag_refs[8];
    for (int i = 0; i < 8; i++) {
        tpag_refs[i] = r.read_u32();
    }
    r.end_early();

    std::vector<gfx::texinfo> texinfos(texinfo_count);
    for (auto &&i : util::range_of(texinfos)) {
        auto &texinfo = texinfos[i];
        
        r.begin(&item_info[0x40 + (i * 4)], 4);
        auto cb         = r.read_ubits(8);
        auto cg         = r.read_ubits(8);
        auto cr         = r.read_ubits(8);
        auto clut_x     = r.read_ubits(4);
        r.discard_bits(1);
        auto semi_trans = r.read_ubits(2);
        auto type       = r.read_ubits(1);
        r.end();

        if (type == 1) {
            if (i == texinfo_count-1) {
                break;
            }
            r.begin(&item_info[0x40 + ((i+1) * 4)], 4);
            auto y_offs       = r.read_ubits(5);
            r.discard_bits(1);
            auto clut_y       = r.read_ubits(7);
            auto x_offs       = r.read_ubits(5);
            auto segment      = r.read_ubits(2);
            auto color_mode   = r.read_ubits(2);
            auto region_index = r.read_ubits(10);
            r.end();

            texinfo.color_mode   = color_mode;
            texinfo.segment      = segment;
            texinfo.x_offs       = x_offs;
            texinfo.y_offs       = y_offs;
            texinfo.region_index = region_index;
            texinfo.clut_x       = clut_x;
            texinfo.clut_y       = clut_y;
            texinfo.semi_trans   = semi_trans;
        }
        texinfo.type    = type;
        texinfo.color.r = cr;
        texinfo.color.g = cg;
        texinfo.color.b = cb;
    }

    // Ensure the triangle count is correct.
    if (triangle_count != item_triangles.size() / 8)
        throw res::import_error("nsf::wgeo_v1: bad triangle item size");

    // Parse the triangles.
    std::vector<gfx::triangle> triangles(triangle_count);
    for (auto &&i : util::range_of(triangles)) {
        auto &triangle = triangles[i];

        // TODO - explain triangle format here

        r.begin(&item_triangles[i * 8], 8);
        auto triangle_unk0 = r.read_ubits(5);
        auto tpag_index    = r.read_ubits(3);
        auto tinf_index    = r.read_ubits(12);
        auto vertex0       = r.read_ubits(12);
        auto triangle_unk1 = r.read_ubits(8);
        auto vertex1       = r.read_ubits(12);
        auto vertex2       = r.read_ubits(12);
        r.end();

        triangle.v[0].vertex_index = vertex0;
        triangle.v[1].vertex_index = vertex1;
        triangle.v[2].vertex_index = vertex2;

        triangle.v[0].color_index = -1;
        triangle.v[1].color_index = -1;
        triangle.v[2].color_index = -1;

        triangle.tpag_index = tpag_index;
        triangle.tinf_index = tinf_index;

        triangle.unk0 = triangle_unk0;
        triangle.unk1 = triangle_unk1;
    }

    // Ensure the vertex count is correct.
    if (vertex_count != item_vertices.size() / 8)
        throw res::import_error("nsf::wgeo_v1: bad vertex item size");
    
    // Parse the vertices.
    std::vector<gfx::vertex> vertices(vertex_count);
    for (auto &&i : util::range_of(vertices)) {
        auto &vertex = vertices[i];

        // TODO - explain vertex format here
        r.begin(&item_vertices[i * 8], 8);
        auto cb = r.read_ubits(8);
        auto cg = r.read_ubits(8);
        auto cr = r.read_ubits(8);
        auto y1 = r.read_ubits(8);
        auto fx = r.read_ubits(1);
        auto y2 = r.read_ubits(2);
        auto x  = r.read_sbits(13);
        auto y3 = r.read_sbits(3);
        auto z  = r.read_sbits(13);
        r.end();

        vertex.x = x;
        vertex.y = y1 | (y2 << 8) | (y3 << 10);
        vertex.z = z;

        vertex.fx = fx | 4;

        vertex.color.r = cr;
        vertex.color.g = cg;
        vertex.color.b = cb;
    }

    // Ensure the tpag ref count is viable.
    if (tpag_ref_count > 8)
        throw res::import_error("nsf::wgeo_v1: bad tpag ref count");

    // Parse the tpag references.
    std::vector<gfx::texture::ref> textures(tpag_ref_count);
    for (auto &&i : util::range_of(textures)) {
        auto &texture = textures[i];

        res::atom t_name = get_proj().get_asset_root()
            / "textures"
            / "$"_fmt(eid(tpag_refs[i]));

        texture = t_name;
        if (!texture)
            throw res::import_error("nsf::wgeo_v1: invalid tpag ref");
    }    

    res::atom atom = get_proj().get_asset_root()
        / "scenery"
        / "$"_fmt(get_eid());

    // Create the frame which will contain this scene's vertex positions.
    gfx::frame::ref frame = atom / "frame";
    frame.create(TS, get_proj());
    frame->set_vertices(TS, std::move(vertices));
    frame->set_x_scale(TS, 8.0f);
    frame->set_y_scale(TS, 8.0f);
    frame->set_z_scale(TS, 8.0f);

    // Create the animation for this scene (just one frame, scenes are not
    // vertex-animated).
    gfx::anim::ref anim = atom / "anim";
    anim.create(TS, get_proj());
    anim->set_frames(TS, {frame});

    // Create the mesh for this scene.
    gfx::mesh::ref mesh = atom / "mesh";
    mesh.create(TS, get_proj());
    mesh->set_triangles(TS, std::move(triangles));
    mesh->set_texinfos(TS, std::move(texinfos));
    mesh->set_textures(TS, std::move(textures));

    // Create the model for this scene.
    gfx::model::ref model = atom / "model";
    model.create(TS, get_proj());
    model->set_anim(TS, anim);
    model->set_mesh(TS, mesh);

    // Create the world for this scene.
    gfx::world::ref world = atom;
    world.create(TS, get_proj());
    world->set_model(TS, model);
    world->set_x(TS, world_x);
    world->set_y(TS, world_y);
    world->set_z(TS, world_z);

    // Finish importing.
    set_tpag_ref_count(TS, tpag_ref_count);
    set_tpag_ref0(TS, tpag_refs[0]);
    set_tpag_ref1(TS, tpag_refs[1]);
    set_tpag_ref2(TS, tpag_refs[2]);
    set_tpag_ref3(TS, tpag_refs[3]);
    set_tpag_ref4(TS, tpag_refs[4]);
    set_tpag_ref5(TS, tpag_refs[5]);
    set_tpag_ref6(TS, tpag_refs[6]);
    set_tpag_ref7(TS, tpag_refs[7]);
    set_world(TS, world);
}

// declared in nsf.hh
std::vector<util::blob> wgeo_v1::export_entry(uint32_t &out_type) const
{
    assert_alive();

    util::binwriter w;

    out_type = 3;
    std::vector<util::blob> items(3);

    auto &item_info      = items[0];
    auto &item_triangles = items[1];
    auto &item_vertices  = items[2];

    auto &&world = get_world();
    if (!world.ok())
        throw res::export_error("nsf::wgeo_v1: bad world ref");

    auto &&model = world->get_model();
    if (!model.ok())
        throw res::export_error("nsf::wgeo_v1: bad model ref");

    auto &&mesh = model->get_mesh();
    if (!mesh.ok())
        throw res::export_error("nsf::wgeo_v1: bad mesh ref");

    auto &&anim = model->get_anim();
    if (!anim.ok())
        throw res::export_error("nsf::wgeo_v1: bad anim ref");

    auto &&frames = anim->get_frames();
    if (frames.size() != 1)
        throw res::export_error("nsf::wgeo_v1: invalid frame count");

    auto &&frame = frames[0];
    if (!frame.ok())
        throw res::export_error("nsf::wgeo_v1: bad frame ref");

    // Check the scale to ensure it matches this format.
    if (frame->get_x_scale() != 8.0f ||
        frame->get_y_scale() != 8.0f ||
        frame->get_z_scale() != 8.0f) {
        // TODO - consider proximity tests instead of exact equality tests
        throw res::export_error("nsf::wgeo_v1: scale is invalid");
    }

    // Export the info item (0).
    w.begin();
    w.write_s32(world->get_x());
    w.write_s32(world->get_z());
    w.write_s32(world->get_y());
    w.write_u32(mesh->get_triangles().size());
    w.write_u32(frame->get_vertices().size());
    w.write_u32(0); // for now
    w.write_u32(get_tpag_ref_count());
    w.write_u32(0); // for now
    w.write_u32(get_tpag_ref0());
    w.write_u32(get_tpag_ref1());
    w.write_u32(get_tpag_ref2());
    w.write_u32(get_tpag_ref3());
    w.write_u32(get_tpag_ref4());
    w.write_u32(get_tpag_ref5());
    w.write_u32(get_tpag_ref6());
    w.write_u32(get_tpag_ref7());
    item_info = w.end();

    // Export the vertices.
    const int COORD_MIN = -(1 << 12);
    const int COORD_MAX = (1 << 12) - 1;
    w.begin();
    for (auto &&vertex : frame->get_vertices()) {
        int x = vertex.x;
        int y = vertex.y;
        int z = vertex.z;

        if (x < COORD_MIN || x >= COORD_MAX ||
            y < COORD_MIN || y >= COORD_MAX ||
            z < COORD_MIN || z >= COORD_MAX) {
            throw res::export_error("nsf::wgeo_v1: vertex x/y/z out of range");
        }

        unsigned int y1 = y & 0xFF;
        unsigned int y2 = (y >> 8) & 0x3;
        signed int y3 = (y >> 10) & 0x7;

        w.write_ubits( 1, vertex.fx);
        w.write_ubits( 2, y2);
        w.write_ubits(13, z);
        w.write_sbits( 3, y3);
        w.write_sbits(13, x);
        w.write_ubits( 8, vertex.color.b);
        w.write_ubits( 8, vertex.color.g);
        w.write_ubits( 8, vertex.color.r);
        w.write_ubits( 8, y1);
    }
    w.pad(4);
    item_vertices = w.end();

    // Export the triangles.
    w.begin();
    for (auto &&triangle : mesh->get_triangles()) {
        w.write_ubits(8, triangle.unk0);
        w.write_ubits(12, triangle.v[0].vertex_index);
        w.write_ubits(12, triangle.v[1].vertex_index);
        w.write_ubits(8, triangle.unk1);
        w.write_ubits(12, 0); // for now
        w.write_ubits(12, triangle.v[2].vertex_index);
    }
    w.pad(4);
    item_triangles = w.end();

    return items;
}

}
}
