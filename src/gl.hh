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

namespace drnsf {
namespace gl {

class machine : private util::nocopy {
public:
	using job = std::function<void()>;

private:
	std::list<job> m_pending_jobs;

protected:
	machine() = default;
	~machine() = default;

	void run_jobs();

	virtual void invalidate() = 0;

public:
	void post_job(job j);
};

class attrib : private util::nocopy {
	friend class program;
	friend class vert_array;

private:
	std::shared_ptr<int> m_id_p;

	explicit attrib(std::shared_ptr<int> id_p);

public:
	attrib(attrib &&src);

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
