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

#include "module.hh"
#include <stdio.h>
#include "gfx.hh"
#include "nsf.hh"

static double angle = 0;

#include "begin.hh"

constexpr static const char *mod_name = "Spinning Test Box";

static std::vector<unsigned char> read_file(const std::string &filename)
{
	std::vector<unsigned char> data;
	FILE *f = fopen(filename.c_str(),"rb");
	if (!f)
		throw 5;
	fseek(f,0,SEEK_END);
	data.resize(ftell(f));
	fseek(f,0,SEEK_SET);
	fread(data.data(),data.size(),1,f);
	fclose(f);
	return data;
}

DEFINE_SHARED(res::anyref,selected_asset);

void frame(int delta) override
{
	static std::vector<gfx::vertex> testbox_vertices = {
		{ -1, -1, -1 },
		{ +1, -1, -1 },
		{ +1, +1, -1 },
		{ -1, +1, -1 },
		{ +1, -1, +1 },
		{ -1, -1, +1 },
		{ -1, +1, +1 },
		{ +1, +1, +1 }
	};

	static std::vector<gfx::poly> testbox_polys = {
		{ gfx::polytype::quad, { 0, 1, 3, 2 }, { 0, 1, 3, 2 } },
		{ gfx::polytype::quad, { 4, 5, 7, 6 }, { 4, 5, 7, 6 } },
		{ gfx::polytype::quad, { 5, 0, 6, 3 }, { 5, 0, 6, 3 } },
		{ gfx::polytype::quad, { 1, 4, 2, 7 }, { 1, 4, 2, 7 } },
		{ gfx::polytype::quad, { 3, 2, 6, 7 }, { 3, 2, 6, 7 } },
		{ gfx::polytype::quad, { 5, 4, 0, 1 }, { 5, 4, 0, 1 } }
	};

	static std::vector<gfx::color> testbox_colors = {
		{ 255,   0,   0 },
		{   0, 255,   0 },
		{   0,   0, 255 },
		{ 255, 255,   0 },
		{   0, 255, 255 },
		{ 255,   0, 255 },
		{   0,   0,   0 },
		{ 255, 255, 255 }
	};

	if (ImGui::Button("Create test box")) {
		nx << [&](TRANSACT) {
			gfx::frame::ref frame(ns,"testbox/anim/0000");
			gfx::anim::ref anim(ns,"testbox/anim");
			gfx::mesh::ref mesh(ns,"testbox/mesh");
			gfx::model::ref model(ns,"testbox");

			ts.describe("Create test box");
			frame.create(ts);
			frame->set_vertices(ts,testbox_vertices);

			anim.create(ts);
			anim->set_frames(ts,std::vector<gfx::frame::ref>{frame});

			mesh.create(ts);
			mesh->set_polys(ts,testbox_polys);
			mesh->set_colors(ts,testbox_colors);

			model.create(ts);
			model->set_anim(ts,anim);
			model->set_mesh(ts,mesh);
		};
	}

	if (ImGui::Button("Load test scene")) {
		nx << [&](TRANSACT) {
			ts.describe("Load test scenery file");
			nsf::raw_entry::ref raw(ns,"raw");
			raw.create(ts);
			raw->import_file(ts,read_file("/tmp/scene.nsentry"));
			raw->process_as<nsf::wgeo_v2>(ts,ns);
		};
	}

	if (ImGui::Button("Load wr scenery")) {
		nx << [&](TRANSACT) {
			ts.describe("Load warp room scenery (test)");
			for (auto &&i : util::range(0,16)) {
				nsf::raw_entry::ref raw(ns,util::format("wr-wgeo-$",i));
				raw.create(ts);
				raw->import_file(ts,read_file(util::format("/tmp/s$.nsentry",i)));
				raw->process_as<nsf::wgeo_v2>(ts,ns);
			}
		};
	}

	if (ImGui::Button("Load /tmp/nsentry")) {
		nx << [&](TRANSACT) {
			ts.describe("Load /tmp/nsentry");
			nsf::raw_entry::ref raw(ns,"nsentry");
			raw.create(ts);
			raw->import_file(ts,read_file("/tmp/nsentry"));
		};
	}

	if (ImGui::Button("Load /tmp/nspage")) {
		nx << [&](TRANSACT) {
			ts.describe("Load /tmp/nspage");
			nsf::spage::ref raw(ns,"nspage");
			raw.create(ts);
			raw->import_file(ts,read_file("/tmp/nspage"));
		};
	}

	if (ImGui::Button("Load /tmp/nsfile")) {
		nx << [&](TRANSACT) {
			ts.describe("Load /tmp/nsfile");
			nsf::raw_entry::ref raw(ns,"nsfile");
			raw.create(ts);
			raw->import_file(ts,read_file("/tmp/nsfile"));
		};
	}

	gfx::model::ref smodel = selected_asset;

	if (!smodel.ok())
		return;

	glPushMatrix();
	glRotatef(cam().pitch,1,0,0);
	glRotatef(angle,0,1,0);

	for (gfx::model::ref model : ns.get_asset_names()) {
		if (!model.ok())
			continue;

		auto mesh = model->get_mesh();
		if (!mesh.ok())
			continue;

		auto anim = model->get_anim();
		if (!anim.ok())
			continue;

		auto &&frames = anim->get_frames();
		if (frames.empty())
			continue;

		auto frame = frames[0];
		if (!frame.ok())
			continue;

		glPushMatrix();
		glTranslatef(-smodel->get_scene_x(),-smodel->get_scene_y(),-smodel->get_scene_z());
		glTranslatef(model->get_scene_x(),model->get_scene_y(),model->get_scene_z());

		auto &&colors = mesh->get_colors();
		auto &&vertices = frame->get_vertices();

		glBegin(GL_TRIANGLES);
		for (auto &&p : mesh->get_polys()) {
			glColor3ubv(colors[p.colors[0]].v);
			glVertex3fv(vertices[p.vertices[0]].v);
			glColor3ubv(colors[p.colors[1]].v);
			glVertex3fv(vertices[p.vertices[1]].v);
			glColor3ubv(colors[p.colors[2]].v);
			glVertex3fv(vertices[p.vertices[2]].v);
			if (p.type == gfx::polytype::quad) {
				glColor3ubv(colors[p.colors[2]].v);
				glVertex3fv(vertices[p.vertices[2]].v);
				glColor3ubv(colors[p.colors[1]].v);
				glVertex3fv(vertices[p.vertices[1]].v);
				glColor3ubv(colors[p.colors[3]].v);
				glVertex3fv(vertices[p.vertices[3]].v);
			}
		}
		glEnd();

		glPopMatrix();
	}

	glBegin(GL_LINE_LOOP);
	glVertex3f(-1,-1,-1);
	glVertex3f(+1,-1,-1);
	glVertex3f(+1,+1,-1);
	glVertex3f(-1,+1,-1);
	glEnd();

	glBegin(GL_LINE_LOOP);
	glVertex3f(-1,-1,+1);
	glVertex3f(+1,-1,+1);
	glVertex3f(+1,+1,+1);
	glVertex3f(-1,+1,+1);
	glEnd();

	glBegin(GL_LINES);
	glVertex3f(-1,-1,-1);
	glVertex3f(-1,-1,+1);
	glVertex3f(+1,-1,-1);
	glVertex3f(+1,-1,+1);
	glVertex3f(+1,+1,-1);
	glVertex3f(+1,+1,+1);
	glVertex3f(-1,+1,-1);
	glVertex3f(-1,+1,+1);
	glEnd();

	// Restore the GL matrices.
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);

	static float delta_factor = 0.1;
	angle += delta * delta_factor;
	ImGui::InputFloat("Spin Speed",&delta_factor,0.05,0);
}

END_MODULE
