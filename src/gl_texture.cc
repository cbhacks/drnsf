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

texture::texture(machine &mach,int target) :
	m_mach(mach),
	m_target(target),
	m_id_p(std::make_shared<unsigned int>(0))
{
	mach.post_job([id_p = m_id_p]{
		glGenTextures(1,id_p.get());
	});
}

texture::~texture()
{
	m_mach.post_job([id_p = m_id_p]{
		glDeleteTextures(1,id_p.get());
	});
}

void texture::put_data_2d(
	util::blob data,
	int internal_format,
	int width,
	int height,
	int format,
	int type)
{
	m_mach.post_job([
		id_p = m_id_p,
		target = m_target,
		data,
		internal_format,
		width,
		height,
		format,
		type
		]{
		glBindTexture(target,*id_p);
		glTexImage2D(
			target,
			0,
			internal_format,
			width,
			height,
			0,
			format,
			type,
			data.data()
		);
		glBindTexture(target,0);
	});
}

void texture::set_parameter(int pname,int value)
{
	m_mach.post_job([
		id_p = m_id_p,
		target = m_target,
		pname,
		value
		]{
		glBindTexture(target,*id_p);
		glTexParameteri(target,pname,value);
		glBindTexture(target,0);
	});
}

unsigned int texture::get_id()
{
	return *m_id_p;
}

}
}
}
