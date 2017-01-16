//
// WILLYMOD - An unofficial Crash Bandicoot level editor
// Copyright (C) 2016  WILLYMOD project contributors
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
#include "res.hh"

namespace res {

static const std::string null_name_str = "(null)";

name::name() :
	m_sym(nullptr)
{
}

name::name(nullptr_t null) :
	m_sym(nullptr)
{
}

name::name(const name &other) :
	m_sym(other.m_sym)
{
	if (m_sym) {
		m_sym->m_refcount++;
	}
}

name::name(name &&other) :
	m_sym(nullptr)
{
	using std::swap;
	swap(m_sym,other.m_sym);
}

name::name(space &ns,const std::string &str)
{
	auto iter = ns.m_map.find(str);
	if (iter != ns.m_map.end()) {
		m_sym = iter->second;
		m_sym->m_refcount++;
	} else {
		m_sym = new sym;
		try {
			m_sym->m_refcount = 1;
			m_sym->m_ns = &ns;
			m_sym->m_str = str;
			m_sym->m_asset = nullptr;
			auto insertion = ns.m_map.insert({str,m_sym});
			m_sym->m_map_iter = insertion.first;
		} catch (...) {
			delete m_sym;
			throw;
		}
	}
}

name::name(sym *sym) :
	m_sym(sym)
{
	sym->m_refcount++;
}

name::~name()
{
	*this = nullptr;
}

name::space &name::get_ns() const
{
	if (!m_sym)
		throw 0; // FIXME

	return *m_sym->m_ns;
}

const std::string &name::get_str() const
{
	if (m_sym) {
		return m_sym->m_str;
	} else {
		return null_name_str;
	}
}

asset &name::get_asset() const
{
	if (!m_sym) {
		throw 0; // FIXME
	}

	if (!m_sym->m_asset) {
		throw 0; // FIXME
	}

	return *m_sym->m_asset;
}

const char *name::c_str() const
{
	return get_str().c_str();
}

bool name::has_asset() const
{
	return (m_sym && m_sym->m_asset);
}

name &name::operator =(std::nullptr_t null)
{
	if (m_sym) {
		m_sym->m_refcount--;
		if (m_sym->m_refcount == 0) {
			if (m_sym->m_ns) {
				m_sym->m_ns->m_map.erase(m_sym->m_map_iter);
			}
			delete m_sym;
		}
		m_sym = nullptr;
	}

	return *this;
}

name &name::operator =(name other)
{
	using std::swap;
	swap(m_sym,other.m_sym);

	return *this;
}

bool name::operator ==(const name &other) const
{
	return m_sym == other.m_sym;
}

bool name::operator ==(std::nullptr_t null) const
{
	return m_sym == nullptr;
}

bool name::operator !=(const name &other) const
{
	return m_sym != other.m_sym;
}

bool name::operator !=(std::nullptr_t null) const
{
	return m_sym != nullptr;
}

name::operator bool() const
{
	return static_cast<bool>(m_sym);
}

bool name::operator !() const
{
	return !m_sym;
}

name name::operator /(const std::string &s) const
{
	if (!m_sym)
		throw 0; // FIXME

	return name(*m_sym->m_ns,m_sym->m_str + "/" + s);
}

name::space::~space()
{
	for (auto &kv : m_map) {
		kv.second->m_ns = nullptr;
	}
}

std::vector<name> name::space::get_asset_names()
{
	std::vector<name> names;

	for (auto &kv : m_map) {
		if (kv.second->m_asset) {
			names.emplace_back(kv.second);
		}
	}

	return names;
}

name name::space::operator /(const std::string &s)
{
	return name(*this,s);
}

std::string to_string(const name &name)
{
	return name.get_str();
}

}
