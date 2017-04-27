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

#pragma once

#include <epoxy/gl.h>

namespace drnsf {
namespace gl {

void init();

class renderbuffer : private util::nocopy {
private:
	unsigned int m_id = 0;

public:
	renderbuffer() = default;

	renderbuffer(renderbuffer &&src)
	{
		std::swap(m_id,src.m_id);
	}

	~renderbuffer()
	{
		glDeleteRenderbuffers(1,&m_id);
	}

	renderbuffer &operator =(renderbuffer &&rhs)
	{
		std::swap(m_id,rhs.m_id);
		return *this;
	}

	operator decltype(m_id)() &
	{
		if (!m_id) {
			glGenRenderbuffers(1,&m_id);
		}
		return m_id;
	}
};

namespace old {

class machine : private util::nocopy {
protected:
	machine() = default;
	~machine() = default;

public:
	template <typename F>
	void post_job(F f)
	{
		f();
	}
};

class attrib {
	friend class program;
	friend class vert_array;

private:
	std::shared_ptr<int> m_id_p;

	explicit attrib(std::shared_ptr<int> id_p);

public:
	attrib() = default;
	attrib(const attrib &src) = default;
	attrib(attrib &&src) = default;

	attrib &operator =(const attrib &rhs) = default;
	attrib &operator =(attrib &&rhs) = default;

	int get_id();
};

class uniform {
	friend class program;

private:
	std::shared_ptr<int> m_id_p;

	explicit uniform(std::shared_ptr<int> id_p);

public:
	uniform() = default;
	uniform(const uniform &src) = default;
	uniform(uniform &&src) = default;

	uniform &operator =(const uniform &rhs) = default;
	uniform &operator =(uniform &&rhs) = default;

	int get_id();
};

class shader : private util::nocopy {
	friend class program;

private:
	machine &m_mach;
	std::shared_ptr<unsigned int> m_id_p;

public:
	explicit shader(machine &mach,int type);
	~shader();

	void compile(std::string code);
};

class program : private util::nocopy {
private:
	machine &m_mach;
	std::shared_ptr<unsigned int> m_id_p;

public:
	explicit program(machine &mach);
	~program();

	void attach(shader &sh);
	void detach(shader &sh);

	void link();

	attrib find_attrib(std::string name);
	uniform find_uniform(std::string name);

	unsigned int get_id();
};

class buffer : private util::nocopy {
	friend class vert_array;

private:
	machine &m_mach;
	std::shared_ptr<unsigned int> m_id_p;

public:
	explicit buffer(machine &mach);
	~buffer();

	void put_data(util::blob data,int usage);

	unsigned int get_id();
};

class texture : private util::nocopy {
private:
	machine &m_mach;
	int m_target;
	std::shared_ptr<unsigned int> m_id_p;

public:
	explicit texture(machine &mach,int target);
	~texture();

	void put_data_2d(
		util::blob data,
		int internal_format,
		int width,
		int height,
		int format,
		int type);

	void set_parameter(int pname,int value);

	unsigned int get_id();
};

class vert_array : private util::nocopy {
private:
	machine &m_mach;
	std::shared_ptr<unsigned int> m_id_p;

public:
	explicit vert_array(machine &mach);
	~vert_array();

	void bind_ibo(buffer &buf);

	void bind_vbo(
		buffer &buf,
		const attrib &atr,
		int size,
		int type,
		bool normalized,
		int stride,
		int offset);

	unsigned int get_id();
};

}
}
}
