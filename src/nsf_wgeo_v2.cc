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
    auto scene_x        = r.read_s32();
    auto scene_y        = r.read_s32();
    auto scene_z        = r.read_s32();
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
    std::vector<int> vertex_colors(vertex_count);
    for (auto &&i : util::range_of(vertices)) {
        auto &vertex = vertices[i];
        auto &vertex_color = vertex_colors[i];

        // TODO - explain vertex format here

        r.begin(&item_vertices[(vertex_count - 1 - i) * 4], 4);
        auto color_mid  = r.read_ubits(4);
        auto x          = r.read_sbits(12);
        auto color_high = r.read_ubits(2);
        auto fx         = r.read_ubits(2);
        auto y          = r.read_sbits(12);
        r.end();

        r.begin(&item_vertices[vertex_count * 4 + i * 2], 2);
        auto color_low = r.read_ubits(4);
        auto z         = r.read_sbits(12);
        r.end();

        vertex.x = x * 16;
        vertex.y = y * 16;
        vertex.z = z * 16;
        vertex.fx = fx;

        vertex_color = color_low | color_mid << 4 | color_high << 8;
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

        // For whatever reason, some triangles may have invalid vertex
        // indices. In this case, we can't grab the vertex color (there
        // is none).
        triangle.v[0].color_index = (vertex0 < vertex_count) ? vertex_colors[vertex0] : -1;
        triangle.v[1].color_index = (vertex1 < vertex_count) ? vertex_colors[vertex1] : -1;
        triangle.v[2].color_index = (vertex2 < vertex_count) ? vertex_colors[vertex2] : -1;

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

        // For whatever reason, some quads may have invalid vertex
        // indices. See above with triangles (same issue).
        quad.v[0].color_index = (vertex0 < vertex_count) ? vertex_colors[vertex0] : -1;
        quad.v[1].color_index = (vertex1 < vertex_count) ? vertex_colors[vertex1] : -1;
        quad.v[2].color_index = (vertex2 < vertex_count) ? vertex_colors[vertex2] : -1;
        quad.v[3].color_index = (vertex3 < vertex_count) ? vertex_colors[vertex3] : -1;

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
    std::vector<gfx::color> colors(color_count);
    r.begin(item_colors);
    for (auto &&color : colors) {
        // TODO - explain color format here

        color.r = r.read_u8();
        color.g = r.read_u8();
        color.b = r.read_u8();
        r.expect_u8(0);
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
    gfx::model::ref model = atom;
    model.create(TS, get_proj());
    model->set_anim(TS, anim);
    model->set_mesh(TS, mesh);
    model->set_scene_x(TS, scene_x);
    model->set_scene_y(TS, scene_y);
    model->set_scene_z(TS, scene_z);

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
    set_model(TS, model);
}

}
}
