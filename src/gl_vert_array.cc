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
#include "gl.hh"

namespace drnsf {
namespace gl {
namespace old {

vert_array::vert_array(machine &mach) :
    m_mach(mach),
    m_id_p(std::make_shared<unsigned int>(0))
{
    mach.post_job([id_p = m_id_p]{
        glGenVertexArrays(1,id_p.get());
    });
}

vert_array::~vert_array()
{
    m_mach.post_job([id_p = m_id_p]{
        glDeleteVertexArrays(1,id_p.get());
    });
}

void vert_array::bind_ibo(buffer &buf)
{
    m_mach.post_job([id_p = m_id_p,buf_id_p = buf.m_id_p]{
        glBindVertexArray(*id_p);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,*buf_id_p);
        glBindVertexArray(0);
    });
}

void vert_array::bind_vbo(
    buffer &buf,
    const attrib &atr,
    int size,
    int type,
    bool normalized,
    int stride,
    int offset)
{
    m_mach.post_job([
        id_p = m_id_p,
        buf_id_p = buf.m_id_p,
        atr_id_p = atr.m_id_p,
        size,
        type,
        normalized,
        stride,
        offset
        ]{
        glBindVertexArray(*id_p);
        glBindBuffer(GL_ARRAY_BUFFER,*buf_id_p);
        glEnableVertexAttribArray(*atr_id_p);
        glVertexAttribPointer(
            *atr_id_p,
            size,
            type,
            normalized,
            stride,
            reinterpret_cast<void *>(offset)
        );
        glBindBuffer(GL_ARRAY_BUFFER,0);
        glBindVertexArray(0);
    });
}

unsigned int vert_array::get_id()
{
    return *m_id_p;
}

}
}
}
