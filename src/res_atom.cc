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
#include <cstring>
#include "res.hh"

namespace drnsf {
namespace res {

// (internal class) nucleus_name_comparator
// FIXME explain
struct nucleus_name_comparator {
	bool operator()(const char *lhs,const char *rhs)
	{
		return (std::strcmp(lhs,rhs) < 0);
	}
};

// (inner class) nucleus
// FIXME explain
struct atom::nucleus {
	// (var) m_refcount
	// FIXME explain
	int m_refcount;

	// (var) m_name
	// FIXME explain
	const char *m_name;

	// (var) m_parent
	// FIXME explain
	nucleus *m_parent;

	// (var) m_asset
	// FIXME explain
	asset *m_asset;

	// (var) m_children
	// FIXME explain
	std::map<const char *,nucleus *,nucleus_name_comparator> m_children;
};

// declared in res.hh
atom::atom(nucleus *nuc) noexcept :
	m_nuc(nuc)
{
	if (m_nuc) {
		m_nuc->m_refcount++;
	}
}

// declared in res.hh
asset *&atom::get_internal_asset_ptr() const
{
	if (!m_nuc) {
		throw 0;//FIXME
	}
	return m_nuc->m_asset;
}

// declared in res.hh
atom atom::make_root()
{
	auto nuc_space = std::malloc(sizeof(nucleus));
	if (!nuc_space) {
		throw 0;//FIXME
	}

	nucleus *nuc;
	try {
		nuc = new(nuc_space) nucleus;
	} catch (...) {
		std::free(nuc_space);
		throw;
	}
	nuc->m_refcount = 0;
	nuc->m_name = "_ROOT";
	nuc->m_parent = nullptr;
	nuc->m_asset = nullptr;
	return atom(nuc);
}

// declared in res.hh
atom::atom() noexcept :
	m_nuc(nullptr)
{
}

// declared in res.hh
atom::atom(std::nullptr_t) noexcept :
	m_nuc(nullptr)
{
}

// declared in res.hh
atom::atom(const atom &src) noexcept :
	m_nuc(src.m_nuc)
{
	if (m_nuc) {
		m_nuc->m_refcount++;
	}
}

// declared in res.hh
atom::atom(atom &&src) noexcept :
	m_nuc(src.m_nuc)
{
	src.m_nuc = nullptr;
}

// declared in res.hh
atom::~atom() noexcept
{
	nucleus *nuc = m_nuc;
	while (nuc && --nuc->m_refcount == 0) {
		nucleus *parent = nuc->m_parent;
		if (parent) {
			parent->m_children.erase(nuc->m_name);
		}
		nuc->~nucleus();
		std::free(nuc);
		nuc = parent;
	}
}

// declared in res.hh
atom &atom::operator =(atom rhs)
{
	std::swap(m_nuc,rhs.m_nuc);
	return *this;
}

// declared in res.hh
bool atom::operator ==(const atom &rhs) const noexcept
{
	return (m_nuc == rhs.m_nuc);
}

// declared in res.hh
bool atom::operator ==(std::nullptr_t) const noexcept
{
	return (m_nuc == nullptr);
}

// declared in res.hh
bool atom::operator !=(const atom &rhs) const noexcept
{
	return (m_nuc != rhs.m_nuc);
}

// declared in res.hh
bool atom::operator !=(std::nullptr_t) const noexcept
{
	return (m_nuc != nullptr);
}

// declared in res.hh
atom::operator bool() const noexcept
{
	return (m_nuc != nullptr);
}

// declared in res.hh
bool atom::operator !() const noexcept
{
	return !m_nuc;
}

// declared in res.hh
bool atom::operator <(const atom &rhs) const
{
	return (m_nuc < rhs.m_nuc);
}

// declared in res.hh
atom atom::operator /(const char *s) const
{
	if (!m_nuc) {
		throw 0;//FIXME
	}

	auto name_len = std::strlen(s);
	if (name_len == 0) {
		throw 0;//FIXME
	}

	auto iter = m_nuc->m_children.find(s);
	if (iter != m_nuc->m_children.end()) {
		return atom(iter->second);
	}

	auto newnuc_space = std::malloc(sizeof(nucleus) + name_len + 1);
	if (!newnuc_space) {
		throw 0;//FIXME
	}

	nucleus *newnuc;
	try {
		newnuc = new(newnuc_space) nucleus;
	} catch (...) {
		std::free(newnuc_space);
		throw;
	}

	auto name = static_cast<char *>(newnuc_space) + sizeof(nucleus);
	newnuc->m_refcount = 0;
	newnuc->m_name = name;
	newnuc->m_parent = m_nuc;
	newnuc->m_asset = nullptr;
	std::strcpy(name,s);

	try {
		m_nuc->m_children.insert({name,newnuc});
	} catch (...) {
		newnuc->~nucleus();
		std::free(newnuc_space);
		throw;
	}

	m_nuc->m_refcount++;

	return atom(newnuc);
}

// declared in res.hh
atom atom::operator /(const std::string &s) const
{
	return operator /(s.c_str());
}

// declared in res.hh
asset *atom::get() const
{
	if (!m_nuc) {
		throw 0;//FIXME
	}
	return m_nuc->m_asset;
}

// declared in res.hh
std::string atom::name() const
{
	if (!m_nuc) {
		return "[null]";
	} else {
		return m_nuc->m_name;
	}
}

// declared in res.hh
std::string atom::full_path() const
{
	if (!m_nuc) {
		return "[null]";
	} else if (!m_nuc->m_parent) {
		return "";
	} else {
		return atom(m_nuc->m_parent).full_path() + "/" + m_nuc->m_name;
	}
}

// declared in res.hh
atom atom::get_parent() const
{
	if (!m_nuc) {
		throw 0;//FIXME
	}

	if (!m_nuc->m_parent) {
		throw 0;//FIXME
	}

	return atom(m_nuc->m_parent);
}

// declared in res.hh
std::vector<atom> atom::get_children() const
{
	if (!m_nuc) {
		throw 0;//FIXME
	}

	std::vector<atom> result;
	for (auto &&kv : m_nuc->m_children) {
		auto &&child = kv.second;
		atom child_atom(child);
		result.push_back(child_atom);
	}
	return result;
}

// declared in res.hh
std::vector<atom> atom::get_children_recursive() const
{
	if (!m_nuc) {
		throw 0;//FIXME
	}

	std::vector<atom> result;
	for (auto &&kv : m_nuc->m_children) {
		auto &&child = kv.second;
		atom child_atom(child);
		result.push_back(child_atom);
		auto subchildren = child_atom.get_children_recursive();
		result.insert(
			result.end(),
			subchildren.begin(),
			subchildren.end()
		);
	}
	return result;
}

}
}
