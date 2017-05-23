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

#include "module.hh"
#include <stdio.h>
#include "gfx.hh"
#include "nsf.hh"
#include "misc.hh"

namespace drnsf {

BEGIN_MODULE

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

void frame(int delta)
{
    auto &&nx = m_core.m_proj.get_transact();
    auto &&ns = m_core.m_proj.get_asset_root();
    auto &&proj = m_core.m_proj;

    if (ImGui::Button("Load nsfile")) {
        nx << [&](TRANSACT) {
            TS.describe("Load nsfile");
            nsf::archive::ref raw = ns / "nsfile";
            raw.create(TS,proj);
            raw->import_file(TS,read_file("nsfile"));
            for (misc::raw_data::ref page : raw->get_pages()) {
                if (page->get_data()[2] == 1)
                    continue;

                nsf::spage::ref spage = page / "hewm";
                spage.create(TS,proj);
                spage->import_file(TS,page->get_data());
                page->destroy(TS);
                spage->rename(TS,page);
                spage = page;

                for (misc::raw_data::ref pagelet : spage->get_pagelets()) {
                    nsf::raw_entry::ref entry = pagelet / "hewmy";
                    entry.create(TS,proj);
                    entry->import_file(TS,pagelet->get_data());
                    pagelet->destroy(TS);
                    entry->rename(TS,pagelet);
                    entry = pagelet;
                    entry->process_by_type(TS,nsf::game_ver::crash2);
                }
            }
        };
    }

    gfx::model::ref smodel = edit::g_selected_asset;

    glPushMatrix();
    glRotatef(edit::g_camera_pitch,1,0,0);
    glRotatef(edit::g_camera_yaw,0,1,0);

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
        if (smodel.ok()) {
            glTranslatef(-smodel->get_scene_x(),-smodel->get_scene_y(),-smodel->get_scene_z());
        }
        glTranslatef(model->get_scene_x(),model->get_scene_y(),model->get_scene_z());

        auto &&colors = mesh->get_colors();
        auto &&vertices = frame->get_vertices();

        glBegin(GL_TRIANGLES);
        for (auto &&p : mesh->get_triangles()) {
            glColor3ubv(colors[p[0].color_index].v);
            glVertex3fv(vertices[p[0].vertex_index].v);
            glColor3ubv(colors[p[1].color_index].v);
            glVertex3fv(vertices[p[1].vertex_index].v);
            glColor3ubv(colors[p[2].color_index].v);
            glVertex3fv(vertices[p[2].vertex_index].v);
        }
        glEnd();

        glBegin(GL_TRIANGLES);
        for (auto &&p : mesh->get_quads()) {
            glColor3ubv(colors[p[0].color_index].v);
            glVertex3fv(vertices[p[0].vertex_index].v);
            glColor3ubv(colors[p[1].color_index].v);
            glVertex3fv(vertices[p[1].vertex_index].v);
            glColor3ubv(colors[p[2].color_index].v);
            glVertex3fv(vertices[p[2].vertex_index].v);
            glColor3ubv(colors[p[2].color_index].v);
            glVertex3fv(vertices[p[2].vertex_index].v);
            glColor3ubv(colors[p[1].color_index].v);
            glVertex3fv(vertices[p[1].vertex_index].v);
            glColor3ubv(colors[p[3].color_index].v);
            glVertex3fv(vertices[p[3].vertex_index].v);
        }
        glEnd();

        glPopMatrix();
    }

    glBegin(GL_LINE_LOOP);
    glVertex3f(-200,-200,-200);
    glVertex3f(+200,-200,-200);
    glVertex3f(+200,+200,-200);
    glVertex3f(-200,+200,-200);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex3f(-200,-200,+200);
    glVertex3f(+200,-200,+200);
    glVertex3f(+200,+200,+200);
    glVertex3f(-200,+200,+200);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(-200,-200,-200);
    glVertex3f(-200,-200,+200);
    glVertex3f(+200,-200,-200);
    glVertex3f(+200,-200,+200);
    glVertex3f(+200,+200,-200);
    glVertex3f(+200,+200,+200);
    glVertex3f(-200,+200,-200);
    glVertex3f(-200,+200,+200);
    glEnd();

    // Restore the GL matrices.
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

END_MODULE

std::function<void(int)> create_mod_testbox(edit::core &core)
{
    return [m = mod(core)](int delta_time) mutable {
        m.frame(delta_time);
    };
}

}
