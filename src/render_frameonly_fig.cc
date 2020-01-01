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
#include <glm/gtc/matrix_transform.hpp>
#include "render.hh"

DRNSF_DECLARE_EMBED(shaders::frameonly_fig::vertex_glsl);
DRNSF_DECLARE_EMBED(shaders::frameonly_fig::fragment_glsl);

namespace drnsf {
namespace render {

// (s-var) s_prog
// The GL shader program to use for the reticle model.
static gl::program s_prog;

// (s-var) s_matrix_uni
// The location of the "u_Matrix" shader uniform variable.
static int s_matrix_uni;

// declared in render.hh
void frameonly_fig::draw(const scene::env &e)
{
    if (!m_frame)
        return;

    if (!m_frame->m_vertices_buffer.ok) {
        glBindBuffer(GL_COPY_WRITE_BUFFER, m_frame->m_vertices_buffer);
        glBufferData(
            GL_COPY_WRITE_BUFFER,
            m_frame->get_vertices().size() * sizeof(gfx::vertex),
            m_frame->get_vertices().data(),
            GL_STATIC_DRAW
        );
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
        m_frame->m_vertices_buffer.ok = true;
    }

    if (!s_prog.ok) {
        gl::vert_shader vs;
        gl::shader_source(vs, {
            "#version 140",
            embed::shaders::frameonly_fig::vertex_glsl::str
        });
        gl::compile_shader(vs);

        gl::frag_shader fs;
        gl::shader_source(fs, {
            "#version 140",
            embed::shaders::frameonly_fig::fragment_glsl::str
        });
        gl::compile_shader(fs);

        glAttachShader(s_prog, vs);
        glAttachShader(s_prog, fs);
        glBindAttribLocation(s_prog, 0, "a_Position");
        glBindFragDataLocation(s_prog, 0, "f_Color");
        gl::link_program(s_prog);
        s_matrix_uni = glGetUniformLocation(s_prog, "u_Matrix");

        s_prog.ok = true;
    }

    glPointSize(4);
    DRNSF_ON_EXIT { glPointSize(1); };

    gl::vert_array vao;
    glBindVertexArray(vao);
    DRNSF_ON_EXIT { glBindVertexArray(0); };

    glBindBuffer(GL_ARRAY_BUFFER, m_frame->m_vertices_buffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_INT, false, sizeof(gfx::vertex), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(s_prog);
    auto matrix = e.projection * e.view * m_matrix;
    matrix = glm::scale(matrix, glm::vec3(
        m_frame->get_x_scale(),
        m_frame->get_y_scale(),
        m_frame->get_z_scale()
    ));
    glUniformMatrix4fv(s_matrix_uni, 1, false, &matrix[0][0]);
    glDrawArrays(GL_POINTS, 0, m_frame->get_vertices().size());
    glUseProgram(0);
}

// declared in render.hh
frameonly_fig::frameonly_fig(scene &scene) :
    figure(scene)
{
    h_frame_vertices_change <<= [this] {
        invalidate();
    };
    h_frame_x_scale_change <<= [this] {
        invalidate();
    };
    h_frame_y_scale_change <<= [this] {
        invalidate();
    };
    h_frame_z_scale_change <<= [this] {
        invalidate();
    };
}

gfx::frame * const &frameonly_fig::get_frame() const
{
    return m_frame;
}

void frameonly_fig::set_frame(gfx::frame *frame)
{
    if (m_frame != frame)
    {
        if (m_frame) {
            h_frame_vertices_change.unbind();
            h_frame_x_scale_change.unbind();
            h_frame_y_scale_change.unbind();
            h_frame_z_scale_change.unbind();
        }
        m_frame = frame;
        if (m_frame) {
            h_frame_vertices_change.bind(m_frame->p_vertices.on_change);
            h_frame_x_scale_change.bind(m_frame->p_x_scale.on_change);
            h_frame_y_scale_change.bind(m_frame->p_y_scale.on_change);
            h_frame_z_scale_change.bind(m_frame->p_z_scale.on_change);
        }
        invalidate();
    }
}

const glm::mat4 &frameonly_fig::get_matrix() const
{
    return m_matrix;
}

void frameonly_fig::set_matrix(glm::mat4 matrix)
{
    if (m_matrix != matrix) {
        m_matrix = matrix;
        invalidate();
    }
}

}
}
