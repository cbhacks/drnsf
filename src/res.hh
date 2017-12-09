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

/*
 * res.hh
 *
 * FIXME explain
 */

#include <map>
#include <vector>
#include "transact.hh"

/*
 * DEFINE_APROP
 *
 * FIXME explain
 */
#define DEFINE_APROP(name, type, ...) \
    ::drnsf::res::prop<type> p_##name = { *this, {__VA_ARGS__} }; \
    const type &get_##name() const \
    { \
        return p_##name.get(); \
    } \
    void set_##name(TRANSACT, type value) \
    { \
        p_##name.set(TS, std::move(value)); \
    }

namespace drnsf {
namespace res {

// Forward declaration for use in res::atom.
class asset;

/*
 * res::atom
 *
 * FIXME explain
 */
class atom {
    friend class asset;

    // (inner class) nucleus
    // FIXME explain
    struct nucleus;

private:
    // (var) m_nuc
    // FIXME explain
    nucleus *m_nuc;

    // (explicit ctor)
    // FIXME explain
    explicit atom(nucleus *nuc) noexcept;

    // (func) get_internal_asset_ptr
    // FIXME explain
    asset *&get_internal_asset_ptr() const;

public:
    // (s-func) make_root
    // FIXME explain
    static atom make_root();

    // (default ctor)
    // FIXME explain
    atom() noexcept;

    // (conversion ctor)
    // FIXME explain
    atom(std::nullptr_t) noexcept;

    // (copy ctor)
    // FIXME explain
    atom(const atom &src) noexcept;

    // (move ctor)
    // FIXME explain
    atom(atom &&src) noexcept;

    // (dtor)
    // FIXME explain
    ~atom() noexcept;

    // (assignment operator)
    // FIXME explain
    atom &operator =(atom rhs);

    // (equal operator)
    // FIXME explain
    bool operator ==(const atom &rhs) const noexcept;
    bool operator ==(std::nullptr_t) const noexcept;

    // (not-equal operator)
    // FIXME explain
    bool operator !=(const atom &rhs) const noexcept;
    bool operator !=(std::nullptr_t) const noexcept;

    // (explicit conversion operator)
    // FIXME explain
    explicit operator bool() const noexcept;

    // (not operator)
    // FIXME explain
    bool operator !() const noexcept;

    // (lesser operator)
    // FIXME explain
    bool operator <(const atom &rhs) const;

    // (path operator)
    // FIXME explain
    atom operator /(const char *s) const;
    atom operator /(const std::string &s) const;

    // (path append operator)
    // This operates as though `a /= b' was `a = a / b'.
    atom &operator /=(const char *s);
    atom &operator /=(const std::string &s);

    // (func) get
    // FIXME explain
    asset *get() const;

    // (func) get_as<T>
    // FIXME explain
    template <typename T>
    T *get_as() const
    {
        return dynamic_cast<T*>(get());
    }

    // (func) is_a<T>
    // FIXME explain
    template <typename T>
    bool is_a() const
    {
        return (get_as<T>() != nullptr);
    }

    // (func) name
    // FIXME explain
    std::string name() const;

    // (func) full_path
    // FIXME explain
    std::string full_path() const;

    // (func) get_parent
    // FIXME explain
    atom get_parent() const;

    // (func) get_children
    // FIXME explain
    std::vector<atom> get_children() const;

    // (func) get_children_recursive
    // FIXME explain
    std::vector<atom> get_children_recursive() const;

    // (func) get_asset_names
    // FIXME explain
    std::vector<atom> get_asset_names() const
    {
        return get_children_recursive();
    }

    // (func) to_string
    // FIXME explain
    friend std::string to_string(const atom &atom)
    {
        return atom.full_path();
    }
};

// FIXME this is a duplicate forward declaration
class asset;

/*
 * res::project
 *
 * FIXME explain
 */
class project : private util::nocopy {
    friend class asset;

private:
    // (var) m_root
    // FIXME explain
    atom m_root;

    // (var) m_assets
    // FIXME explain
    std::list<std::unique_ptr<asset>> m_assets;

    // (var) m_transact
    // FIXME explain
    transact::nexus m_transact;

public:
    // (default ctor)
    // FIXME explain
    project() :
        m_root(atom::make_root()) {}

    // (func) get_asset_root
    // FIXME explain
    const atom &get_asset_root()
    {
        return m_root;
    }

    // (func) get_asset_list
    // FIXME explain
    const decltype(m_assets) &get_asset_list()
    {
        return m_assets;
    }

    // (func) get_transact
    // FIXME explain
    transact::nexus &get_transact()
    {
        return m_transact;
    }

    // (event) on_asset_appear
    // FIXME explain
    util::event<asset &> on_asset_appear;

    // (event) on_asset_disappear
    // FIXME explain
    util::event<asset &> on_asset_disappear;
};

/*
 * res::asset
 *
 * FIXME explain
 */
class asset : private util::nocopy {
private:
    // (var) m_name
    // FIXME explain
    atom m_name;

    // (var) m_proj
    // FIXME explain
    project &m_proj;

    // (var) m_iter
    // FIXME explain
    std::list<std::unique_ptr<asset>>::iterator m_iter;

    // (func) create_imple
    // FIXME explain
    void create_impl(TRANSACT, atom name);

protected:
    // (explicit ctor)
    // FIXME explain
    explicit asset(project &proj) :
        m_proj(proj) {}

public:
    // (dtor)
    // FIXME explain
    virtual ~asset() = default;

    // (func) assert_alive
    // FIXME explain
    void assert_alive() const;

    // (s-func) create<T>
    // FIXME explain
    template <typename T>
    static void create(TRANSACT, atom name, project &proj)
    {
        if (!name)
            throw 0; // FIXME

        if (name.get())
            throw 0; // FIXME

        (new T(proj))->create_impl(TS, name);
    }

    // (func) rename
    // FIXME explain
    void rename(TRANSACT, atom name);

    // (func) destroy
    // FIXME explain
    void destroy(TRANSACT);

    // (func) get_name
    // FIXME explain
    const atom &get_name() const;

    // (func) get_proj
    // FIXME explain
    project &get_proj() const;

    // FIXME obsolete
    template <typename Reflector>
    void reflect(Reflector &rfl)
    {
    }
};

/*
 * res::prop<T>
 *
 * FIXME explain
 */
template <typename T>
class prop : private util::nocopy {
private:
    // (var) m_owner
    // FIXME explain
    asset &m_owner;

    // (var) m_value
    // FIXME explain
    T m_value;

public:
    // (ctor)
    // FIXME explain
    prop(asset &owner, T value) :
        m_owner(owner),
        m_value(value) {}

    // (func) get
    // FIXME explain
    const T &get() const
    {
        m_owner.assert_alive();
        return m_value;
    }

    // (func) set
    // FIXME explain
    void set(TRANSACT, T value)
    {
        m_owner.assert_alive();
        TS.set(m_value, std::move(value));
    }
};

/*
 * res::ref<T>
 *
 * FIXME explain
 */
template <typename T>
class ref : public atom {
public:
    // (default ctor)
    // FIXME explain
    ref() = default;

    // (copy ctor)
    // FIXME explain
    ref(const atom &src) :
        atom(src) {}

    // (move ctor)
    // FIXME explain
    ref(atom &&src) :
        atom(std::move(src)) {}

    // (func) create
    // FIXME explain
    void create(TRANSACT, project &proj) const
    {
        res::asset::create<T>(TS, *this, proj);
    }

    // (func) ok
    // FIXME explain
    bool ok() const
    {
        return (static_cast<bool>(*this) && is_a<T>());
    }

    // (func) get
    // FIXME explain
    T *get() const
    {
        return get_as<T>();
    }

    // (pointer member access operator)
    // FIXME explain
    T *operator ->() const
    {
        return &operator *();
    }

    // (dereference operator)
    // FIXME explain
    T &operator *() const
    {
        auto result = get_as<T>();
        if (!result) {
            throw 0;//FIXME
        }
        return *result;
    }
};

// FIXME obsolete
using anyref = ref<asset>;

}
}
