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
#include "render.hh"

namespace drnsf {

namespace embed {
    namespace meshframe_vert {
        extern const unsigned char data[];
        extern const std::size_t size;
    }
    namespace meshframe_frag {
        extern const unsigned char data[];
        extern const std::size_t size;
    }
}

namespace render {

const float cube_vb_data[] = {
    -1, -1, -1,
    +1, -1, -1,
    +1, +1, -1,
    -1, +1, -1,
    -1, -1, +1,
    +1, -1, +1,
    +1, +1, +1,
    -1, +1, +1
};

const unsigned char cube_ib_data[] = {
    0, 1,
    0, 2,
    0, 3,
    0, 4,
    0, 5,
    0, 6,
    0, 7,
    1, 2,
    1, 3,
    1, 4,
    1, 5,
    1, 6,
    1, 7,
    2, 3,
    2, 4,
    2, 5,
    2, 6,
    2, 7,
    3, 4,
    3, 5,
    3, 6,
    3, 7,
    4, 5,
    4, 6,
    4, 7,
    5, 6,
    5, 7,
    6, 7
};

// (s-var) s_vao
// The VAO for the reticle model.
static gl::vert_array s_vao;

// (s-var) s_vbo
// The VBO for the reticle model.
static gl::buffer s_vbo;

// (s-var) s_ibo
// The IBO for the reticle model.
static gl::buffer s_ibo;

// (s-var) s_prog
// The GL shader program to use for the reticle model.
static gl::program s_prog;

// (s-var) s_matrix_uni
// The location of the "u_Matrix" shader uniform variable.
static int s_matrix_uni;

// declared in render.hh
void meshframe_fig::draw(const env &e)
{
    if (!s_vao.ok()) {
        glBindVertexArray(s_vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_ibo);
        glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        s_vao.set_ok();
    }

    if (!s_vbo.ok()) {
        glBindBuffer(GL_COPY_WRITE_BUFFER, s_vbo);
        glBufferData(
            GL_COPY_WRITE_BUFFER,
            sizeof(cube_vb_data),
            cube_vb_data,
            GL_STATIC_DRAW
        );
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
        s_vbo.set_ok();
    }

    if (!s_ibo.ok()) {
        glBindBuffer(GL_COPY_WRITE_BUFFER, s_ibo);
        glBufferData(
            GL_COPY_WRITE_BUFFER,
            sizeof(cube_ib_data),
            cube_ib_data,
            GL_STATIC_DRAW
        );
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
        s_ibo.set_ok();
    }

    if (!s_prog.ok()) {
        gl::vert_shader vs;
        compile_shader(vs, {
            reinterpret_cast<const char *>(embed::meshframe_vert::data),
            embed::meshframe_vert::size
        });
        glAttachShader(s_prog, vs);

        gl::frag_shader fs;
        compile_shader(fs, {
            reinterpret_cast<const char *>(embed::meshframe_frag::data),
            embed::meshframe_frag::size
        });
        glAttachShader(s_prog, fs);

        glLinkProgram(s_prog);
        s_matrix_uni = glGetUniformLocation(s_prog, "u_Matrix");
        s_prog.set_ok();
    }

    glUseProgram(s_prog);
    glUniformMatrix4fv(s_matrix_uni, 1, false, &e.matrix[0][0]);
    glBindVertexArray(s_vao);
    glDrawElements(GL_LINES, 56, GL_UNSIGNED_BYTE, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

}
}
