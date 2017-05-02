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

/*
 * gl.hh
 *
 * FIXME explain
 */

#include <epoxy/gl.h>

namespace drnsf {
namespace gl {

/*
 * gl::init
 *
 * FIXME explain
 */
void init();

/*
 * gl::texture
 *
 * FIXME explain
 */
class texture : private util::nocopy {
private:
	// (var) m_id
	// FIXME explain
	unsigned int m_id = 0;

public:
	// (default ctor)
	// FIXME explain
	texture() = default;

	// (move ctor)
	// FIXME explain
	texture(texture &&src)
	{
		std::swap(m_id,src.m_id);
	}

	// (dtor)
	// FIXME explain
	~texture()
	{
		glDeleteTextures(1,&m_id);
	}

	// (move-assignment operator)
	// FIXME explain
	texture &operator =(texture &&rhs)
	{
		std::swap(m_id,rhs.m_id);
		return *this;
	}

	// (conversion operator)
	// FIXME explain
	operator decltype(m_id)() &
	{
		if (!m_id) {
			glGenTextures(1,&m_id);
		}
		return m_id;
	}
};

/*
 * gl::buffer
 *
 * FIXME explain
 */
class buffer : private util::nocopy {
private:
	// (var) m_id
	// FIXME explain
	unsigned int m_id = 0;

public:
	// (default ctor)
	// FIXME explain
	buffer() = default;

	// (move ctor)
	// FIXME explain
	buffer(buffer &&src)
	{
		std::swap(m_id,src.m_id);
	}

	// (dtor)
	// FIXME explain
	~buffer()
	{
		glDeleteBuffers(1,&m_id);
	}

	// (move-assignment operator)
	// FIXME explain
	buffer &operator =(buffer &&rhs)
	{
		std::swap(m_id,rhs.m_id);
		return *this;
	}

	// (conversion operator)
	// FIXME explain
	operator decltype(m_id)() &
	{
		if (!m_id) {
			glGenBuffers(1,&m_id);
		}
		return m_id;
	}
};

/*
 * gl::vert_array
 *
 * FIXME explain
 */
class vert_array : private util::nocopy {
private:
	// (var) m_id
	// FIXME explain
	unsigned int m_id = 0;

public:
	// (default ctor)
	// FIXME explain
	vert_array() = default;

	// (move ctor)
	// FIXME explain
	vert_array(vert_array &&src)
	{
		std::swap(m_id,src.m_id);
	}

	// (dtor)
	// FIXME explain
	~vert_array()
	{
		glDeleteVertexArrays(1,&m_id);
	}

	// (move-assignment operator)
	// FIXME explain
	vert_array &operator =(vert_array &&rhs)
	{
		std::swap(m_id,rhs.m_id);
		return *this;
	}

	// (conversion operator)
	// FIXME explain
	operator decltype(m_id)() &
	{
		if (!m_id) {
			glGenVertexArrays(1,&m_id);
		}
		return m_id;
	}
};

/*
 * gl::renderbuffer
 *
 * FIXME explain
 */
class renderbuffer : private util::nocopy {
private:
	// (var) m_id
	// FIXME explain
	unsigned int m_id = 0;

public:
	// (default ctor)
	// FIXME explain
	renderbuffer() = default;

	// (move ctor)
	// FIXME explain
	renderbuffer(renderbuffer &&src)
	{
		std::swap(m_id,src.m_id);
	}

	// (dtor)
	// FIXME explain
	~renderbuffer()
	{
		glDeleteRenderbuffers(1,&m_id);
	}

	// (move-assignment operator)
	// FIXME explain
	renderbuffer &operator =(renderbuffer &&rhs)
	{
		std::swap(m_id,rhs.m_id);
		return *this;
	}

	// (conversion operator)
	// FIXME explain
	operator decltype(m_id)() &
	{
		if (!m_id) {
			glGenRenderbuffers(1,&m_id);
		}
		return m_id;
	}
};

/*
 * gl::framebuffer
 *
 * FIXME explain
 */
class framebuffer : private util::nocopy {
private:
	// (var) m_id
	// FIXME explain
	unsigned int m_id = 0;

public:
	// (default ctor)
	// FIXME explain
	framebuffer() = default;

	// (move ctor)
	// FIXME explain
	framebuffer(framebuffer &&src)
	{
		std::swap(m_id,src.m_id);
	}

	// (dtor)
	// FIXME explain
	~framebuffer()
	{
		glDeleteFramebuffers(1,&m_id);
	}

	// (move-assignment operator)
	// FIXME explain
	framebuffer &operator =(framebuffer &&rhs)
	{
		std::swap(m_id,rhs.m_id);
		return *this;
	}

	// (conversion operation)
	// FIXME explain
	operator decltype(m_id)() &
	{
		if (!m_id) {
			glGenFramebuffers(1,&m_id);
		}
		return m_id;
	}
};

/*
 * gl::program
 *
 * FIXME explain
 */
class program : private util::nocopy {
private:
	// (var) m_id
	// FIXME explain
	unsigned int m_id = 0;

public:
	// (default ctor)
	// FIXME explain
	program() = default;

	// (move ctor)
	// FIXME explain
	program(program &&src)
	{
		std::swap(m_id,src.m_id);
	}

	// (dtor)
	// FIXME explain
	~program()
	{
		glDeleteProgram(m_id);
	}

	// (move-assignment operator)
	// FIXME explain
	program &operator =(program &&rhs)
	{
		std::swap(m_id,rhs.m_id);
		return *this;
	}

	// (conversion operator)
	// FIXME explain
	operator decltype(m_id)() &
	{
		if (!m_id) {
			m_id = glCreateProgram();
		}
		return m_id;
	}
};

/*
 * gl::shader
 *
 * FIXME explain
 */
template <int Type>
class shader : private util::nocopy {
private:
	// (var) m_id
	// FIXME explain
	unsigned int m_id = 0;

public:
	// (default ctor)
	// FIXME explain
	shader() = default;

	// (move ctor)
	// FIXME explain
	shader(shader &&src)
	{
		std::swap(m_id,src.m_id);
	}

	// (dtor)
	// FIXME explain
	~shader()
	{
		glDeleteShader(m_id);
	}

	// (move-assignment operator)
	// FIXME explain
	shader &operator =(shader &&rhs)
	{
		std::swap(m_id,rhs.m_id);
		return *this;
	}

	// (conversion operator)
	// FIXME explain
	operator decltype(m_id)() &
	{
		if (!m_id) {
			m_id = glCreateShader(Type);
		}
		return m_id;
	}

	// (func) compile
	// FIXME explain
	void compile(const std::string &code)
	{
		const char *code_cstr = code.c_str();
		glShaderSource(*this,1,&code_cstr,nullptr);
		glCompileShader(*this);

		int status;
		glGetShaderiv(*this,GL_COMPILE_STATUS,&status);

		if (!status) {
			int log_size;
			glGetShaderiv(*this,GL_INFO_LOG_LENGTH,&log_size);

			std::vector<char> log_buffer(log_size);
			glGetShaderInfoLog(
				*this,
				log_size,
				nullptr,
				log_buffer.data()
			);

			fprintf(stderr," == BEGIN SHADER COMPILE LOG ==\n");
			fprintf(stderr,"%s\n",log_buffer.data());
			fprintf(stderr," === END SHADER COMPILE LOG ===\n");

			throw 0;//FIXME
		}
	}
};

/*
 * gl::vert_shader
 *
 * FIXME explain
 */
using vert_shader = shader<GL_VERTEX_SHADER>;

/*
 * gl::frag_shader
 *
 * FIXME explain
 */
using frag_shader = shader<GL_FRAGMENT_SHADER>;

// FIXME obsolete
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
