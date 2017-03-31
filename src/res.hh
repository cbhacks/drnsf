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

#include <map>
#include <vector>
#include "transact.hh"

#define DEFINE_APROP(name) \
	::res::prop<decltype(m_##name)> p_##name = { *this, m_##name }; \
	const decltype(m_##name) &get_##name() const \
	{ \
		return p_##name.get(); \
	} \
	void set_##name(TRANSACT,decltype(m_##name) value) \
	{ \
		p_##name.set(TS,std::move(value)); \
	}

namespace res {

class asset;
class ns;

class name {
	friend class asset;

private:
	struct sym;

public:
	class space : private util::nocopy {
		friend class name;

	private:
		std::map<std::string,sym *> m_map;

	public:
		space() = default;

		~space();

		std::vector<name> get_asset_names();

		name operator /(const std::string &s);
	};

private:
	struct sym {
		int m_refcount;

		space *m_ns;
		std::string m_str;
		decltype(space::m_map)::iterator m_map_iter;

		std::unique_ptr<asset> m_asset;
	};

	sym *m_sym;

public:
	name();
	name(nullptr_t null);
	name(const name &other);
	name(name &&other);
	explicit name(space &ns,const std::string &str);
	name(sym *sym);

	~name();

	auto get_ns() const -> space &;
	auto get_str() const -> const std::string &;
	auto get_asset() const -> asset &;

	const char *c_str() const;

	bool has_asset() const;

	template <typename T>
	bool is_a() const
	{
		if (!m_sym)
			return false;
		if (dynamic_cast<T*>(m_sym->m_asset.get()) == nullptr)
			return false;
		return true;
	}

	name &operator =(std::nullptr_t null);
	name &operator =(name other);

	bool operator ==(const name &other) const;
	bool operator ==(std::nullptr_t null) const;
	bool operator !=(const name &other) const;
	bool operator !=(std::nullptr_t null) const;
	operator bool() const;
	bool operator !() const;

	name operator /(const std::string &s) const;
};

std::string to_string(const name &name);

class asset : private util::nocopy {
private:
	name m_name;

protected:
	explicit asset(name name);

public:
	virtual ~asset() = default;

	void assert_alive() const;

	template <typename T>
	static void create(TRANSACT,name name)
	{
		if (!name)
			throw 0; // FIXME

		if (name.has_asset())
			throw 0; // FIXME

		std::unique_ptr<asset> t(new T(name));
		TS.set(t->m_name,name);
		TS.set(name.m_sym->m_asset,std::move(t));
	}

	void rename(TRANSACT,name name);
	void destroy(TRANSACT);

	const name &get_name() const;

	template <typename Reflector>
	void reflect(Reflector &rfl)
	{
	}
};

template <typename T>
class prop : private util::nocopy {
private:
	asset &m_owner;
	T &m_value;

public:
	prop(asset &owner,T &value) :
		m_owner(owner),
		m_value(value) {}

	const T &get() const
	{
		m_owner.assert_alive();
		return m_value;
	}

	void set(TRANSACT,T value)
	{
		m_owner.assert_alive();
		TS.set(m_value,std::move(value));
	}
};

template <typename T = asset>
class ref {
	template <typename T2>
	friend class res::ref;

private:
	name m_name;

public:
	ref() :
		m_name(nullptr) {}

	ref(std::nullptr_t null) :
		m_name(nullptr) {}

	template <typename T2>
	ref(const ref<T2> &other) :
		m_name(other.m_name) {}

	template <typename T2>
	ref(ref<T2> &&other) :
		m_name(std::move(other.m_name)) {}

	ref(name name) :
		m_name(name) {}

	ref(name::space &ns,const std::string &str) :
		m_name(ns,str) {}

	void create(TRANSACT) const
	{
		res::asset::create<T>(TS,*this);
	}

	const name &get_name() const
	{
		return m_name;
	}

	name::space &get_ns() const
	{
		return m_name.get_ns();
	}

	std::string get_str() const
	{
		return m_name.get_str();
	}

	const char *c_str() const
	{
		return m_name.c_str();
	}

	bool ok() const
	{
		return m_name.is_a<T>();
	}

	template <typename T2>
	ref &operator =(ref<T2> other)
	{
		m_name = std::move(other.m_name);
		return *this;
	}

	template <typename T2>
	bool operator ==(const ref<T2> &other) const
	{
		return (m_name == other.m_name);
	}

	bool operator==(const name &name) const
	{
		return (m_name == name);
	}

	bool operator==(std::nullptr_t null) const
	{
		return (m_name == nullptr);
	}

	template <typename T2>
	bool operator !=(const ref<T2> &other) const
	{
		return (m_name != other.m_name);
	}

	bool operator !=(const name &name) const
	{
		return (m_name != name);
	}

	bool operator !=(std::nullptr_t null) const
	{
		return (m_name != nullptr);
	}

	explicit operator bool() const
	{
		return static_cast<bool>(m_name);
	}

	bool operator !() const
	{
		return !m_name;
	}

	T *operator ->() const
	{
		return &operator *();
	}

	T &operator *() const
	{
		return dynamic_cast<T&>(m_name.get_asset());
	}

	operator name() const
	{
		return m_name;
	}
};

using anyref = ref<asset>;

}
