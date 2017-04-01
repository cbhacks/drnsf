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
#include "res.hh"

#include <iostream>//FIXME

namespace res {

class atom::nucleus : private util::nocopy {
	friend class atom;

private:
	nucleus *m_parent;
	int m_refcount;
	std::string m_name;
	std::unique_ptr<asset> m_asset;
	std::map<std::string,nucleus *> m_children;
	decltype(m_children)::iterator m_iter;

public:
	explicit nucleus() :
		m_parent(nullptr),
		m_refcount(0),
		m_name("root")
	{
	}

	explicit nucleus(nucleus *parent,std::string name) :
		m_parent(parent),
		m_refcount(0),
		m_name(name)
	{
		auto result = parent->m_children.insert({m_name,this});
		m_iter = result.first;
		parent->incref();
	}

	~nucleus()
	{
		if (m_parent) {
			m_parent->m_children.erase(m_iter);
			m_parent->decref();
		}
	}

	void incref()
	{
		m_refcount++;
		std::cout
			<< "INCREF("
			<< m_refcount
			<< ") "
			<< m_name
			<< std::endl;
	}

	void decref()
	{
		m_refcount--;
		std::cout
			<< "DECREF("
			<< m_refcount
			<< ") "
			<< m_name
			<< std::endl;
		if (m_refcount <= 0) {
			delete this;
		}
	}
};

atom::atom(nucleus *nuc) :
	m_nuc(nuc)
{
	if (nuc) {
		nuc->incref();
	}
}

std::unique_ptr<asset> &atom::get_internal_asset_ptr() const
{
	if (!m_nuc) {
		throw 0;//FIXME
	}
	return m_nuc->m_asset;
}

atom atom::make_root()
{
	return atom(new nucleus());
}

atom::atom() :
	m_nuc(nullptr)
{
}

atom::atom(std::nullptr_t) :
	m_nuc(nullptr)
{
}

atom::atom(const atom &other) :
	m_nuc(other.m_nuc)
{
	if (m_nuc) {
		m_nuc->incref();
	}
}

atom::atom(atom &&other) :
	m_nuc(other.m_nuc)
{
	other.m_nuc = nullptr;
}

atom::~atom()
{
	if (m_nuc) {
		m_nuc->decref();
	}
}

atom &atom::operator =(atom other)
{
	std::swap(m_nuc,other.m_nuc);
	return *this;
}

bool atom::operator ==(const atom &other) const
{
	return (m_nuc == other.m_nuc);
}

bool atom::operator ==(std::nullptr_t) const
{
	return (m_nuc == nullptr);
}

bool atom::operator !=(const atom &other) const
{
	return (m_nuc != other.m_nuc);
}

bool atom::operator !=(std::nullptr_t) const
{
	return (m_nuc != nullptr);
}

atom::operator bool() const
{
	return (m_nuc != nullptr);
}

bool atom::operator !() const
{
	return !m_nuc;
}

atom atom::operator /(const std::string &s) const
{
	if (!m_nuc) {
		throw 0;//FIXME
	}

	auto iter = m_nuc->m_children.find(s);
	if (iter == m_nuc->m_children.end()) {
		auto nuc = new nucleus(m_nuc,s);
		return atom(nuc);
	} else {
		auto nuc = iter->second;
		return atom(nuc);
	}
}

asset *atom::get() const
{
	if (!m_nuc) {
		throw 0;//FIXME
	}
	return m_nuc->m_asset.get();
}

std::string atom::name() const
{
	if (!m_nuc) {
		return "[null]";
	} else {
		return m_nuc->m_name;
	}
}

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
