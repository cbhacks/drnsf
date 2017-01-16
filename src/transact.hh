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

#define TRANSACT transact::teller &ts

namespace transact {

typedef std::function<void()> operation;

class transaction : util::not_copyable {
	friend class nexus;
	friend class teller;

private:
	std::list<operation> m_ops;
	std::string m_desc;
	std::unique_ptr<transaction> m_next;

	explicit transaction(std::list<operation> &&ops,std::string desc);

public:
	const char *describe() const;
};

class teller : util::not_copyable {
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
		auto newsrc = std::make_shared<T>(std::move(src));
		push_op([&dest,newsrc]() {
			using std::swap;
			swap(dest,*newsrc);
		});
	}

	template <typename T>
	void swap(T &a,T &b)
	{
		push_op([&a,&b]() {
			using std::swap;
			swap(a,b);
		});
	}
};

class nexus : util::not_copyable {
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
