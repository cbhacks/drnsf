//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2019  DRNSF contributors
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

// (internal type) root_info
// Extra information block attached to each root.
struct root_info {
    // (var) m_proj
    // The project which owns this root atom.
    project *m_proj;
};

// (inner class) nucleus
// FIXME explain
struct alignas(root_info) atom::nucleus {
    // (var) m_refcount
    // FIXME explain
    int m_refcount;

    // (var) m_name
    // FIXME explain
    std::string_view m_name;

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
    std::map<std::string_view, nucleus *> m_children;

    // (var) m_children_iter
    // For non-root nodes, the iterator in the parent's child map referring to
    // this node.
    decltype(m_children)::iterator m_children_iter;
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
    auto nuc_space = operator new(sizeof(nucleus) + sizeof(root_info));

    nucleus *nuc;
    root_info *info;
    try {
        nuc = new(nuc_space) nucleus;
    } catch (...) {
        operator delete(nuc_space);
        throw;
    }
    try {
        info = new(static_cast<char *>(nuc_space) + sizeof(nucleus)) root_info;
    } catch (...) {
        nuc->~nucleus();
        operator delete(nuc_space);
        throw;
    }
    nuc->m_refcount = 0;
    nuc->m_name = "_ROOT";
    nuc->m_depth = 0;
    nuc->m_parent = nullptr;
    nuc->m_asset = nullptr;
    info->m_proj = proj;
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
            parent->m_children.erase(nuc->m_children_iter);
        } else {
            reinterpret_cast<root_info *>(nuc + 1)->~root_info();
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
atom atom::operator /(std::string_view s) const
{
    if (!m_nuc) {
        throw std::logic_error("res::atom::(slash op): atom is null");
    }

    if (s.size() == 0) {
        throw std::logic_error("res::atom::(slash op): string is empty");
    }

    // TODO - check for invalid characters

    auto iter = m_nuc->m_children.find(s);
    if (iter != m_nuc->m_children.end()) {
        return atom(iter->second);
    }

    auto newnuc_space = operator new(sizeof(nucleus) + s.size());

    nucleus *newnuc;
    try {
        newnuc = new(newnuc_space) nucleus;
    } catch (...) {
        operator delete(newnuc_space);
        throw;
    }

    auto name = static_cast<char *>(newnuc_space) + sizeof(nucleus);
    newnuc->m_refcount = 0;
    newnuc->m_name = { name, s.size() };
    newnuc->m_depth = m_nuc->m_depth + 1;
    newnuc->m_parent = m_nuc;
    newnuc->m_asset = nullptr;
    std::memcpy(name, s.data(), s.size());

    try {
        auto insert_result = m_nuc->m_children.insert({newnuc->m_name, newnuc});
        newnuc->m_children_iter = insert_result.first;
    } catch (...) {
        newnuc->~nucleus();
        operator delete(newnuc_space);
        throw;
    }

    m_nuc->m_refcount++;

    return atom(newnuc);
}

// declared in res.hh
atom &atom::operator /=(std::string_view s)
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
std::string atom::basename() const
{
    if (!m_nuc) {
        return "[null]";
    } else if (!m_nuc->m_parent) {
        return "";
    } else {
        return std::string(m_nuc->m_name);
    }
}

// declared in res.hh
std::string atom::dirname() const
{
    if (!m_nuc) {
        throw std::logic_error("[null]");
    } else if (!m_nuc->m_parent) {
        return "";
    } else {
        return get_parent().path();
    }
}

// declared in res.hh
std::string atom::path() const
{
    if (!m_nuc) {
        return "[null]";
    } else if (!m_nuc->m_parent) {
        return "";
    } else {
        return get_parent().path() + "/" + std::string(m_nuc->m_name);
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
atom atom::first_child() const
{
    if (!m_nuc) {
        throw std::logic_error("res::atom::first_child: atom is null");
    }

    auto it = m_nuc->m_children.begin();
    if (it == m_nuc->m_children.end()) {
        return nullptr;
    } else {
        return atom(it->second);
    }
}

// declared in res.hh
atom atom::next_sibling() const
{
    if (!m_nuc) {
        throw std::logic_error("res::atom::next_sibling: atom is null");
    }

    // Consider orphans (root atoms) as only children.
    if (!m_nuc->m_parent) {
        return nullptr;
    }

    auto it = m_nuc->m_children_iter;
    ++it;
    if (it == m_nuc->m_parent->m_children.end()) {
        return nullptr;
    } else {
        return atom(it->second);
    }
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

    // Retrieve the root info block stashed after the nucleus structure in
    // memory.
    auto info = reinterpret_cast<root_info *>(top + 1);

    return info->m_proj;
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
