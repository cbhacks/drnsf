//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2020  DRNSF contributors
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
    bool operator()(const char *lhs, const char *rhs) const
    {
        return (std::strcmp(lhs, rhs) < 0);
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

    // (var) m_depth
    // The depth, as explained in the comment for `atom::get_depth'. For the
    // root this will be zero.
    int m_depth;

    // (var) m_asset
    // FIXME explain
    asset *m_asset;

    // (var) m_children
    // FIXME explain
    std::map<const char *, nucleus *, nucleus_name_comparator> m_children;
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
        throw std::logic_error("res::atom::<internal>: atom is null");
    }
    return m_nuc->m_asset;
}

// declared in res.hh
atom atom::make_root(project *proj)
{
    auto nuc_space = operator new(sizeof(nucleus) + sizeof(project *));

    nucleus *nuc;
    try {
        nuc = new(nuc_space) nucleus;
    } catch (...) {
        operator delete(nuc_space);
        throw;
    }
    nuc->m_refcount = 0;
    nuc->m_name = "_ROOT";
    nuc->m_depth = 0;
    nuc->m_parent = nullptr;
    nuc->m_asset = nullptr;
    std::memcpy(nuc + 1, &proj, sizeof(project *));
    return atom(nuc);
}

// declared in res.hh
atom::atom() noexcept :
    m_nuc(nullptr)
{
}

// declared in res.hh
atom::atom(nullptr_t) noexcept :
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
        operator delete(nuc);
        nuc = parent;
    }
}

// declared in res.hh
atom &atom::operator =(atom rhs)
{
    std::swap(m_nuc, rhs.m_nuc);
    return *this;
}

// declared in res.hh
bool atom::operator ==(const atom &rhs) const noexcept
{
    return (m_nuc == rhs.m_nuc);
}

// declared in res.hh
bool atom::operator ==(nullptr_t) const noexcept
{
    return (m_nuc == nullptr);
}

// declared in res.hh
bool atom::operator !=(const atom &rhs) const noexcept
{
    return (m_nuc != rhs.m_nuc);
}

// declared in res.hh
bool atom::operator !=(nullptr_t) const noexcept
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
        throw std::logic_error("res::atom::(slash op): atom is null");
    }

    auto name_len = std::strlen(s);
    if (name_len == 0) {
        throw std::logic_error("res::atom::(slash op): string is empty");
    }

    auto iter = m_nuc->m_children.find(s);
    if (iter != m_nuc->m_children.end()) {
        return atom(iter->second);
    }

    auto newnuc_space = operator new(sizeof(nucleus) + name_len + 1);

    nucleus *newnuc;
    try {
        newnuc = new(newnuc_space) nucleus;
    } catch (...) {
        operator delete(newnuc_space);
        throw;
    }

    auto name = static_cast<char *>(newnuc_space) + sizeof(nucleus);
    newnuc->m_refcount = 0;
    newnuc->m_name = name;
    newnuc->m_depth = m_nuc->m_depth + 1;
    newnuc->m_parent = m_nuc;
    newnuc->m_asset = nullptr;
    std::strcpy(name, s);

    try {
        m_nuc->m_children.insert({name, newnuc});
    } catch (...) {
        newnuc->~nucleus();
        operator delete(newnuc_space);
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
atom &atom::operator /=(const char *s)
{
    *this = *this / s;
    return *this;
}

// declared in res.hh
atom &atom::operator /=(const std::string &s)
{
    *this = *this / s;
    return *this;
}

// declared in res.hh
asset *atom::get() const
{
    if (!m_nuc) {
        throw std::logic_error("res::atom::get: atom is null");
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
        throw std::logic_error("res::atom::get_parent: atom is null");
    }

    if (!m_nuc->m_parent) {
        throw std::logic_error("res::atom::get_parent: atom is root");
    }

    return atom(m_nuc->m_parent);
}

// declared in res.hh
int atom::get_depth() const
{
    if (!m_nuc) {
        throw std::logic_error("res::atom::get_depth: atom is null");
    }

    return m_nuc->m_depth;
}

// declared in res.hh
std::vector<atom> atom::get_children() const
{
    if (!m_nuc) {
        throw std::logic_error("res::atom::get_children: atom is null");
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
        throw std::logic_error(
            "res::atom::get_children_recursive: atom is null"
        );
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

// declared in res.hh
project *atom::get_proj() const
{
    if (!m_nuc) {
        throw std::logic_error("res::atom::get_proj: atom is null");
    }

    // Find the root atom.
    auto top = m_nuc;
    while (top->m_parent) {
        top = top->m_parent;
    }

    // Retrieve the project pointer stashed after the nucleus structure in
    // memory.
    project *proj;
    std::memcpy(&proj, top + 1, sizeof(project *));

    return proj;
}

// declared in res.hh
bool atom::is_descendant_of(const atom &potential_ancestor) const
{
    if (!m_nuc) {
        throw std::logic_error("res::atom::is_descendant_of: atom is null");
    }

    if (!potential_ancestor) {
        // Null atoms are never ancestors.
        return false;
    }

    // Compute the difference of the depths of the two atoms within the tree.
    // The potential_ancestor must be less-deep than this atom to possible be
    // an ancestor.
    int depth_difference = m_nuc->m_depth - potential_ancestor.m_nuc->m_depth;
    if (depth_difference <= 0) {
        return false;
    }

    // Walk up the ancestral path to the same depth as the given potential
    // ancestor.
    auto it = m_nuc;
    for (int i = 0; i < depth_difference; i++) {
        it = it->m_parent;
    }

    return it == potential_ancestor.m_nuc;
}

}
}
