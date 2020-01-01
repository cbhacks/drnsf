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
#include "transact.hh"

namespace drnsf {
namespace transact {

// declared in transact.hh
nexus::nexus() :
    m_status(status::ready)
{
}

// declared in transact.hh
nexus::~nexus()
{
    // FIXME - handle non-ready status
}

// declared in transact.hh
status nexus::get_status() const
{
    return m_status;
}

// declared in transact.hh
bool nexus::has_undo() const
{
    if (m_status != status::ready) {
        throw std::logic_error("transact::nexus::has_undo: busy");
    }
    return m_undo != nullptr;
}

// declared in transact.hh
bool nexus::has_redo() const
{
    if (m_status != status::ready) {
        throw std::logic_error("transact::nexus::has_redo: busy");
    }
    return m_redo != nullptr;
}

// declared in transact.hh
const transaction &nexus::get_undo() const
{
    if (m_status != status::ready) {
        throw std::logic_error("transact::nexus::get_undo: busy");
    }
    return *m_undo;
}

// declared in transact.hh
const transaction &nexus::get_redo() const
{
    if (m_status != status::ready) {
        throw std::logic_error("transact::nexus::get_redo: busy");
    }
    return *m_redo;
}

// declared in transact.hh
void nexus::undo()
{
    if (m_status != status::ready) {
        throw std::logic_error("transact::nexus::undo: busy");
    }

    // Ensure that there is actually a transaction to undo.
    // TODO

    m_status = status::busy;
    on_status_change();

    // Run each of the operations in the transaction, causing them to be
    // undone.
    for (auto &&op : m_undo->m_ops) {
        op->execute();
    }

    // Reverse the transaction's operation list. This ensures the list
    // is executed in the inverse order if it is redone in the future.
    m_undo->m_ops.reverse();

    // Pop the transaction from the undo stack and push it onto the redo
    // stack. Essentially, given transactions {B,A} which can be undone,
    // transactions {J,K} which can be redone, and transaction X which is
    // in the process of being undone:
    //
    // A <- B <- X <- (current state) -> J -> K
    // becomes
    // A <- B <- (current state) -> X -> J -> K
    std::unique_ptr<transaction> t;
    m_undo.swap(t);
    m_undo.swap(t->m_next);
    m_redo.swap(t->m_next);
    m_redo.swap(t);

    m_status = status::ready;
    on_status_change();
}

// declared in transact.hh
void nexus::redo()
{
    if (m_status != status::ready) {
        throw std::logic_error("transact::nexus::redo: busy");
    }

    // Ensure that there is actually a transaction to redo.
    // TODO

    m_status = status::busy;
    on_status_change();

    // Run each of the operations in the transaction, causing them to be
    // redone.
    for (auto &&op : m_redo->m_ops) {
        op->execute();
    }

    // Reverse the transaction's operation list. This ensures the list
    // is executed in the inverse order if it is undone in the future.
    m_redo->m_ops.reverse();

    // Pop the transaction from the redo stack and push it onto the undo
    // stack. Essentially, given transactions {B,A} which can be undone,
    // transactions {J,K} which can be redone, and transaction X which is
    // in the process of being redone:
    //
    // A <- B <- (current state) -> X -> J -> K
    // becomes
    // A <- B <- X <- (current state) -> J -> K
    std::unique_ptr<transaction> t;
    m_redo.swap(t);
    m_redo.swap(t->m_next);
    m_undo.swap(t->m_next);
    m_undo.swap(t);

    m_status = status::ready;
    on_status_change();
}

// declared in transact.hh
void nexus::run(std::function<void(TRANSACT)> fn)
{
    if (m_status != status::ready) {
        throw std::logic_error("transact::nexus::run: busy");
    }

    m_status = status::busy;
    on_status_change();

    // Create the teller used to build this transaction. If this function
    // exits without committing the transaction, the teller will rollback
    // all of the changes automatically.
    transact::teller ts;

    // Run the functor given to the nexus.
    fn(ts);

    // Commit the transaction.
    auto t = ts.commit();

    // Set this new transaction's next pointer to the current next-to-undo
    // transaction, and set the next-to-undo to the new transaction.
    //
    // Given current transaction history (latest-first) {C, B, A} and a new
    // transaction X being introduced:
    //
    // A <- B <- C <- (m_undo)
    // becomes
    // A <- B <- C <- X <- (m_undo)
    m_undo.swap(t->m_next);
    m_undo.swap(t);

    // Clear all of the redo-able actions.
    m_redo.reset(nullptr);

    m_status = status::ready;
    on_status_change();
}

}
}
