//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2019  DRNSF contributors
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

DRNSF_DECLARE_EMBED(shaders::zone_box_fig::vertex_glsl);
DRNSF_DECLARE_EMBED(shaders::zone_box_fig::fragment_glsl);

namespace drnsf {
namespace render {

// (s-var) s_model
// The model used by the box. This is used for an IBO, there are no vertex
// buffers. The vertex positions are derived from the vertex index.
static const unsigned char s_model[] = {
    // Square A
    0, 1,
    1, 3,
    3, 2,
    2, 0,

    // Square B
    4, 5,
    5, 7,
    7, 6,
    6, 4,

    // Lines between respective points of squares A and B
    0, 4,
    1, 5,
    2, 6,
    3, 7
};

// (s-var) s_ibo
// The IBO for the box model.
static gl::buffer s_ibo;

// (s-var) s_vao
// The VAO for the box model.
static gl::vert_array s_vao;

// (s-var) s_prog
// The GL shader program to use for the reticle model.
static gl::program s_prog;

// (s-var) s_matrix_uni
// The location of the "u_Matrix" shader uniform variable.
static int s_matrix_uni;

// declared in render.hh
void zone_box_fig::draw(const scene::env &e)
{
    if (!m_zone)
        return;

    if (!s_vao.ok) {
        glBindVertexArray(s_vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_ibo);
        glBindVertexArray(0);
        s_vao.ok = true;
    }

    if (!s_ibo.ok) {
        glBindBuffer(GL_COPY_WRITE_BUFFER, s_ibo);
        glBufferData(
            GL_COPY_WRITE_BUFFER,
            sizeof(s_model),
            s_model,
            GL_STATIC_DRAW
        );
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
        s_ibo.ok = true;
    }

    if (!s_prog.ok) {
        gl::vert_shader vs;
        gl::shader_source(vs, {
            "#version 140",
            embed::shaders::zone_box_fig::vertex_glsl::str
        });
        gl::compile_shader(vs);

        gl::frag_shader fs;
        gl::shader_source(fs, {
            "#version 140",
            embed::shaders::zone_box_fig::fragment_glsl::str
        });
        gl::compile_shader(fs);

        glAttachShader(s_prog, vs);
        glAttachShader(s_prog, fs);
        glBindFragDataLocation(s_prog, 0, "f_Color");
        gl::link_program(s_prog);
        s_matrix_uni = glGetUniformLocation(s_prog, "u_Matrix");

        s_prog.ok = true;
    }

    glPointSize(4);
    DRNSF_ON_EXIT { glPointSize(1); };

    glUseProgram(s_prog);
    auto matrix = e.projection * e.view * m_matrix;
    matrix = glm::translate(matrix, glm::vec3(
        m_zone->get_x(),
        m_zone->get_y(),
        m_zone->get_z()
    ));
    matrix = glm::scale(matrix, glm::vec3(
        m_zone->get_x_size(),
        m_zone->get_y_size(),
        m_zone->get_z_size()
    ));
    glUniformMatrix4fv(s_matrix_uni, 1, false, &matrix[0][0]);
    glBindVertexArray(s_vao);
    glDrawElements(GL_LINES, sizeof(s_model), GL_UNSIGNED_BYTE, nullptr);
    glBindVertexArray(0);
    glUseProgram(0);
}

// declared in render.hh
zone_box_fig::zone_box_fig(scene &scene) :
    figure(scene)
{
    h_zone_x_change <<= [this] {
        invalidate();
    };
    h_zone_y_change <<= [this] {
        invalidate();
    };
    h_zone_z_change <<= [this] {
        invalidate();
    };
    h_zone_x_size_change <<= [this] {
        invalidate();
    };
    h_zone_y_size_change <<= [this] {
        invalidate();
    };
    h_zone_z_size_change <<= [this] {
        invalidate();
    };
}

game::zone * const &zone_box_fig::get_zone() const
{
    return m_zone;
}

void zone_box_fig::set_zone(game::zone *zone)
{
    if (m_zone != zone)
    {
        if (m_zone) {
            h_zone_x_change.unbind();
            h_zone_y_change.unbind();
            h_zone_z_change.unbind();
            h_zone_x_size_change.unbind();
            h_zone_y_size_change.unbind();
            h_zone_z_size_change.unbind();
        }
        m_zone = zone;
        if (m_zone) {
            h_zone_x_change.bind(m_zone->p_x.on_change);
            h_zone_y_change.bind(m_zone->p_y.on_change);
            h_zone_z_change.bind(m_zone->p_z.on_change);
            h_zone_x_size_change.bind(m_zone->p_x_size.on_change);
            h_zone_y_size_change.bind(m_zone->p_y_size.on_change);
            h_zone_z_size_change.bind(m_zone->p_z_size.on_change);
        }
        invalidate();
    }
}

const glm::mat4 &zone_box_fig::get_matrix() const
{
    return m_matrix;
}

void zone_box_fig::set_matrix(glm::mat4 matrix)
{
    if (m_matrix != matrix) {
        m_matrix = matrix;
        invalidate();
    }
}

}
}
