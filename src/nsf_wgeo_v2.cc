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

#include "common.hh"
#include "nsf.hh"

namespace nsf {

void wgeo_v2::import_entry(TRANSACT,const raw_item_list &items,res::name::space &ns)
{
	assert_alive();

	util::binreader r;

	// Ensure we have the correct number of items (7).
	if (items.size() != 7)
		throw 0; // FIXME

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
		throw 0; // FIXME

	// Parse the vertices.
	std::vector<gfx::vertex> vertices(vertex_count);
	std::vector<int> vertex_colors(vertex_count);
	for (auto &&i : util::range_of(vertices)) {
		auto &vertex = vertices[i];
		auto &vertex_color = vertex_colors[i];

		// TODO - explain vertex format here

		r.begin(&item_vertices[(vertex_count - 1 - i) * 4],4);
		auto color_mid  = r.read_ubits(4);
		auto x          = r.read_sbits(12);
		auto color_high = r.read_ubits(2);
		auto fx         = r.read_ubits(2);
		auto y          = r.read_sbits(12);
		r.end();

		r.begin(&item_vertices[vertex_count * 4 + i * 2],2);
		auto color_low = r.read_ubits(4);
		auto z         = r.read_sbits(12);
		r.end();

		vertex.x = x / 2048.0;
		vertex.y = y / 2048.0;
		vertex.z = z / 2048.0;

		vertex_color = color_low | color_mid << 4 | color_high << 8;
	}

	// Ensure the triangle count is correct.
	if (triangle_count != item_triangles.size() / 6)
		throw 0; // FIXME

	// Parse the triangles.
	std::vector<gfx::poly> triangles(triangle_count);
	for (auto &&i : util::range_of(triangles)) {
		auto &triangle = triangles[i];

		// TODO - explain triangle format here

		r.begin(&item_triangles[(triangle_count - 1 - i) * 4],4);
		auto triangle_unk0 = r.read_ubits(8);
		auto vertex0       = r.read_ubits(12);
		auto vertex1       = r.read_ubits(12);
		r.end();

		r.begin(&item_triangles[triangle_count * 4 + i * 2],2);
		auto triangle_unk1 = r.read_ubits(4);
		auto vertex2       = r.read_ubits(12);
		r.end();

		triangle.type = gfx::polytype::tri;
		triangle.vertices[0] = vertex0;
		triangle.vertices[1] = vertex1;
		triangle.vertices[2] = vertex2;

		// For whatever reason, some triangles may have invalid vertex
		// indices. In this case, we can't grab the vertex color (there
		// is none).
		triangle.colors[0] = (vertex0 < vertex_count) ? vertex_colors[vertex0] : -1;
		triangle.colors[1] = (vertex1 < vertex_count) ? vertex_colors[vertex1] : -1;
		triangle.colors[2] = (vertex2 < vertex_count) ? vertex_colors[vertex2] : -1;
	}

	// Ensure the quad count is correct.
	if (quad_count != item_quads.size() / 8)
		throw 0; // FIXME

	// Parse the quads.
	std::vector<gfx::poly> quads(quad_count);
	r.begin(item_quads);
	for (auto &&quad : quads) {
		// TODO - explain quad format here

		auto quad_unk0 = r.read_ubits(8);
		auto vertex0   = r.read_ubits(12);
		auto vertex1   = r.read_ubits(12);
		auto quad_unk1 = r.read_ubits(8);
		auto vertex2   = r.read_ubits(12);
		auto vertex3   = r.read_ubits(12);

		quad.type = gfx::polytype::quad;
		quad.vertices[0] = vertex0;
		quad.vertices[1] = vertex1;
		quad.vertices[2] = vertex2;
		quad.vertices[3] = vertex3;

		// For whatever reason, some quads may have invalid vertex
		// indices. See above with triangles (same issue).
		quad.colors[0] = (vertex0 < vertex_count) ? vertex_colors[vertex0] : -1;
		quad.colors[1] = (vertex1 < vertex_count) ? vertex_colors[vertex1] : -1;
		quad.colors[2] = (vertex2 < vertex_count) ? vertex_colors[vertex2] : -1;
		quad.colors[3] = (vertex3 < vertex_count) ? vertex_colors[vertex3] : -1;
	}
	r.end();

	// Ensure the item4 count is correct.
	if (item4_count != item_4.size() / 12)
		throw 0; // FIXME

	// Parse item4.
	// TODO - what is this? likely texture info

	// Ensure the color count is correct.
	if (color_count != item_colors.size() / 4)
		throw 0; // FIXME

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
		throw 0; // FIXME

	// Parse item6.
	// TODO - what is this? likely animated texture info

	// Ensure the tpag ref count is viable.
	if (tpag_ref_count > 8)
		throw 0; // FIXME

	// Parse the tpag references.
	// TODO

	res::name basename = ns / "scenery" / util::format("$",get_eid());

	// Create the frame which will contain this scene's vertex positions.
	gfx::frame::ref frame = basename / "frame";
	frame.create(ts);
	frame->set_vertices(ts,std::move(vertices));

	// Create the animation for this scene (just one frame, scenes are not
	// vertex-animated).
	gfx::anim::ref anim = basename / "anim";
	anim.create(ts);
	anim->set_frames(ts,{frame});

	// Create the mesh for this scene.
	gfx::mesh::ref mesh = basename / "mesh";
	mesh.create(ts);
	std::vector<gfx::poly> polys;
	polys.insert(polys.end(),triangles.begin(),triangles.end());
	polys.insert(polys.end(),quads.begin(),quads.end());
	mesh->set_polys(ts,std::move(polys));
	mesh->set_colors(ts,std::move(colors));

	// Create the model for this scene.
	gfx::model::ref model = basename;
	model.create(ts);
	model->set_anim(ts,anim);
	model->set_mesh(ts,mesh);
	model->set_scene_x(ts,scene_x / 32768.0);
	model->set_scene_y(ts,scene_y / 32768.0);
	model->set_scene_z(ts,scene_z / 32768.0);

	// Finish importing.
	set_item0(ts,items[0]);
	set_item4(ts,items[4]);
	set_item6(ts,items[6]);
	set_model(ts,model);
}

}
