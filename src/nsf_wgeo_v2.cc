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
void wgeo_v2::import_entry(TRANSACT, const std::vector<util::blob> &items)
{
    assert_alive();

    util::binreader r;

    // Ensure we have the correct number of items (7).
    if (items.size() != 7)
        throw res::import_error("nsf::wgeo_v2: wrong item count");

    auto &item_info      = items[0];
    auto &item_vertices  = items[1];
    auto &item_triangles = items[2];
    auto &item_quads     = items[3];
    auto &item_4         = items[4];
    auto &item_colors    = items[5];
    auto &item_6         = items[6];

    // Parse the info item (0).
    r.begin(item_info);
    auto world_x        = r.read_s32();
    auto world_z        = r.read_s32();
    auto world_y        = r.read_s32();
    auto info_unk0      = r.read_u32();
    auto vertex_count   = r.read_u32();
    auto triangle_count = r.read_u32();
    auto quad_count     = r.read_u32();
    auto item4_count    = r.read_u32();
    auto color_count    = r.read_u32();
    auto item6_count    = r.read_u32();
    auto tpag_ref_count = r.read_u32();
    auto tpag_ref0      = r.read_u32();
    auto tpag_ref1      = r.read_u32();
    auto tpag_ref2      = r.read_u32();
    auto tpag_ref3      = r.read_u32();
    auto tpag_ref4      = r.read_u32();
    auto tpag_ref5      = r.read_u32();
    auto tpag_ref6      = r.read_u32();
    auto tpag_ref7      = r.read_u32();
    r.end();

    // Ensure the vertex count is correct.
    if (vertex_count != item_vertices.size() / 6)
        throw res::import_error("nsf::wgeo_v2: bad vertex item size");

    // Parse the vertices.
    std::vector<gfx::vertex> vertices(vertex_count);
    for (auto &&i : util::range_of(vertices)) {
        auto &vertex = vertices[i];

        // TODO - explain vertex format here

        r.begin(&item_vertices[(vertex_count - 1 - i) * 4], 4);
        auto color_mid  = r.read_ubits(4);
        auto x          = r.read_sbits(12);
        auto color_high = r.read_ubits(2);
        auto fx         = r.read_ubits(2);
        auto z          = r.read_sbits(12);
        r.end();

        r.begin(&item_vertices[vertex_count * 4 + i * 2], 2);
        auto color_low = r.read_ubits(4);
        auto y         = r.read_sbits(12);
        r.end();

        vertex.x = x;
        vertex.y = y;
        vertex.z = z;

        vertex.fx = fx;

        vertex.color_index = color_low | color_mid << 4 | color_high << 8;
    }

    // Ensure the triangle count is correct.
    if (triangle_count != item_triangles.size() / 6)
        throw res::import_error("nsf::wgeo_v2: bad triangle item size");

    // Parse the triangles.
    std::vector<gfx::triangle> triangles(triangle_count);
    for (auto &&i : util::range_of(triangles)) {
        auto &triangle = triangles[i];

        // TODO - explain triangle format here

        r.begin(&item_triangles[(triangle_count - 1 - i) * 4], 4);
        auto triangle_unk0 = r.read_ubits(8);
        auto vertex0       = r.read_ubits(12);
        auto vertex1       = r.read_ubits(12);
        r.end();

        r.begin(&item_triangles[triangle_count * 4 + i * 2], 2);
        auto triangle_unk1 = r.read_ubits(4);
        auto vertex2       = r.read_ubits(12);
        r.end();

        triangle.v[0].vertex_index = vertex0;
        triangle.v[1].vertex_index = vertex1;
        triangle.v[2].vertex_index = vertex2;

        triangle.v[0].color_index = -1;
        triangle.v[1].color_index = -1;
        triangle.v[2].color_index = -1;

        triangle.unk0 = triangle_unk0;
        triangle.unk1 = triangle_unk1;
    }

    // Ensure the quad count is correct.
    if (quad_count != item_quads.size() / 8)
        throw res::import_error("nsf::wgeo_v2: bad quad item size");

    // Parse the quads.
    std::vector<gfx::quad> quads(quad_count);
    r.begin(item_quads);
    for (auto &&quad : quads) {
        // TODO - explain quad format here

        auto quad_unk0 = r.read_ubits(8);
        auto vertex0   = r.read_ubits(12);
        auto vertex1   = r.read_ubits(12);
        auto quad_unk1 = r.read_ubits(8);
        auto vertex2   = r.read_ubits(12);
        auto vertex3   = r.read_ubits(12);

        quad.v[0].vertex_index = vertex0;
        quad.v[1].vertex_index = vertex1;
        quad.v[2].vertex_index = vertex2;
        quad.v[3].vertex_index = vertex3;

        quad.v[0].color_index = -1;
        quad.v[1].color_index = -1;
        quad.v[2].color_index = -1;
        quad.v[3].color_index = -1;

        quad.unk0 = quad_unk0;
        quad.unk1 = quad_unk1;
    }
    r.end();

    // Ensure the item4 count is correct.
    if (item4_count != item_4.size() / 12)
        throw res::import_error("nsf::wgeo_v2: bad item4 size");

    // Parse item4.
    // TODO - what is this? likely texture info

    // Ensure the color count is correct.
    if (color_count != item_colors.size() / 4)
        throw res::import_error("nsf::wgeo_v2: bad color item size");

    // Parse the colors.
    std::vector<gfx::rgb888> colors(color_count);
    r.begin(item_colors);
    for (auto &&color : colors) {
        // TODO - explain color format here

        color.r = r.read_u8();
        color.g = r.read_u8();
        color.b = r.read_u8();
        auto ex = r.read_u8();

        if (ex != 0)
            throw res::import_error("nsf::wgeo_v2: bad extra color byte");
    }
    r.end();

    // Ensure the item6 count is correct.
    if (item6_count != item_6.size() / 4)
        throw res::import_error("nsf::wgeo_v2: bad item6 size");

    // Parse item6.
    // TODO - what is this? likely animated texture info

    // Ensure the tpag ref count is viable.
    if (tpag_ref_count > 8)
        throw res::import_error("nsf::wgeo_v2: bad tpag ref count");

    // Parse the tpag references.
    // TODO

    res::atom atom = get_proj().get_asset_root()
        / "scenery"
        / "$"_fmt(get_eid());

    // Create the frame which will contain this scene's vertex positions.
    gfx::frame::ref frame = atom / "frame";
    frame.create(TS, get_proj());
    frame->set_vertices(TS, std::move(vertices));
    frame->set_x_scale(TS, 16.0f);
    frame->set_y_scale(TS, 16.0f);
    frame->set_z_scale(TS, 16.0f);

    // Create the animation for this scene (just one frame, scenes are not
    // vertex-animated).
    gfx::anim::ref anim = atom / "anim";
    anim.create(TS, get_proj());
    anim->set_frames(TS, {frame});

    // Create the mesh for this scene.
    gfx::mesh::ref mesh = atom / "mesh";
    mesh.create(TS, get_proj());
    mesh->set_triangles(TS, std::move(triangles));
    mesh->set_quads(TS, std::move(quads));
    mesh->set_colors(TS, std::move(colors));

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
    set_info_unk0(TS, info_unk0);
    set_tpag_ref_count(TS, tpag_ref_count);
    set_tpag_ref0(TS, tpag_ref0);
    set_tpag_ref1(TS, tpag_ref1);
    set_tpag_ref2(TS, tpag_ref2);
    set_tpag_ref3(TS, tpag_ref3);
    set_tpag_ref4(TS, tpag_ref4);
    set_tpag_ref5(TS, tpag_ref5);
    set_tpag_ref6(TS, tpag_ref6);
    set_tpag_ref7(TS, tpag_ref7);
    set_item4(TS, items[4]);
    set_item6(TS, items[6]);
    set_world(TS, world);
}

// declared in nsf.hh
std::vector<util::blob> wgeo_v2::export_entry(uint32_t &out_type) const
{
    assert_alive();

    util::binwriter w;

    out_type = 3;
    std::vector<util::blob> items(7);

    auto &item_info      = items[0];
    auto &item_vertices  = items[1];
    auto &item_triangles = items[2];
    auto &item_quads     = items[3];
    auto &item_4         = items[4];
    auto &item_colors    = items[5];
    auto &item_6         = items[6];

    auto &&world = get_world();
    if (!world.ok())
        throw res::export_error("nsf::wgeo_v2: bad world ref");

    auto &&model = world->get_model();
    if (!model.ok())
        throw res::export_error("nsf::wgeo_v2: bad model ref");

    auto &&mesh = model->get_mesh();
    if (!mesh.ok())
        throw res::export_error("nsf::wgeo_v2: bad mesh ref");

    auto &&anim = model->get_anim();
    if (!anim.ok())
        throw res::export_error("nsf::wgeo_v2: bad anim ref");

    auto &&frames = anim->get_frames();
    if (frames.size() != 1)
        throw res::export_error("nsf::wgeo_v2: invalid frame count");

    auto &&frame = frames[0];
    if (!frame.ok())
        throw res::export_error("nsf::wgeo_v2: bad frame ref");

    // Check the scale to ensure it matches this format.
    if (frame->get_x_scale() != 16.0f ||
        frame->get_y_scale() != 16.0f ||
        frame->get_z_scale() != 16.0f) {
        // TODO - consider proximity tests instead of exact equality tests
        throw res::export_error("nsf::wgeo_v2: scale is invalid");
    }

    // Export the info item (0).
    w.begin();
    w.write_s32(world->get_x());
    w.write_s32(world->get_z());
    w.write_s32(world->get_y());
    w.write_u32(get_info_unk0());
    w.write_u32(frame->get_vertices().size());
    w.write_u32(mesh->get_triangles().size());
    w.write_u32(mesh->get_quads().size());
    w.write_u32(get_item4().size() / 12);
    w.write_u32(mesh->get_colors().size());
    w.write_u32(get_item6().size() / 4);
    w.write_u32(get_tpag_ref_count());
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
    const int COORD_MIN = -(1 << 11);
    const int COORD_MAX = (1 << 11) - 1;
    w.begin();
    for (auto &&vertex : util::reverse_of(frame->get_vertices())) {
        int x = vertex.x;
        int z = vertex.z;

        if (x < COORD_MIN || x >= COORD_MAX ||
            z < COORD_MIN || z >= COORD_MAX) {
            throw res::export_error("nsf::wgeo_v2: vertex x/z out of range");
        }

        if (vertex.color_index == -1) {
            throw res::export_error("nsf::wgeo_v2: vertex colors required");
        }
        if (vertex.color_index < 0 || vertex.color_index >= (1L << 10)) {
            throw res::export_error("nsf::wgeo_v2: vertex color out of range");
        }

        unsigned int color_high = (vertex.color_index >> 8) & 0x3;
        unsigned int color_mid  = (vertex.color_index >> 4) & 0xF;

        w.write_ubits( 4, color_mid);
        w.write_sbits(12, x);
        w.write_ubits( 2, color_high);
        w.write_ubits( 2, vertex.fx);
        w.write_sbits(12, z);
    }
    for (auto &&vertex : frame->get_vertices()) {
        int y = vertex.y;

        if (y < COORD_MIN || y >= COORD_MAX) {
            throw res::export_error("nsf::wgeo_v2: vertex y out of range");
        }

        // The vertex color index has already been range checked in the loop
        // above.

        unsigned int color_low = vertex.color_index & 0xF;

        w.write_ubits( 4, color_low);
        w.write_sbits(12, y);
    }
    w.pad(4);
    item_vertices = w.end();

    // Export the triangles.
    w.begin();
    for (auto &&triangle : util::reverse_of(mesh->get_triangles())) {
        w.write_ubits(8, triangle.unk0);
        w.write_ubits(12, triangle.v[0].vertex_index);
        w.write_ubits(12, triangle.v[1].vertex_index);
    }
    for (auto &&triangle : mesh->get_triangles()) {
        w.write_ubits(4, triangle.unk1);
        w.write_ubits(12, triangle.v[2].vertex_index);

        if (triangle.v[0].color_index != -1 ||
            triangle.v[1].color_index != -1 ||
            triangle.v[2].color_index != -1) {
            throw res::export_error(
                "nsf::wgeo_v2: corner colors not supported"
            );
        }
    }
    w.pad(4);
    item_triangles = w.end();

    // Export the quads.
    w.begin();
    for (auto &&quad : mesh->get_quads()) {
        w.write_ubits(8, quad.unk0);
        w.write_ubits(12, quad.v[0].vertex_index);
        w.write_ubits(12, quad.v[1].vertex_index);
        w.write_ubits(8, quad.unk1);
        w.write_ubits(12, quad.v[2].vertex_index);
        w.write_ubits(12, quad.v[3].vertex_index);

        if (quad.v[0].color_index != -1 || quad.v[1].color_index != -1 ||
            quad.v[2].color_index != -1 || quad.v[3].color_index != -1) {
            throw res::export_error(
                "nsf::wgeo_v2: corner colors not supported"
            );
        }
    }
    item_quads = w.end();

    // Export item4.
    item_4 = get_item4();

    // Export the colors.
    w.begin();
    for (auto &&color : mesh->get_colors()) {
        w.write_u8(color.r);
        w.write_u8(color.g);
        w.write_u8(color.b);
        w.write_u8(0);
    }
    item_colors = w.end();

    // Export item6.
    item_6 = get_item6();

    return items;
}

}
}
