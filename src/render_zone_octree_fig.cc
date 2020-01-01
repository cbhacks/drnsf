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

DRNSF_DECLARE_EMBED(shaders::zone_octree_fig::vertex_glsl);
DRNSF_DECLARE_EMBED(shaders::zone_octree_fig::fragment_glsl);

namespace drnsf {
namespace render {

// (s-var) s_prog
// The GL shader program to use for the reticle model.
static gl::program s_prog;

// (s-var) s_resolution_uni
// The location of the "u_Resolution" shader uniform variable.
static int s_resolution_uni;

// (s-var) s_matrix_uni
// The location of the "u_Matrix" shader uniform variable.
static int s_matrix_uni;

namespace {

// (internal type) bad_octree
// Special exception type for `octree_flattener' below.
class bad_octree {};

// (internal type) octree_flattener
// Used by `draw' to build the flattened 3D octree texture.
class octree_flattener {
private:
    enum {
        STEP_MAIN,
        STEP_BISECT_X,
        STEP_BISECT_Y,
        STEP_BISECT_Z
    };

    uint16_t root;
    const std::vector<uint16_t> &tree;

    int x_trueres;
    int y_trueres;
    int z_trueres;

    std::vector<uint16_t> result;

    void f(int step, uint16_t &i, int x, int y, int z, int w, int h, int d)
    {
        switch (step) {
        case STEP_BISECT_X:
            if (w > 1) {
                w >>= 1;
                f(STEP_BISECT_Z, i, x, y, z, w, h, d);
                x += w;
            }
            // fallthrough

        case STEP_BISECT_Z:
            if (d > 1) {
                d >>= 1;
                f(STEP_BISECT_Y, i, x, y, z, w, h, d);
                z += d;
            }
            // fallthrough

        case STEP_BISECT_Y:
            if (h > 1) {
                h >>= 1;
                f(STEP_MAIN, i, x, y, z, w, h, d);
                y += h;
            }
            // fallthrough

        case STEP_MAIN:
            uint16_t value;
            if (i == 0x1C) {
                value = root;
            } else if (i >= uint16_t(game::zone::octree_header_size)) {
                int index = (i - game::zone::octree_header_size) / 2;
                if (size_t(index) >= tree.size()) {
                    throw bad_octree{};
                }
                value = tree[index];
            } else {
                throw bad_octree{};
            }

            if (value & 1) {
                // Non-air leaf node, set all in region (x, y, z) ... (x+w, y+h, z+d).

                for (int zz = 0; zz < d; zz++)
                for (int yy = 0; yy < h; yy++)
                for (int xx = 0; xx < w; xx++) {
                    result[
                        (xx + x) +
                        (yy + y) * x_trueres +
                        (zz + z) * y_trueres * x_trueres
                    ] = i;
                }
            } else if (value != 0) {
                // Non-leaf node.

                f(STEP_BISECT_X, value, x, y, z, w, h, d);
            } // else == 0, air leaf node

            i += 2;

            break;
        }
    }

public:
    explicit octree_flattener(
        uint16_t root,
        const std::vector<uint16_t> &tree,
        int x_trueres,
        int y_trueres,
        int z_trueres
    ) :
        root(root),
        tree(tree),
        x_trueres(x_trueres),
        y_trueres(y_trueres),
        z_trueres(z_trueres),
        result(x_trueres * y_trueres * z_trueres)
    {
        uint16_t root_offset = 0x1C;
        f(STEP_MAIN, root_offset, 0, 0, 0, x_trueres, y_trueres, z_trueres);
    }

    void operator ()(std::vector<uint16_t> &out) &&
    {
        out = std::move(result);
    }
};

}

// declared in render.hh
void zone_octree_fig::draw(const scene::env &e)
{
    if (!m_zone)
        return;

    if (!s_prog.ok) {
        gl::vert_shader vs;
        gl::shader_source(vs, {
            "#version 140",
            embed::shaders::zone_octree_fig::vertex_glsl::str
        });
        gl::compile_shader(vs);

        gl::frag_shader fs;
        gl::shader_source(fs, {
            "#version 140",
            embed::shaders::zone_octree_fig::fragment_glsl::str
        });
        gl::compile_shader(fs);

        glAttachShader(s_prog, vs);
        glAttachShader(s_prog, fs);
        glBindFragDataLocation(s_prog, 0, "f_Color");
        gl::link_program(s_prog);
        s_matrix_uni = glGetUniformLocation(s_prog, "u_Matrix");
        s_resolution_uni = glGetUniformLocation(s_prog, "u_Resolution");
        glUseProgram(s_prog);
        glUniform1i(glGetUniformLocation(s_prog, "u_TreeTex"), 0);
        glUniform1i(glGetUniformLocation(s_prog, "u_FlatTex"), 1);
        glUseProgram(0);

        s_prog.ok = true;
    }

    if (!m_zone->m_octree_tree_buffer.ok) {
        glBindBuffer(GL_COPY_WRITE_BUFFER, m_zone->m_octree_tree_buffer);
        glBufferData(
            GL_COPY_WRITE_BUFFER,
            m_zone->get_octree().size() * sizeof(uint16_t),
            m_zone->get_octree().data(),
            GL_STATIC_DRAW
        );
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
        m_zone->m_octree_tree_buffer.ok = true;
    }

    if (!m_zone->m_octree_tree_texture.ok) {
        glBindTexture(GL_TEXTURE_BUFFER, m_zone->m_octree_tree_texture);
        glTexBuffer(
            GL_TEXTURE_BUFFER,
            GL_R16UI,
            m_zone->m_octree_tree_buffer
        );
        glBindTexture(GL_TEXTURE_BUFFER, 0);
        m_zone->m_octree_tree_texture.ok = true;
    }

    const int MAX_RESOLUTION = 12;

    if (m_zone->get_x_res() > MAX_RESOLUTION)
        return; // FIXME - warning?
    if (m_zone->get_y_res() > MAX_RESOLUTION)
        return; // FIXME - warning?
    if (m_zone->get_z_res() > MAX_RESOLUTION)
        return; // FIXME - warning?

    int x_trueres = 1 << m_zone->get_x_res();
    int y_trueres = 1 << m_zone->get_y_res();
    int z_trueres = 1 << m_zone->get_z_res();

    if (!m_zone->m_octree_flat_texture.ok) {
        std::vector<uint16_t> flattex;

        try {
            octree_flattener{
                m_zone->get_octree_root(),
                m_zone->get_octree(),
                x_trueres,
                y_trueres,
                z_trueres,
            }(flattex);
        } catch (bad_octree) {
            return; // FIXME - warning?
        }

        glBindTexture(GL_TEXTURE_3D, m_zone->m_octree_flat_texture);
        glTexImage3D(
            GL_TEXTURE_3D,
            0,
            GL_R16UI,
            x_trueres,
            y_trueres,
            z_trueres,
            0,
            GL_RED_INTEGER,
            GL_UNSIGNED_SHORT,
            flattex.data()
        );
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_3D, 0);

        m_zone->m_octree_flat_texture.ok = true;
    }

    gl::vert_array vao;
    glBindVertexArray(vao);
    DRNSF_ON_EXIT { glBindVertexArray(0); };

    glBindTexture(GL_TEXTURE_BUFFER, m_zone->m_octree_tree_texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, m_zone->m_octree_flat_texture);
    glActiveTexture(GL_TEXTURE0);
    DRNSF_ON_EXIT {
        glBindTexture(GL_TEXTURE_3D, 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_BUFFER, 0);
        glActiveTexture(GL_TEXTURE0);
    };

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
    glUniform3i(
        s_resolution_uni,
        x_trueres,
        y_trueres,
        z_trueres
    );
    glDrawArrays(GL_TRIANGLES, 0, 6 * (x_trueres + y_trueres + z_trueres + 3));
    glUseProgram(0);
}

// declared in render.hh
zone_octree_fig::zone_octree_fig(scene &scene) :
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
    h_zone_x_res_change <<= [this] {
        invalidate();
    };
    h_zone_y_res_change <<= [this] {
        invalidate();
    };
    h_zone_z_res_change <<= [this] {
        invalidate();
    };
    h_zone_octree_root_change <<= [this] {
        invalidate();
    };
    h_zone_octree_change <<= [this] {
        invalidate();
    };
}

game::zone * const &zone_octree_fig::get_zone() const
{
    return m_zone;
}

void zone_octree_fig::set_zone(game::zone *zone)
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
            h_zone_x_res_change.unbind();
            h_zone_y_res_change.unbind();
            h_zone_z_res_change.unbind();
            h_zone_octree_root_change.unbind();
            h_zone_octree_change.unbind();
        }
        m_zone = zone;
        if (m_zone) {
            h_zone_x_change.bind(m_zone->p_x.on_change);
            h_zone_y_change.bind(m_zone->p_y.on_change);
            h_zone_z_change.bind(m_zone->p_z.on_change);
            h_zone_x_size_change.bind(m_zone->p_x_size.on_change);
            h_zone_y_size_change.bind(m_zone->p_y_size.on_change);
            h_zone_z_size_change.bind(m_zone->p_z_size.on_change);
            h_zone_x_res_change.bind(m_zone->p_x_res.on_change);
            h_zone_y_res_change.bind(m_zone->p_y_res.on_change);
            h_zone_z_res_change.bind(m_zone->p_z_res.on_change);
            h_zone_octree_root_change.bind(m_zone->p_octree_root.on_change);
            h_zone_octree_change.bind(m_zone->p_octree.on_change);
        }
        invalidate();
    }
}

const glm::mat4 &zone_octree_fig::get_matrix() const
{
    return m_matrix;
}

void zone_octree_fig::set_matrix(glm::mat4 matrix)
{
    if (m_matrix != matrix) {
        m_matrix = matrix;
        invalidate();
    }
}

}
}
