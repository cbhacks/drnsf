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

#include "common.hh"
#include <stdio.h>
#include "edit.hh"
#include "gfx.hh"
#include "nsf.hh"
#include "misc.hh"

namespace drnsf {

static void dovert(
    const gfx::corner &cn,
    const std::vector<gfx::color> &colors,
    const std::vector<gfx::vertex> &vertices)
{
    float r = 1.0;
    float g = 1.0;
    float b = 1.0;
    if (cn.color_index >= 0 && cn.color_index < colors.size()) {
        r *= colors[cn.color_index].r;
        g *= colors[cn.color_index].g;
        b *= colors[cn.color_index].b;
        r /= 255.0;
        g /= 255.0;
        b /= 255.0;
    }
    if (cn.vertex_index >= 0 && cn.vertex_index < vertices.size()) {
        auto &&vertex = vertices[cn.vertex_index];
        if (vertex.color_index >= 0 && vertex.color_index < colors.size()) {
            r *= colors[vertex.color_index].r;
            g *= colors[vertex.color_index].g;
            b *= colors[vertex.color_index].b;
            r /= 255.0;
            g /= 255.0;
            b /= 255.0;
        }
        glColor3f(r, g, b);
        glVertex3fv(vertex.v);
    } else {
        glVertex3i(0, 0, 0);
    }
}

static void frame(edit::core &m_core, int delta)
{
    auto &&ns = m_core.m_proj.get_asset_root();

    gfx::world::ref sworld = edit::g_selected_asset;

    glPushMatrix();
    glRotatef(edit::g_camera_pitch, 1, 0, 0);
    glRotatef(edit::g_camera_yaw, 0, 1, 0);

    for (gfx::world::ref world : ns.get_asset_names()) {
        if (!world.ok())
            continue;

        auto model = world->get_model();
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
        if (sworld.ok()) {
            glTranslatef(-sworld->get_x(), -sworld->get_y(), -sworld->get_z());
        }
        glTranslatef(world->get_x(), world->get_y(), world->get_z());

        auto &&colors = mesh->get_colors();
        auto &&vertices = frame->get_vertices();

        glBegin(GL_TRIANGLES);
        for (auto &&p : mesh->get_triangles()) {
            dovert(p.v[0], colors, vertices);
            dovert(p.v[1], colors, vertices);
            dovert(p.v[2], colors, vertices);
        }
        glEnd();

        glBegin(GL_TRIANGLES);
        for (auto &&p : mesh->get_quads()) {
            dovert(p.v[0], colors, vertices);
            dovert(p.v[1], colors, vertices);
            dovert(p.v[2], colors, vertices);
            dovert(p.v[2], colors, vertices);
            dovert(p.v[1], colors, vertices);
            dovert(p.v[3], colors, vertices);
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
