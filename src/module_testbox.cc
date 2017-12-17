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
#include <stdio.h>
#include "edit.hh"
#include "gfx.hh"
#include "nsf.hh"
#include "misc.hh"

namespace drnsf {

static void frame(edit::core &m_core, int delta)
{
    auto &&ns = m_core.m_proj.get_asset_root();

    gfx::model::ref smodel = edit::g_selected_asset;

    glPushMatrix();
    glRotatef(edit::g_camera_pitch, 1, 0, 0);
    glRotatef(edit::g_camera_yaw, 0, 1, 0);

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
            glTranslatef(-smodel->get_scene_x(), -smodel->get_scene_y(), -smodel->get_scene_z());
        }
        glTranslatef(model->get_scene_x(), model->get_scene_y(), model->get_scene_z());

        auto &&colors = mesh->get_colors();
        auto &&vertices = frame->get_vertices();

        glBegin(GL_TRIANGLES);
        for (auto &&p : mesh->get_triangles()) {
            glColor3ubv(colors[p.v[0].color_index].v);
            glVertex3fv(vertices[p.v[0].vertex_index].v);
            glColor3ubv(colors[p.v[1].color_index].v);
            glVertex3fv(vertices[p.v[1].vertex_index].v);
            glColor3ubv(colors[p.v[2].color_index].v);
            glVertex3fv(vertices[p.v[2].vertex_index].v);
        }
        glEnd();

        glBegin(GL_TRIANGLES);
        for (auto &&p : mesh->get_quads()) {
            glColor3ubv(colors[p.v[0].color_index].v);
            glVertex3fv(vertices[p.v[0].vertex_index].v);
            glColor3ubv(colors[p.v[1].color_index].v);
            glVertex3fv(vertices[p.v[1].vertex_index].v);
            glColor3ubv(colors[p.v[2].color_index].v);
            glVertex3fv(vertices[p.v[2].vertex_index].v);
            glColor3ubv(colors[p.v[2].color_index].v);
            glVertex3fv(vertices[p.v[2].vertex_index].v);
            glColor3ubv(colors[p.v[1].color_index].v);
            glVertex3fv(vertices[p.v[1].vertex_index].v);
            glColor3ubv(colors[p.v[3].color_index].v);
            glVertex3fv(vertices[p.v[3].vertex_index].v);
        }
        glEnd();

        glPopMatrix();
    }

    glBegin(GL_LINE_LOOP);
    glVertex3f(-200, -200, -200);
    glVertex3f(+200, -200, -200);
    glVertex3f(+200, +200, -200);
    glVertex3f(-200, +200, -200);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex3f(-200, -200, +200);
    glVertex3f(+200, -200, +200);
    glVertex3f(+200, +200, +200);
    glVertex3f(-200, +200, +200);
    glEnd();

    glBegin(GL_LINES);
    glVertex3f(-200, -200, -200);
    glVertex3f(-200, -200, +200);
    glVertex3f(+200, -200, -200);
    glVertex3f(+200, -200, +200);
    glVertex3f(+200, +200, -200);
    glVertex3f(+200, +200, +200);
    glVertex3f(-200, +200, -200);
    glVertex3f(-200, +200, +200);
    glEnd();

    // Restore the GL matrices.
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

std::function<void(int)> create_mod_testbox(edit::core &core)
{
    return [&core](int delta_time) {
        frame(core, delta_time);
    };
}

}
