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

shader::shader(machine &mach,int type) :
    m_mach(mach),
    m_id_p(std::make_shared<unsigned int>(0))
{
    mach.post_job([id_p = m_id_p,type]{
        *id_p = glCreateShader(type);
    });
}

shader::~shader()
{
    m_mach.post_job([id_p = m_id_p]{
        glDeleteShader(*id_p);
    });
}

void shader::compile(std::string code)
{
    m_mach.post_job([id_p = m_id_p,code]{
        const char *code_cstr = code.c_str();
        glShaderSource(*id_p,1,&code_cstr,nullptr);
        glCompileShader(*id_p);

        int status;
        glGetShaderiv(*id_p,GL_COMPILE_STATUS,&status);

        if (!status) {
            int log_size;
            glGetShaderiv(*id_p,GL_INFO_LOG_LENGTH,&log_size);

            std::vector<char> log_buffer(log_size);
            glGetShaderInfoLog(
                *id_p,
                log_size,
                nullptr,
                log_buffer.data()
            );

            std::cerr << " == BEGIN SHADER LOG ==" << std::endl;
            std::cerr << log_buffer.data() << std::endl;
            std::cerr << " === END SHADER LOG ===" << std::endl;

            throw 0;//FIXME
        }
    });
}

}
}
}
