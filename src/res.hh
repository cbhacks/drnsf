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

#pragma once

/*
 * res.hh
 *
 * FIXME explain
 */

#include <map>
#include <vector>
#include "transact.hh"

#if FEATURE_SCRIPTING
#include "scripting.hh"
#endif

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
class project;
template <typename F> class atom_iterable;

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
    // Returns a reference to the asset pointer held within the nucleus. Used
    // by `res::asset' to handle changing atom bindings when renaming assets
    // or creating/destroying them.
    asset *&get_internal_asset_ptr() const;

public:
    // (s-func) is_valid_char
    // Returns true if the given character is a valid character for an atom
    // name, or false otherwise.
    //
    // Valid characters are:
    //
    //   alphanumeric characters '0-9A-Za-z'
    //
    //   underscore '_'
    //
    //   exclamation mark '!'
    //
    //   equal sign '='
    //
    //   dash '-'
    //
    // The forward slash '/' is not valid in a name, but is used as a name
    // separator and at the start of a path string.
    static bool is_valid_char(int c) noexcept;

    // (s-func) make_root
    // Creates a root atom referencing the specified project. Should not be
    // called by any code except the project constructor.
    static atom make_root(project *proj);

    // (default ctor)
    // Creates a null atom.
    atom() noexcept;

    // (conversion ctor)
    // Creates a null atom.
    atom(nullptr_t) noexcept;

    // (copy ctor)
    // Creates an atom as a copy of the given atom.
    atom(const atom &src) noexcept;

    // (move ctor)
    // Creates an atom by moving from the given atom.
    atom(atom &&src) noexcept;

    // (dtor)
    // Destroys the atom. This may destroy the nucleus as well, which would
    // modify the parent atom's nucleus.
    ~atom() noexcept;

    // (assignment operator)
    // Assigns a new value to the atom.
    atom &operator =(atom rhs);

    // (equal operator)
    // Returns true if this atom is equal to the given atom (or is null for the
    // nullptr case).
    bool operator ==(const atom &rhs) const noexcept;
    bool operator ==(nullptr_t) const noexcept;

    // (not-equal operator)
    // Returns true if this atom is not equal to the given atom (or is not null
    // for the nullptr case).
    bool operator !=(const atom &rhs) const noexcept;
    bool operator !=(nullptr_t) const noexcept;

    // (explicit conversion operator)
    // Returns true if this atom is not null, or false otherwise. This allows
    // it to be used as a condition in if statements, while statements, etc.
    explicit operator bool() const noexcept;

    // (not operator)
    // Returns true if this atom is null.
    bool operator !() const noexcept;

    // (lesser operator)
    // FIXME explain
    bool operator <(const atom &rhs) const;

    // (path operator)
    // Returns the child atom with the given name. For example, if the current
    // atom is `/A/B' and the string given is "C", the result is atom `/A/B/C'.
    //
    // If no child atom with the given name exists, one is created.
    atom operator /(std::string_view s) const;

    // (path append operator)
    // This operates as though `a /= b' was `a = a / b'.
    atom &operator /=(std::string_view s);

    // (func) get
    // Returns the asset associated with this atom; that is, the asset whose
    // name is equal to this atom. If no such asset exists, returns null.
    asset *get() const;

    // (func) get_as<T>
    // Returns the asset associated with this atom if its type is derived from
    // `T'. Returns null if no asset is associated, or if the associated asset
    // is not compatible with `T'.
    template <typename T>
    T *get_as() const
    {
        static_assert(
            std::is_base_of_v<asset, T>,
            "Type T given for get_as<T> must derive from res::asset."
        );
        return dynamic_cast<T*>(get());
    }

    // (func) is_a<T>
    // Returns true if the asset associated with this atom is of a type derived
    // from `T'. Returns false if the asset associated is not, or if there is
    // no associated asset.
    template <typename T>
    bool is_a() const
    {
        static_assert(
            std::is_base_of_v<asset, T>,
            "Type T given for is_a<T> must derive from res::asset."
        );
        return (get_as<T>() != nullptr);
    }

    // (func) basename
    // Returns the name of this atom excluding the ancestral path leading to it
    // from the root atom. For example, given the atom `/A/B/C', returns "C".
    //
    // If null, returns "[null]". If this is a root atom, an empty string is
    // returned.
    std::string basename() const;

    // (func) dirname
    // Returns the path leading up to this atom but excluding the name of the
    // atom itself. For example, given the atom `/A/B/C', returns "/A/B".
    //
    // If null, returns "[null]". If this is a root atom, an empty string is
    // returned.
    std::string dirname() const;

    // (func) path
    // Returns the full path of the atom, including its name. For example,
    // given the atom `/A/B/C', returns "/A/B/C".
    //
    // If null, returns "[null]". If this is a root atom, an empty string is
    // returned.
    std::string path() const;

    // (func) get_parent
    // Returns this atom's parent atom, or null if this is a root atom.
    atom get_parent() const;

    // (func) get_depth
    // Returns this atom's distance from the root atom. For example:
    //
    //   Asset name `/foo' has depth 1.
    //
    //   Asset name `/foo/bar' has depth 2.
    //
    //   Asset name `/foo/bar/baz' has depth 3
    //
    // A root atom has a depth of zero.
    //
    // It is an error to call this method on a null atom.
    int get_depth() const;

    // (func) first_child
    // Returns the first child of this atom, or null if this atom has no
    // children.
    atom first_child() const;

    // (func) next_sibling
    // Returns this atom's next sibling, or null if this is the last sibling
    // or it is an only child. For a root node, this always returns null.
    atom next_sibling() const;

    // (func) each_child
    // Returns an iterable object which, when iterated, yields each of the
    // child atoms of this atom.
    auto each_child() const;

    // (func) each_descendant
    // Returns an iterable object which, when iterated, yields each descendant
    // atom of this atom.
    auto each_descendant() const;

    // (func) get_proj
    // Gets the project pointer the root node was created with (see make_root).
    project *get_proj() const;

    // (func) is_descendant_of
    // Returns true if the atom is a descendant of the specified atom. For
    // example, `/foo/bar' is a descendant of `/foo', but not of `/hi'. `/a/b/c'
    // and `/a/b/hello' are both descendants of `/a/b' but not of eachother.
    //
    // An atom is not a descendant of itself. All non-root atoms are descendants
    // of their tree's root atom, but not of the root atom of any other tree.
    //
    // It is an error to call this method on a null atom. The parameter atom may
    // be null, however, in which case the return value will be false.
    bool is_descendant_of(const atom &potential_ancestor) const;

    // (func) to_string
    // Returns a textual representation of this atom for string formatting.
    friend std::string to_string(const atom &atom)
    {
        return atom.path();
    }
};

/*
 * res::atom_iterator
 *
 * FIXME explain
 */
template <typename F>
class atom_iterator {
private:
    atom m_current;
    F m_f;

public:
    atom_iterator(atom start, F f) :
        m_current(start),
        m_f(f) {}

    atom operator *() const
    {
        return m_current;
    }

    void operator ++()
    {
        if (!m_current)
            throw std::runtime_error("res::atom_iterator: concluded");
        m_current = m_f(m_current);
    }

    bool operator !=(util::end_t) const
    {
        return m_current != nullptr;
    }
};

/*
 * res::atom_iterable
 *
 * FIXME explain
 */
template <typename F>
class atom_iterable {
private:
    atom m_start;
    F m_f;

public:
    atom_iterable(atom start, F f) :
        m_start(start),
        m_f(f) {}

    auto begin()
    {
        return atom_iterator<F>(m_start, m_f);
    }

    auto end()
    {
        return util::end;
    }
};

/*
 * res::make_iterable
 *
 * Constructs an atom_iterable from the given start and functor. Workaround for
 * MSVC which does not support inferring F when constructing atom_iterable<F>.
 */
template <typename F>
inline auto make_iterable(atom start, F f)
{
    return atom_iterable<F>(start, f);
}

// declared previously in this file
inline auto atom::each_child() const
{
    if (!m_nuc) {
        throw std::logic_error("res::atom::each_child: atom is null");
    }

    return make_iterable(first_child(), [](atom it) -> atom {
        return it.next_sibling();
    });
}

// declared previously in this file
inline auto atom::each_descendant() const
{
    if (!m_nuc) {
        throw std::logic_error("res::atom::each_descendant: atom is null");
    }

    return make_iterable(first_child(), [base = *this](atom it) -> atom {
        auto next = it.first_child();
        if (next)
            return next;

        while (true) {
            next = it.next_sibling();
            if (next)
                return next;

            it = it.get_parent();
            if (it == base)
                return nullptr;
        }
    });
}

/*
 * res::project
 *
 * FIXME explain
 */
class project :
    private util::nocopy,
    public std::enable_shared_from_this<project> {

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

    // (default ctor)
    // Constructs an empty project. This is private so that projects can only
    // be created with an std::shared_ptr via the `make' method defined later
    // in this class.
    project() :
        m_root(atom::make_root(this)) {}

public:
    // (func) make
    // Creates a new empty project and returns a shared pointer to the project.
    static std::shared_ptr<project> make()
    {
        auto self = new project();
        try {
            return std::shared_ptr<project>(self);
        } catch (...) {
            delete self;
            throw;
        }
    }

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

#if FEATURE_SCRIPTING
    // (var) m_scripthandle
    // An internal handle used by the scripting engine to maintain a two-way
    // association between native objects (this) and scripting objects.
    scripting::handle m_scripthandle;
#endif
};

/*
 * res::asset
 *
 * FIXME explain
 */
class asset : private util::nocopy {
    template <typename T>
    friend class prop;

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

    // (func) on_prop_change
    // This function is called after the value of a property on the asset is
    // changed. This includes if the change was due to a transaction undo, redo,
    // or rollback.
    //
    // The default implementation performs no action, but it can be overridden
    // by other asset types.
    virtual void on_prop_change(void *prop) noexcept {}

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
            throw std::logic_error("res::asset::create: name is null");

        if (name.get())
            throw std::logic_error("res::asset::create: name in use");

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

#if FEATURE_SCRIPTING
    // (var) m_scripthandle
    // An internal handle used by the scripting engine to maintain a two-way
    // association between native objects (this) and scripting objects.
    scripting::handle m_scripthandle;
#endif
};

/*
 * res::prop<T>
 *
 * FIXME explain
 */
template <typename T>
class prop : private util::nocopy {
private:
    // (internal class) change_op
    // FIXME explain
    class change_op : public transact::operation {
    private:
        // (var) m_prop
        // The property this change operation is running on.
        prop &m_prop;

        // (var) m_after
        // FIXME explain
        bool m_after;

    public:
        // (explicit ctor)
        // Constructs the change operation against the given property with the
        // specified `after' state.
        explicit change_op(prop &prop, bool after) :
            m_prop(prop),
            m_after(after) {}

        // (func) execute
        // FIXME explain
        void execute() noexcept override
        {
            if (m_after) {
                m_prop.m_owner.on_prop_change(&m_prop);
                m_prop.on_change();
            }
            m_after = !m_after;
        }
    };

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
        TS.push_op(std::make_unique<change_op>(*this, false));
        TS.set(m_value, std::move(value));
        TS.push_op(std::make_unique<change_op>(*this, true));
    }

    // (event) on_change
    // This event is raised after the value of the property is changed. This
    // includes changes which occur due to undo or redo.
    util::event<> on_change;
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

    // (conversion ctor)
    // Constructs a null asset ref.
    ref(nullptr_t) {}

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
            throw std::logic_error("res::ref::(deref op): bad ref");
        }
        return *result;
    }
};

// FIXME obsolete
using anyref = ref<asset>;

/*
 * res::tracker
 *
 * FIXME explain
 */
template <typename T>
class tracker : private util::nocopy {
private:
    // (var) m_name
    // The asset name this object is tracking.
    ref<T> m_name;

    // (handler) h_asset_appear, h_asset_disappear
    // Hooks the on_asset_appear and on_asset_disappear events of m_name's
    // associated project to track whenever an asset appears or disappears on
    // that name.
    decltype(project::on_asset_appear)::watch h_asset_appear;
    decltype(project::on_asset_disappear)::watch h_asset_disappear;

public:
    // (ctor)
    // Constructs a tracker with no name set on it.
    tracker()
    {
        h_asset_appear <<= [this](asset &asset) {
            if (asset.get_name() != m_name)
                return;
            if (!m_name.ok())
                return;
            on_acquire(&static_cast<T &>(asset));
        };
        h_asset_disappear <<= [this](asset &asset) {
            if (asset.get_name() != m_name)
                return;
            if (!m_name.ok())
                return;
            on_lose();
        };
    }

    // (func) get_name, set_name
    // Gets or sets the name this object is tracking.
    const ref<T> &get_name() const
    {
        return m_name;
    }
    void set_name(ref<T> name)
    {
        if (m_name == name)
            return;
        if (m_name) {
            h_asset_appear.unbind();
            h_asset_disappear.unbind();
            if (m_name.ok()) {
                on_lose();
            }
        }
        m_name = std::move(name);
        if (m_name) {
            h_asset_appear.bind(m_name.get_proj()->on_asset_appear);
            h_asset_disappear.bind(m_name.get_proj()->on_asset_disappear);
            auto asset = m_name.get();
            if (asset) {
                on_acquire(asset);
            }
        }
    }

    // (event) on_acquire, on_lose
    // The acquire event is raised in two scenarios:
    //
    //  - The name for the tracker is set using `set_name' and the new name is
    //  currently associated with an asset with an appropriate type (`T' or a
    //  type derived from `T').
    //
    //  - An asset of an appropriate type appears under the current name. This
    //  could happen because the asset was created on that name, or because it
    //  was renamed from another name.
    //
    // In either case, when that asset disappears from the name, or when the
    // name is changed away before that, the lose event will be raised. In the
    // case of using `set_name' to change from one appropriately bound asset
    // name to another, the lose event will be raised before the acquire event.
    //
    // The acquire event is raised with a pointer to the asset. The lose event
    // is not raised with any parameters.
    //
    // (*) An asset of the appropriate type has a type of `T' or is derived from
    // `T', directly or indirectly.
    util::event<T *> on_acquire;
    util::event<> on_lose;
};

/*
 * res::tree_tracker
 *
 * FIXME explain
 */
template <typename T>
class tree_tracker : private util::nocopy {
private:
    // (var) m_base
    // The base of the asset tree this object is tracking.
    atom m_base;

    // (handler) h_asset_appear, h_asset_disappear
    // Hooks the on_asset_appear and on_asset_disappear event of m_base's
    // associated project to track whenever assets appear or disappear on the
    // base name or a child of that name.
    decltype(project::on_asset_appear)::watch h_asset_appear;
    decltype(project::on_asset_disappear)::watch h_asset_disappear;

    // (func) report
    // Raises an on_acquire or on_lose event with the given asset pointer if it
    // is non-null and refers to an asset whose name matches or is a descendant
    // of the current base name.
    void report(T *asset, bool is_acquire)
    {
        if (!asset)
            return;

        const auto &name = asset->get_name();
        if (!(name == m_base || name.is_descendant_of(m_base)))
            return;

        if (is_acquire) {
            on_acquire(asset);
        } else {
            on_lose(asset);
        }
    }

public:
    // (default ctor)
    // Constructs a tracker with no base name set on it.
    tree_tracker()
    {
        h_asset_appear <<= [this](asset &asset) {
            report(dynamic_cast<T *>(&asset), true);
        };
        h_asset_disappear <<= [this](asset &asset) {
            report(dynamic_cast<T *>(&asset), false);
        };
    }

    // (func) get_base, set_base
    // Gets or sets the base asset name this object is tracking.
    const atom &get_base() const
    {
        return m_base;
    }
    void set_base(atom base)
    {
        // Exit early if the new name matches the existing one.
        if (m_base == base)
            return;

        // If there is already a base name set, raise an on_lose event for
        // each of the applicable assets (`T' or derived) bound to that name
        // and its descendants.
        if (m_base) {
            h_asset_appear.unbind();
            h_asset_disappear.unbind();
            report(m_base.get_as<T>(), false);
            for (auto &&descendant : m_base.each_descendant()) {
                report(descendant.get_as<T>(), false);
            }
        }

        m_base = base;

        // If the new name is non-null, raise on_acquire events in a similar
        // manner.
        if (m_base) {
            h_asset_appear.bind(m_base.get_proj()->on_asset_appear);
            h_asset_disappear.bind(m_base.get_proj()->on_asset_disappear);
            report(m_base.get_as<T>(), true);
            for (auto &&descendant : m_base.each_descendant()) {
                report(descendant.get_as<T>(), true);
            }
        }
    }

    // (event) on_acquire, on_lose
    // These events operate in the same manner as the standard non-tree tracker
    // object events. Unlike those events, however, the on_lose event here
    // specifies which asset has been lost.
    //
    // There is no guarantee that on_lose events for various assets will be
    // raised in the same order as the matching on_acquire events were.
    util::event<T *> on_acquire;
    util::event<T *> on_lose;
};

/*
 * res::import_error
 *
 * FIXME explain
 */
class import_error : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

/*
 * res::export_error
 *
 * FIXME explain
 */
class export_error : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

}

namespace reflect {

/*
 * reflect::asset_type_info
 *
 * FIXME explain
 */
template <typename AssetType>
struct asset_type_info {
    // (typedef) base_type
    // FIXME explain
    using base_type = res::asset;

    // (const) name
    // FIXME explain
    static constexpr const char *name = "(unknown type)";

    // (const) prop_count
    // The number of properties specific to this asset type. This count does
    // not include properties from parent types.
    static constexpr int prop_count = 0;

    static_assert (
        std::is_base_of<res::asset, AssetType>::value,
        "reflect::asset_type_info may only be used with types derived from "
        "res::asset"
    );
};

/*
 * reflect::asset_prop_info
 *
 * FIXME explain
 */
template <typename AssetType, int PropNum>
struct asset_prop_info;

// reflection info for res::asset
template <>
struct asset_type_info<res::asset> {
    using base_type = void;

    static constexpr const char *name = "Asset";
    static constexpr int prop_count = 0;
};

}
}
