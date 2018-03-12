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
#include "render.hh"

DRNSF_DECLARE_EMBED(reticle_vert);
DRNSF_DECLARE_EMBED(reticle_frag);

namespace drnsf {
namespace render {

// (internal struct) reticle_vert
// The vertex attribute type for the reticle model.
struct reticle_vert {
    float pos[3];
    enum : signed char {
        AXIS_X,
        AXIS_Y,
        AXIS_Z,
        AXIS_CUBE = -1
    } axis;
};

// (s-var) reticle_model
// The model used by the reticle. It consists of three lines, each on a
// particular axis (x, y, z; listed as 0, 1, 2). These will be colored by the
// shader as (red, green, blue).
reticle_vert reticle_model[] = {
    { { -1.0f, 0.0f, 0.0f }, reticle_vert::AXIS_X },
    { { +1.0f, 0.0f, 0.0f }, reticle_vert::AXIS_X },
    { { 0.0f, -1.0f, 0.0f }, reticle_vert::AXIS_Y },
    { { 0.0f, +1.0f, 0.0f }, reticle_vert::AXIS_Y },
    { { 0.0f, 0.0f, -1.0f }, reticle_vert::AXIS_Z },
    { { 0.0f, 0.0f, +1.0f }, reticle_vert::AXIS_Z },
    { { -1.0f, -1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
    { { +1.0f, -1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
    { { +1.0f, -1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
    { { +1.0f, +1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
    { { +1.0f, +1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
    { { -1.0f, +1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
    { { -1.0f, +1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
    { { -1.0f, -1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
    { { -1.0f, -1.0f, +1.0f }, reticle_vert::AXIS_CUBE },
    { { +1.0f, -1.0f, +1.0f }, reticle_vert::AXIS_CUBE },
    { { +1.0f, -1.0f, +1.0f }, reticle_vert::AXIS_CUBE },
    { { +1.0f, +1.0f, +1.0f }, reticle_vert::AXIS_CUBE },
    { { +1.0f, +1.0f, +1.0f }, reticle_vert::AXIS_CUBE },
    { { -1.0f, +1.0f, +1.0f }, reticle_vert::AXIS_CUBE },
    { { -1.0f, +1.0f, +1.0f }, reticle_vert::AXIS_CUBE },
    { { -1.0f, -1.0f, +1.0f }, reticle_vert::AXIS_CUBE },
    { { -1.0f, -1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
    { { -1.0f, -1.0f, +1.0f }, reticle_vert::AXIS_CUBE },
    { { +1.0f, -1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
    { { +1.0f, -1.0f, +1.0f }, reticle_vert::AXIS_CUBE },
    { { +1.0f, +1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
    { { +1.0f, +1.0f, +1.0f }, reticle_vert::AXIS_CUBE },
    { { -1.0f, +1.0f, -1.0f }, reticle_vert::AXIS_CUBE },
    { { -1.0f, +1.0f, +1.0f }, reticle_vert::AXIS_CUBE }
};

// (s-var) s_vao
// The VAO for the reticle model.
static gl::vert_array s_vao;

// (s-var) s_vbo
// The VBO for the reticle model.
static gl::buffer s_vbo;

// (s-var) s_prog
// The GL shader program to use for the reticle model.
static gl::program s_prog;

// (s-var) s_matrix_uni
// The location of the "u_Matrix" shader uniform variable.
static int s_matrix_uni;

// declared in render.hh
void reticle_fig::draw(const env &e)
{
    if (!s_vao.ok()) {
        glBindVertexArray(s_vao);
        glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            false,
            sizeof(reticle_vert),
            reinterpret_cast<void *>(offsetof(reticle_vert, pos))
        );
        glEnableVertexAttribArray(1);
        glVertexAttribIPointer(
            1,
            1,
            GL_BYTE,
            sizeof(reticle_vert),
            reinterpret_cast<void *>(offsetof(reticle_vert, axis))
        );
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        s_vao.set_ok();
    }

    if (!s_vbo.ok()) {
        glBindBuffer(GL_COPY_WRITE_BUFFER, s_vbo);
        glBufferData(
            GL_COPY_WRITE_BUFFER,
            sizeof(reticle_model),
            reticle_model,
            GL_STATIC_DRAW
        );
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
        s_vbo.set_ok();
    }

    if (!s_prog.ok()) {
        gl::vert_shader vs;
        compile_shader(
            vs,
            embed::reticle_vert::data,
            embed::reticle_vert::size
        );

        gl::frag_shader fs;
        compile_shader(
            fs,
            embed::reticle_frag::data,
            embed::reticle_frag::size
        );

        glAttachShader(s_prog, vs);
        glAttachShader(s_prog, fs);
        glBindAttribLocation(s_prog, 0, "a_Position");
        glBindAttribLocation(s_prog, 1, "a_Axis");
        glBindFragDataLocation(s_prog, 0, "f_Color");
        glLinkProgram(s_prog);
        s_matrix_uni = glGetUniformLocation(s_prog, "u_Matrix");

        s_prog.set_ok();
    }

    glUseProgram(s_prog);
    auto matrix = e.projection * e.view * m_matrix;
    glUniformMatrix4fv(s_matrix_uni, 1, false, &matrix[0][0]);
    glBindVertexArray(s_vao);
    glDrawArrays(GL_LINES, 0, sizeof(reticle_model) / sizeof(reticle_vert));
    glBindVertexArray(0);
    glUseProgram(0);
}

const glm::mat4 &reticle_fig::get_matrix() const
{
    return m_matrix;
}

void reticle_fig::set_matrix(glm::mat4 matrix)
{
    if (m_matrix != matrix) {
        m_matrix = matrix;
        invalidate();
    }
}

}
}
