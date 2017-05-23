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
#include <iostream>
#include "gl.hh"

namespace drnsf {
namespace gl {
namespace old {

program::program(machine &mach) :
    m_mach(mach),
    m_id_p(std::make_shared<unsigned int>(0))
{
    mach.post_job([id_p = m_id_p]{
        *id_p = glCreateProgram();
    });
}

program::~program()
{
    m_mach.post_job([id_p = m_id_p]{
        glDeleteProgram(*id_p);
    });
}

void program::attach(shader &sh)
{
    m_mach.post_job([id_p = m_id_p,sh_id_p = sh.m_id_p]{
        glAttachShader(*id_p,*sh_id_p);
    });
}

void program::detach(shader &sh)
{
    m_mach.post_job([id_p = m_id_p,sh_id_p = sh.m_id_p]{
        glDetachShader(*id_p,*sh_id_p);
    });
}

void program::link()
{
    m_mach.post_job([id_p = m_id_p]{
        glLinkProgram(*id_p);

        int status;
        glGetProgramiv(*id_p,GL_LINK_STATUS,&status);

        if (!status) {
            int log_size;
            glGetProgramiv(*id_p,GL_INFO_LOG_LENGTH,&log_size);

            std::vector<char> log_buffer(log_size);
            glGetProgramInfoLog(
                *id_p,
                log_size,
                nullptr,
                log_buffer.data()
            );

            std::cerr << " == BEGIN PROGRAM LOG ==" << std::endl;
            std::cerr << log_buffer.data() << std::endl;
            std::cerr << " === END PROGRAM LOG ===" << std::endl;

            throw 0;//FIXME
        }
    });
}

unsigned int program::get_id()
{
    return *m_id_p;
}

attrib program::find_attrib(std::string name)
{
    auto atr_id_p = std::make_shared<int>(-1);
    m_mach.post_job([id_p = m_id_p,name,atr_id_p]{
        *atr_id_p = glGetAttribLocation(*id_p,name.c_str());
    });
    return attrib(atr_id_p);
}

uniform program::find_uniform(std::string name)
{
    auto uni_id_p = std::make_shared<int>(-1);
    m_mach.post_job([id_p = m_id_p,name,uni_id_p]{
        *uni_id_p = glGetUniformLocation(*id_p,name.c_str());
    });
    return uniform(uni_id_p);
}

}
}
}
