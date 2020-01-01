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
teller::teller() :
    m_done(false),
    m_desc("[UNLABELED ACTION - REPORT BUG]")
{
}

// declared in transact.hh
teller::~teller()
{
    // If this teller never committed, then the destructor should rollback
    // all of its operations. This is done by executing each of the
    // operations again in reverse order.
    if (!m_done) {
        for (auto &&op : m_ops) {
            op->execute();
        }
    }
}

// declared in transact.hh
std::unique_ptr<transaction> teller::commit()
{
    // Ensure this teller hasn't already committed its transaction.
    if (m_done) {
        throw std::logic_error("transact::teller::commit: already committed");
    }

    // Mark this teller as complete.
    m_done = true;

    // Produce the resulting transaction object and return it.
    return std::unique_ptr<transaction>(
        new transaction(
            std::move(m_ops),
            m_desc
        )
    );
}

// declared in transact.hh
void teller::describe(std::string desc)
{
    m_desc = desc;
}

// declared in transact.hh
void teller::push_op(std::unique_ptr<operation> op)
{
    // Ensure this teller hasn't already committed its transaction.
    if (m_done) {
        throw std::logic_error("transact::teller::push_op: already committed");
    }

    // Add the operation at the front of the list. When a transaction (or
    // teller) is rolled back, the operations are undone front-to-back.
    //
    // This operation 'moves' the op into the list, so the variable `op'
    // is no longer usable after this.
    m_ops.push_front(std::move(op));

    // Execute the operation. This is done after pushing the operation in
    // case pushing throws an exception (out-of-memory, etc).
    //
    // `op' cannot be invoked directly anymore because it was moved from.
    m_ops.front()->execute();
}

}
}
