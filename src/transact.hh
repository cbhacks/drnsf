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
 * transact.hh
 *
 * FIXME explain
 */

#include <list>

/*
 * TRANSACT
 *
 * Simple alias for `teller &TS'. This is intended to be used in function
 * prototypes, such as `void set_foo(TRANSACT, int new_value)' to nicely
 * demonstrate that it is transactional and must be run with a teller.
 */
#define TRANSACT ::drnsf::transact::teller &TS

namespace drnsf {
namespace transact {

/*
 * transact::operation
 *
 * FIXME explain
 */
class operation : private util::nocopy {
public:
    // (pure func) execute
    // FIXME explain
    virtual void execute() noexcept = 0;

    // (dtor)
    // FIXME EXPLAIN
    virtual ~operation() = default;
};

/*
 * transact::assign_op<T>
 *
 * FIXME explain
 */
template <typename T>
class assign_op : public operation {
private:
    // (var) m_dest
    // FIXME explain
    T &m_dest;

    // (var) m_src
    // FIXME explain
    T m_src;

public:
    // (explicit ctor)
    // FIXME explain
    template <typename T2>
    explicit assign_op(T &dest, T2 src) :
        m_dest(dest),
        m_src(std::move(src)) {}

    // (func) execute
    // FIXME explain
    void execute() noexcept override
    {
        using std::swap;
        swap(m_dest, m_src);
    }
};

/*
 * transact::insert_op<T>
 *
 * FIXME explain
 */
template <typename T>
class insert_op : public operation {
private:
    // (var) m_temp
    // FIXME explain
    std::list<T> m_temp;

    // (var) m_value_iter
    // FIXME explain
    typename std::list<T>::iterator m_value_iter;

    // (var) m_dest_list
    // FIXME explain
    std::list<T> *m_dest_list;

    // (var) m_dest_iter
    // FIXME explain
    typename std::list<T>::iterator m_dest_iter;

    // (var) m_src_list
    // FIXME explain
    std::list<T> *m_src_list;

    // (var) m_src_iter
    // FIXME explain
    typename std::list<T>::iterator m_src_iter;

public:
    // (explicit ctor)
    // FIXME explain
    explicit insert_op(
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

    // (func) execute
    // FIXME explain
    void execute() noexcept override
    {
        m_dest_list->splice(m_dest_iter, *m_src_list, m_value_iter);
        std::swap(m_dest_list, m_src_list);
        std::swap(m_dest_iter, m_src_iter);
    }

    // (func) get_value_iterator
    // FIXME explain
    typename std::list<T>::iterator get_value_iterator() const
    {
        return m_value_iter;
    }
};

/*
 * transact::erase_op<T>
 *
 * FIXME explain
 */
template <typename T>
class erase_op : public operation {
private:
    // (var) m_temp
    // FIXME explain
    std::list<T> m_temp;

    // (var) m_value_iter
    // FIXME explain
    typename std::list<T>::iterator m_value_iter;

    // (var) m_dest_list
    // FIXME explain
    std::list<T> *m_dest_list;

    // (var) m_dest_iter
    // FIXME explain
    typename std::list<T>::iterator m_dest_iter;

    // (var) m_src_list
    // FIXME explain
    std::list<T> *m_src_list;

    // (var) m_src_iter
    // FIXME explain
    typename std::list<T>::iterator m_src_iter;

public:
    // (explicit ctor)
    // FIXME explain
    explicit erase_op(
        std::list<T> &list,
        typename std::list<T>::iterator pos) :
        m_value_iter(pos),
        m_dest_list(&m_temp),
        m_dest_iter(m_temp.end()),
        m_src_list(&list),
        m_src_iter(std::next(pos)) {}

    // (func) execute
    // FIXME explain
    void execute() noexcept override
    {
        m_dest_list->splice(m_dest_iter, *m_src_list, m_value_iter);
        std::swap(m_dest_list, m_src_list);
        std::swap(m_dest_iter, m_src_iter);
    }
};

/*
 * transact::transaction
 *
 * FIXME explain
 */
class transaction : private util::nocopy {
    friend class nexus;
    friend class teller;

private:
    // (var) m_ops
    // FIXME explain
    std::list<std::unique_ptr<operation>> m_ops;

    // (var) m_desc
    // FIXME explain
    std::string m_desc;

    // (var) m_next
    // FIXME explain
    std::unique_ptr<transaction> m_next;

    // (explicit ctor)
    // FIXME explain
    explicit transaction(
        std::list<std::unique_ptr<operation>> ops,
        std::string desc);

public:
    // (func) describe
    // FIXME explain
    const char *describe() const;
};

/*
 * transact::teller
 *
 * FIXME explain
 */
class teller : private util::nocopy {
    friend class nexus;

private:
    // (var) m_done
    // FIXME explain
    bool m_done;

    // (var) m_ops
    // FIXME explain
    std::list<std::unique_ptr<operation>> m_ops;

    // (var) m_desc
    // FIXME explain
    std::string m_desc;

    // (default ctor)
    // FIXME explain
    teller();

    // (dtor)
    // FIXME explain
    ~teller();

    // (func) commit
    // FIXME explain
    std::unique_ptr<transaction> commit();

public:
    // (func) describe
    // FIXME explain
    void describe(std::string desc);

    // (func) push_op
    // FIXME explain
    void push_op(std::unique_ptr<operation> op);

    // (func) set
    // FIXME explain
    template <typename T, typename T2>
    void set(T &dest, T2 src)
    {
        push_op(std::make_unique<assign_op<T>>(dest, std::move(src)));
    }

    // (func) insert
    // FIXME explain
    template <typename T, typename T2>
    typename std::list<T>::iterator insert(
        std::list<T> &list,
        typename std::list<T>::iterator pos,
        T2 value)
    {
        auto op = std::make_unique<insert_op<T>>(
            list,
            pos,
            std::move(value)
        );
        auto result = op->get_value_iterator();
        push_op(std::move(op));
        return result;
    }

    // (func) erase
    // FIXME explain
    template <typename T>
    void erase(std::list<T> &list, typename std::list<T>::iterator pos)
    {
        push_op(std::make_unique<erase_op<T>>(list, pos));
    }
};

/*
 * transact::status
 *
 * FIXME explain
 */
enum class status {
    ready,
    busy,
    failed
};

/*
 * transact::nexus
 *
 * FIXME explain
 */
class nexus : private util::nocopy {
private:
    // (var) m_status
    // FIXME explain
    status m_status;

    // (var) m_undo
    // FIXME explain
    std::unique_ptr<transaction> m_undo;

    // (var) m_redo
    // FIXME explain
    std::unique_ptr<transaction> m_redo;

public:
    // (default ctor)
    // FIXME explain
    nexus();

    // (dtor)
    // FIXME explain
    ~nexus();

    // (func) get_status
    // FIXME explain
    status get_status() const;

    // (func) has_undo
    // FIXME explain
    bool has_undo() const;

    // (func) has_redo
    // FIXME explain
    bool has_redo() const;

    // (func) get_undo
    // FIXME explain
    const transaction &get_undo() const;

    // (func) get_redo
    // FIXME explain
    const transaction &get_redo() const;

    // (func) undo
    // FIXME explain
    void undo();

    // (func) redo
    // FIXME explain
    void redo();

    // (func) run
    // FIXME explain
    void run(std::function<void(TRANSACT)> fn);

    // (event) on_status_change
    // FIXME explain
    util::event<> on_status_change;
};

}
}
