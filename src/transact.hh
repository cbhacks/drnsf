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

#include <list>

#define TRANSACT ::drnsf::transact::teller &TS

namespace drnsf {
namespace transact {

class base_op_impl : private util::nocopy {
public:
	virtual void execute() noexcept = 0;

	virtual ~base_op_impl() = default;
};

template <typename T>
class assign_op_impl : public base_op_impl {
private:
	T &m_dest;
	T m_src;

public:
	template <typename T2>
	explicit assign_op_impl(T &dest,T2 src) :
		m_dest(dest),
		m_src(std::move(src)) {}

	void execute() noexcept override
	{
		using std::swap;
		swap(m_dest,m_src);
	}
};

class operation : private util::nocopy {
private:
	std::unique_ptr<base_op_impl> m_impl;

public:
	operation(operation &&src) :
		m_impl(std::move(src.m_impl)) {}

	operation(std::unique_ptr<base_op_impl> impl) :
		m_impl(std::move(impl)) {}

	void operator ()() const noexcept
	{
		m_impl->execute();
	}
};

class transaction : private util::nocopy {
	friend class nexus;
	friend class teller;

private:
	std::list<operation> m_ops;
	std::string m_desc;
	std::unique_ptr<transaction> m_next;

	explicit transaction(std::list<operation> ops,std::string desc);

public:
	const char *describe() const;
};

class teller : private util::nocopy {
	friend class nexus;

private:
	bool m_done;
	std::list<operation> m_ops;
	std::string m_desc;

	teller();
	~teller();

	std::unique_ptr<transaction> commit();

public:
	void describe(std::string desc);

	template <typename... Args>
	void describef(std::string fmt,Args... args)
	{
		describe(util::format(fmt,std::forward<Args>(args)...));
	}

	void push_op(operation op);

	template <typename T,typename T2>
	void set(T &dest,T2 src)
	{
		auto impl = new assign_op_impl<T>(dest,std::move(src));
		push_op(std::unique_ptr<base_op_impl>(impl));
	}
};

class nexus : private util::nocopy {
private:
	std::unique_ptr<transaction> m_undo;
	std::unique_ptr<transaction> m_redo;

public:
	bool has_undo() const;
	bool has_redo() const;

	const transaction &get_undo() const;
	const transaction &get_redo() const;

	void undo();
	void redo();

	nexus &operator <<(std::function<void(TRANSACT)> job);
};

}
}
