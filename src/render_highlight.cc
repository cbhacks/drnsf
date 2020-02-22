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
#include "render.hh"

DRNSF_DECLARE_EMBED(shaders::highlight::vertex_glsl);
DRNSF_DECLARE_EMBED(shaders::highlight::fragment_glsl);

namespace drnsf {
namespace render {

// (s-var) s_prog
// The GL shader program to use for the highlight overlay.
static gl::program s_prog;

// (s-var) s_marking_uni
// The location of the "u_Marking" shader uniform variable.
static int s_marking_uni;

// declared in render.hh
void highlight::draw(unsigned int markingtexture)
{
    if (m_markings.empty())
        return;

    if (!s_prog.ok) {
        gl::vert_shader vs;
        gl::shader_source(vs, {
            "#version 140",
            embed::shaders::highlight::vertex_glsl::str
        });
        gl::compile_shader(vs);

        gl::frag_shader fs;
        gl::shader_source(fs, {
            "#version 140",
            embed::shaders::highlight::fragment_glsl::str
        });
        gl::compile_shader(fs);

        glAttachShader(s_prog, vs);
        glAttachShader(s_prog, fs);
        glBindFragDataLocation(s_prog, 0, "f_Color");
        gl::link_program(s_prog);
        s_marking_uni = glGetUniformLocation(s_prog, "u_Marking");
        glUseProgram(s_prog);
        glUniform1i(glGetUniformLocation(s_prog, "u_MarkingTexture"), 0);
        glUseProgram(0);

        s_prog.ok = true;
    }

    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    gl::vert_array vao;
    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, markingtexture);
    glUseProgram(s_prog);

    for (auto &marking : m_markings) {
        glUniform2i(s_marking_uni, marking.first, marking.second);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

// declared in render.hh
highlight::highlight(viewport &vp) :
    m_vp(vp)
{
    m_vp.m_highlights.push_back(this);
}

// declared in render.hh
highlight::~highlight()
{
    // Remove from the viewport.
    m_vp.m_highlights.erase(std::find(
        m_vp.m_highlights.begin(),
        m_vp.m_highlights.end(),
        this
    ));

    // Invalidate viewport if the highlight was potentially visible.
    if (!m_markings.empty()) {
        m_vp.invalidate(true);
    }
}

// declared in render.hh
void highlight::set(marker &m, int subindex)
{
    std::set<std::pair<int, int>> new_markings;
    new_markings.emplace(m.id(), subindex);
    std::swap(m_markings, new_markings);

    m_vp.invalidate(true);
}

// declared in render.hh
void highlight::add(marker &m, int subindex)
{
    m_markings.emplace(m.id(), subindex);
    m_vp.invalidate(true);
}

// declared in render.hh
void highlight::clear()
{
    if (m_markings.empty())
        return;

    m_markings.clear();
    m_vp.invalidate(true);
}

}
}
