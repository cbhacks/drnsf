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
#include "render.hh"

DRNSF_DECLARE_EMBED(meshframe_triangle_vert);
DRNSF_DECLARE_EMBED(meshframe_quad_vert);
DRNSF_DECLARE_EMBED(meshframe_vert);
DRNSF_DECLARE_EMBED(meshframe_frag);

namespace drnsf {
namespace render {

namespace {

// (s-var) s_triangle_prog, s_quad_prog
// These are the "poly shaders" used when rendering the models. Whole polygons
// are fed into the appropriate type of poly shader, which produces the vertex
// attributes used by the main shader program below (`s_main_prog'). This is not
// to be confused with a "geometry shader" which is not used here.
//
// The poly shader is actually a vertex-only shader program. Each polygon is
// intended to be fed as a single GL_POINTS vertex into the poly shader. Using
// transform feedback, the "varying" outputs are dumped into an interrim buffer
// object (`s_interrim_vb' below) which is used by the main program afterwards.
gl::program s_triangle_prog;
gl::program s_quad_prog;

// (s-var) s_main_prog
// The GL shader program used to render the model. This includes a vertex and
// fragment shader. The inputs do not come from any existing asset data, but are
// instead computed by another shader using transform feedback (see above).
gl::program s_main_prog;

// (s-var) s_matrix_uni
// The location of the "u_Matrix" shader uniform variable.
int s_matrix_uni;

// (s-var) s_interrim_vb, s_interrim_va
// This is the interrim VBO which is filled by the "poly shader" (see above)
// and then given to the main shader program as a source of vertex attributes.
// attributes. The buffer should be empty except during execution of the `draw'
// function (below).
//
// FIXME - explain VAO
gl::buffer s_interrim_vb;
gl::vert_array s_interrim_va;

// (internal type) interrim_vertex
// This is the type of each vertex which is produced by the poly shader and
// consumed as a set of vertex attributes by the vertex shader.
struct interrim_vertex {
    int vertex_index;
    int color_index;
};

}

// declared in render.hh
void meshframe_fig::draw(const env &e)
{
    if (!m_mesh)
        return;

    if (!m_frame)
        return;

    if (!s_triangle_prog.ok) {
        gl::vert_shader vs;
        gl::compile_shader(
            vs,
            embed::meshframe_triangle_vert::data,
            embed::meshframe_triangle_vert::size
        );

        glAttachShader(s_triangle_prog, vs);
        glBindAttribLocation(s_triangle_prog, 0, "ai_VertexIndex0");
        glBindAttribLocation(s_triangle_prog, 1, "ai_ColorIndex0");
        glBindAttribLocation(s_triangle_prog, 2, "ai_VertexIndex1");
        glBindAttribLocation(s_triangle_prog, 3, "ai_ColorIndex1");
        glBindAttribLocation(s_triangle_prog, 4, "ai_VertexIndex2");
        glBindAttribLocation(s_triangle_prog, 5, "ai_ColorIndex2");
        const char *varyings[] = {
            "ao_VertexIndex0",
            "ao_ColorIndex0",
            "ao_VertexIndex1",
            "ao_ColorIndex1",
            "ao_VertexIndex2",
            "ao_ColorIndex2"
        };
        glTransformFeedbackVaryings(
            s_triangle_prog,
            sizeof(varyings) / sizeof(*varyings),
            varyings,
            GL_INTERLEAVED_ATTRIBS
        );
        gl::link_program(s_triangle_prog);

        s_triangle_prog.ok = true;
    }

    if (!s_quad_prog.ok) {
        gl::vert_shader vs;
        gl::compile_shader(
            vs,
            embed::meshframe_quad_vert::data,
            embed::meshframe_quad_vert::size
        );

        glAttachShader(s_quad_prog, vs);
        glBindAttribLocation(s_quad_prog, 0, "ai_VertexIndex0");
        glBindAttribLocation(s_quad_prog, 1, "ai_ColorIndex0");
        glBindAttribLocation(s_quad_prog, 2, "ai_VertexIndex1");
        glBindAttribLocation(s_quad_prog, 3, "ai_ColorIndex1");
        glBindAttribLocation(s_quad_prog, 4, "ai_VertexIndex2");
        glBindAttribLocation(s_quad_prog, 5, "ai_ColorIndex2");
        glBindAttribLocation(s_quad_prog, 6, "ai_VertexIndex3");
        glBindAttribLocation(s_quad_prog, 7, "ai_ColorIndex3");
        const char *varyings[] = {
            "ao_VertexIndexA0",
            "ao_ColorIndexA0",
            "ao_VertexIndexA1",
            "ao_ColorIndexA1",
            "ao_VertexIndexA2",
            "ao_ColorIndexA2",
            "ao_VertexIndexB0",
            "ao_ColorIndexB0",
            "ao_VertexIndexB1",
            "ao_ColorIndexB1",
            "ao_VertexIndexB2",
            "ao_ColorIndexB2"
        };
        glTransformFeedbackVaryings(
            s_quad_prog,
            sizeof(varyings) / sizeof(*varyings),
            varyings,
            GL_INTERLEAVED_ATTRIBS
        );
        gl::link_program(s_quad_prog);

        s_quad_prog.ok = true;
    }

    if (!s_main_prog.ok) {
        gl::vert_shader vs;
        gl::compile_shader(
            vs,
            embed::meshframe_vert::data,
            embed::meshframe_vert::size
        );

        gl::frag_shader fs;
        gl::compile_shader(
            fs,
            embed::meshframe_frag::data,
            embed::meshframe_frag::size
        );

        glAttachShader(s_main_prog, vs);
        glAttachShader(s_main_prog, fs);
        glBindAttribLocation(s_main_prog, 0, "a_VertexIndex");
        glBindAttribLocation(s_main_prog, 1, "a_ColorIndex");
        glBindFragDataLocation(s_main_prog, 0, "f_Color");
        gl::link_program(s_main_prog);
        s_matrix_uni = glGetUniformLocation(s_main_prog, "u_Matrix");
        glUseProgram(s_main_prog);
        glUniform1i(glGetUniformLocation(s_main_prog, "u_VertexList"), 0);
        glUniform1i(glGetUniformLocation(s_main_prog, "u_ColorList"), 1);
        glUseProgram(0);

        s_main_prog.ok = true;
    }

    if (!s_interrim_vb.ok) {
        glBindBuffer(GL_COPY_WRITE_BUFFER, s_interrim_vb);
        glBufferData(GL_COPY_WRITE_BUFFER, 0, nullptr, GL_STREAM_COPY);
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
        s_interrim_vb.ok = true;
    }

    if (!s_interrim_va.ok) {
        glBindVertexArray(s_interrim_va);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, s_interrim_vb);
        glVertexAttribIPointer(
            0,
            1,
            GL_INT,
            sizeof(interrim_vertex),
            reinterpret_cast<void *>(offsetof(interrim_vertex, vertex_index))
        );
        glEnableVertexAttribArray(1);
        glVertexAttribIPointer(
            1,
            1,
            GL_INT,
            sizeof(interrim_vertex),
            reinterpret_cast<void *>(offsetof(interrim_vertex, color_index))
        );
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        s_interrim_va.ok = true;
    }

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

    if (!m_frame->m_vertices_texture.ok) {
        glBindTexture(GL_TEXTURE_BUFFER, m_frame->m_vertices_texture);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_R32I, m_frame->m_vertices_buffer);
        glBindTexture(GL_TEXTURE_BUFFER, 0);
        m_frame->m_vertices_texture.ok = true;
    }

    if (!m_mesh->m_triangles_buffer.ok) {
        glBindBuffer(GL_COPY_WRITE_BUFFER, m_mesh->m_triangles_buffer);
        glBufferData(
            GL_COPY_WRITE_BUFFER,
            m_mesh->get_triangles().size() * sizeof(gfx::triangle),
            m_mesh->get_triangles().data(),
            GL_STATIC_DRAW
        );
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
        m_mesh->m_triangles_buffer.ok = true;
    }

    if (!m_mesh->m_triangles_va.ok) {
        glBindVertexArray(m_mesh->m_triangles_va);
        glBindBuffer(GL_ARRAY_BUFFER, m_mesh->m_triangles_buffer);
        for (int i = 0; i < 3; i++) {
            glEnableVertexAttribArray(i * 2 + 0);
            glVertexAttribIPointer(
                i * 2 + 0,
                1,
                GL_INT,
                sizeof(gfx::triangle),
                reinterpret_cast<void *>(
                    offsetof(gfx::triangle, v) +
                    sizeof(gfx::corner) * i +
                    offsetof(gfx::corner, vertex_index)
                )
            );
            glEnableVertexAttribArray(i * 2 + 1);
            glVertexAttribIPointer(
                i * 2 + 1,
                1,
                GL_INT,
                sizeof(gfx::triangle),
                reinterpret_cast<void *>(
                    offsetof(gfx::triangle, v) +
                    sizeof(gfx::corner) * i +
                    offsetof(gfx::corner, color_index)
                )
            );
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        m_mesh->m_triangles_va.ok = true;
    }

    if (!m_mesh->m_quads_buffer.ok) {
        glBindBuffer(GL_COPY_WRITE_BUFFER, m_mesh->m_quads_buffer);
        glBufferData(
            GL_COPY_WRITE_BUFFER,
            m_mesh->get_quads().size() * sizeof(gfx::quad),
            m_mesh->get_quads().data(),
            GL_STATIC_DRAW
        );
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
        m_mesh->m_quads_buffer.ok = true;
    }

    if (!m_mesh->m_quads_va.ok) {
        glBindVertexArray(m_mesh->m_quads_va);
        glBindBuffer(GL_ARRAY_BUFFER, m_mesh->m_quads_buffer);
        for (int i = 0; i < 4; i++) {
            glEnableVertexAttribArray(i * 2 + 0);
            glVertexAttribIPointer(
                i * 2 + 0,
                1,
                GL_INT,
                sizeof(gfx::quad),
                reinterpret_cast<void *>(
                    offsetof(gfx::quad, v) +
                    sizeof(gfx::corner) * i +
                    offsetof(gfx::corner, vertex_index)
                )
            );
            glEnableVertexAttribArray(i * 2 + 1);
            glVertexAttribIPointer(
                i * 2 + 1,
                1,
                GL_INT,
                sizeof(gfx::quad),
                reinterpret_cast<void *>(
                    offsetof(gfx::quad, v) +
                    sizeof(gfx::corner) * i +
                    offsetof(gfx::corner, color_index)
                )
            );
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        m_mesh->m_quads_va.ok = true;
    }

    if (!m_mesh->m_colors_buffer.ok) {
        glBindBuffer(GL_COPY_WRITE_BUFFER, m_mesh->m_colors_buffer);
        glBufferData(
            GL_COPY_WRITE_BUFFER,
            m_mesh->get_colors().size() * sizeof(gfx::color),
            m_mesh->get_colors().data(),
            GL_STATIC_DRAW
        );
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
        m_mesh->m_colors_buffer.ok = true;
    }

    if (!m_mesh->m_colors_texture.ok) {
        glBindTexture(GL_TEXTURE_BUFFER, m_mesh->m_colors_texture);
        glTexBuffer(GL_TEXTURE_BUFFER, GL_R8, m_mesh->m_colors_buffer);
        glBindTexture(GL_TEXTURE_BUFFER, 0);
        m_mesh->m_colors_texture.ok = true;
    }

    glUseProgram(s_main_prog);
    auto matrix = e.projection * e.view * m_matrix;
    glUniformMatrix4fv(s_matrix_uni, 1, false, &matrix[0][0]);
    glUniform1i(
        glGetUniformLocation(s_main_prog, "u_ColorCount"),
        m_mesh->get_colors().size()
    );
    glUseProgram(0);

    /* for (auto &&material : ? ? ?) not implemented yet */ {
        // Allocate space for the triangle vertices in the interrim VBO.
        glBindBuffer(GL_COPY_WRITE_BUFFER, s_interrim_vb);
        glBufferData(
            GL_COPY_WRITE_BUFFER,
            sizeof(interrim_vertex) * 3 * m_mesh->get_triangles().size(),
            nullptr,
            GL_STREAM_COPY
        );
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

        // Run the poly shader to produce vertex attributes for the triangles
        // in the VBO.
        glUseProgram(s_triangle_prog);
        glBindVertexArray(m_mesh->m_triangles_va);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, s_interrim_vb);
        glEnable(GL_RASTERIZER_DISCARD);
        glBeginTransformFeedback(GL_POINTS);
        glDrawArrays(GL_POINTS, 0, m_mesh->get_triangles().size());
        glEndTransformFeedback();
        glDisable(GL_RASTERIZER_DISCARD);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
        // FIXME - polygons with bad vertex indices should be excluded

        // Run the main shader program to render the triangles.
        glUseProgram(s_main_prog);
        glBindVertexArray(s_interrim_va);
        glBindTexture(GL_TEXTURE_BUFFER, m_frame->m_vertices_texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_BUFFER, m_mesh->m_colors_texture);
        glActiveTexture(GL_TEXTURE0);
        glDrawArrays(GL_TRIANGLES, 0, m_mesh->get_triangles().size() * 3);
        glBindTexture(GL_TEXTURE_BUFFER, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_BUFFER, 0);
        glActiveTexture(GL_TEXTURE0);

        // Allocate space for the quad vertices in the interrim VBO.
        glBindBuffer(GL_COPY_WRITE_BUFFER, s_interrim_vb);
        glBufferData(
            GL_COPY_WRITE_BUFFER,
            sizeof(interrim_vertex) * 6 * m_mesh->get_quads().size(),
            nullptr,
            GL_STREAM_COPY
        );
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

        // Run the poly shader again, this time to produce vertex attributes for
        // the quads.
        glUseProgram(s_quad_prog);
        glBindVertexArray(m_mesh->m_quads_va);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, s_interrim_vb);
        glEnable(GL_RASTERIZER_DISCARD);
        glBeginTransformFeedback(GL_POINTS);
        glDrawArrays(GL_POINTS, 0, m_mesh->get_quads().size());
        glEndTransformFeedback();
        glDisable(GL_RASTERIZER_DISCARD);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);

        // Run the main shader program again to render the quads.
        glUseProgram(s_main_prog);
        glBindVertexArray(s_interrim_va);
        glBindTexture(GL_TEXTURE_BUFFER, m_frame->m_vertices_texture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_BUFFER, m_mesh->m_colors_texture);
        glActiveTexture(GL_TEXTURE0);
        glDrawArrays(GL_TRIANGLES, 0, m_mesh->get_quads().size() * 6);
        glBindTexture(GL_TEXTURE_BUFFER, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_BUFFER, 0);
        glActiveTexture(GL_TEXTURE0);

        glUseProgram(0);
        glBindVertexArray(0);

        // Release the allocated space in the interrim VBO.
        glBindBuffer(GL_COPY_WRITE_BUFFER, s_interrim_vb);
        glBufferData(GL_COPY_WRITE_BUFFER, 0, nullptr, GL_STREAM_COPY);
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    }
}

// declared in render.hh
meshframe_fig::meshframe_fig(viewport &vp) :
    figure(vp)
{
    h_mesh_triangles_change <<= [this] {
        invalidate();
    };
    h_mesh_quads_change <<= [this] {
        invalidate();
    };
    h_mesh_colors_change <<= [this] {
        invalidate();
    };
    h_frame_vertices_change <<= [this] {
        invalidate();
    };
}

gfx::mesh * const &meshframe_fig::get_mesh() const
{
    return m_mesh;
}

void meshframe_fig::set_mesh(gfx::mesh *mesh)
{
    if (m_mesh != mesh) {
        if (m_mesh) {
            h_mesh_triangles_change.unbind();
            h_mesh_quads_change.unbind();
            h_mesh_colors_change.unbind();
        }
        m_mesh = mesh;
        if (m_mesh) {
            h_mesh_triangles_change.bind(m_mesh->p_triangles.on_change);
            h_mesh_quads_change.bind(m_mesh->p_quads.on_change);
            h_mesh_colors_change.bind(m_mesh->p_colors.on_change);
        }
        invalidate();
    }
}

gfx::frame * const &meshframe_fig::get_frame() const
{
    return m_frame;
}

void meshframe_fig::set_frame(gfx::frame *frame)
{
    if (m_frame != frame)
    {
        if (m_frame) {
            h_frame_vertices_change.unbind();
        }
        m_frame = frame;
        if (m_frame) {
            h_frame_vertices_change.bind(m_frame->p_vertices.on_change);
        }
        invalidate();
    }
}

const glm::mat4 &meshframe_fig::get_matrix() const
{
    return m_matrix;
}

void meshframe_fig::set_matrix(glm::mat4 matrix)
{
    if (m_matrix != matrix) {
        m_matrix = matrix;
        invalidate();
    }
}

}
}
