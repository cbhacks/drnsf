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
#include <epoxy/gl.h>
#include "gl.hh"

namespace drnsf {
namespace gl {
namespace old {

buffer::buffer(machine &mach) :
	m_mach(mach),
	m_id_p(std::make_shared<unsigned int>(0))
{
	mach.post_job([id_p = m_id_p]{
		glGenBuffers(1,id_p.get());
	});
}

buffer::~buffer()
{
	m_mach.post_job([id_p = m_id_p]{
		glDeleteBuffers(1,id_p.get());
	});
}

void buffer::put_data(util::blob data,int usage)
{
	m_mach.post_job([id_p = m_id_p,data,usage]{
		glBindBuffer(GL_COPY_WRITE_BUFFER,*id_p);
		glBufferData(
			GL_COPY_WRITE_BUFFER,
			data.size(),
			data.data(),
			usage
		);
		glBindBuffer(GL_COPY_WRITE_BUFFER,0);
	});
}

unsigned int buffer::get_id()
{
	return *m_id_p;
}

}
}
}
