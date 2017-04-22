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

template <typename T>
class insert_op_impl : public base_op_impl {
private:
	std::list<T> m_temp;
	typename std::list<T>::iterator m_value_iter;
	std::list<T> *m_dest_list;
	typename std::list<T>::iterator m_dest_iter;
	std::list<T> *m_src_list;
	typename std::list<T>::iterator m_src_iter;

public:
	explicit insert_op_impl(
		std::list<T> &list,
		typename std::list<T>::iterator pos,
		T value) :
		m_dest_list(&list),
		m_dest_iter(pos),
		m_src_list(&m_temp),
		m_src_iter(m_temp.end())
	{
		m_temp.push_back(std::move(value));
		m_value_iter = m_temp.begin();
	}

	void execute() noexcept override
	{
		m_dest_list->splice(m_dest_iter,*m_src_list,m_value_iter);
		std::swap(m_dest_list,m_src_list);
		std::swap(m_dest_iter,m_src_iter);
	}

	typename std::list<T>::iterator get_value_iterator() const
	{
		return m_value_iter;
	}
};

template <typename T>
class erase_op_impl : public base_op_impl {
private:
	std::list<T> m_temp;
	typename std::list<T>::iterator m_value_iter;
	std::list<T> *m_dest_list;
	typename std::list<T>::iterator m_dest_iter;
	std::list<T> *m_src_list;
	typename std::list<T>::iterator m_src_iter;

public:
	explicit erase_op_impl(
		std::list<T> &list,
		typename std::list<T>::iterator pos) :
		m_value_iter(pos),
		m_dest_list(&m_temp),
		m_dest_iter(m_temp.end()),
		m_src_list(&list),
		m_src_iter(std::next(pos)) {}

	void execute() noexcept override
	{
		m_dest_list->splice(m_dest_iter,*m_src_list,m_value_iter);
		std::swap(m_dest_list,m_src_list);
		std::swap(m_dest_iter,m_src_iter);
	}
};

class transaction : private util::nocopy {
	friend class nexus;
	friend class teller;

private:
	std::list<std::unique_ptr<base_op_impl>> m_ops;
	std::string m_desc;
	std::unique_ptr<transaction> m_next;

	explicit transaction(
		std::list<std::unique_ptr<base_op_impl>> ops,
		std::string desc);

public:
	const char *describe() const;
};

class teller : private util::nocopy {
	friend class nexus;

private:
	bool m_done;
	std::list<std::unique_ptr<base_op_impl>> m_ops;
	std::string m_desc;

	teller();
	~teller();

	std::unique_ptr<transaction> commit();

public:
	void describe(std::string desc);

	void push_op(std::unique_ptr<base_op_impl> op);

	template <typename T,typename T2>
	void set(T &dest,T2 src)
	{
		auto impl = new assign_op_impl<T>(dest,std::move(src));
		push_op(std::unique_ptr<base_op_impl>(impl));
	}

	template <typename T,typename T2>
	typename std::list<T>::iterator insert(
		std::list<T> &list,
		typename std::list<T>::iterator pos,
		T2 value)
	{
		auto impl = new insert_op_impl<T>(list,pos,std::move(value));
		push_op(std::unique_ptr<base_op_impl>(impl));
		return impl->get_value_iterator();
	}

	template <typename T>
	void erase(std::list<T> &list,typename std::list<T>::iterator pos)
	{
		auto impl = new erase_op_impl<T>(list,pos);
		push_op(std::unique_ptr<base_op_impl>(impl));
	}
};

enum class status {
	ready,
	busy,
	failed
};

class nexus : private util::nocopy {
private:
	status m_status;
	std::unique_ptr<transaction> m_undo;
	std::unique_ptr<transaction> m_redo;

public:
	nexus();
	~nexus();

	status get_status() const;

	bool has_undo() const;
	bool has_redo() const;

	const transaction &get_undo() const;
	const transaction &get_redo() const;

	void undo();
	void redo();

	nexus &operator <<(std::function<void(TRANSACT)> job);

	util::event<> on_status_change;
};

}
}
