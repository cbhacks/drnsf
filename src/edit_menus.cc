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
#include "edit.hh"

namespace drnsf {
namespace edit {
namespace menus {

// declared in edit.hh
void mni_exit::on_activate()
{
    gtk_main_quit();
}

// (s-func) get_undo_str
// Gets the description of the specified editor's undo transaction if it can be
// undone, or null if there is some reason this cannot be done. Used by
// mni_undo::update.
static const char *get_undo_str(editor &ed)
{
    auto proj = ed.get_proj();
    if (!proj) return nullptr;

    auto &tsn = proj->get_transact();
    if (tsn.get_status() != transact::status::ready) return nullptr;
    if (!tsn.has_undo()) return nullptr;

    return tsn.get_undo().describe();
}

// declared in edit.hh
void mni_undo::update()
{
    auto str = get_undo_str(m_ed);
    if (str) {
        set_text("Undo: $"_fmt(str));
        set_enabled(true);
    } else {
        set_text("Undo");
        set_enabled(false);
    }
}

// declared in edit.hh
void mni_undo::on_activate()
{
    auto proj = m_ed.get_proj();
    if (!proj) return;

    auto &tsn = proj->get_transact();
    if (tsn.get_status() != transact::status::ready) return;
    if (!tsn.has_undo()) return;

    tsn.undo();
}

// declared in edit.hh
mni_undo::mni_undo(gui::menu &menu, editor &ed) :
    item(menu, "<undo>"),
    m_ed(ed)
{
    h_status_change <<= [this]{
        update();
    };
    h_status_change.bind(ed.get_proj()->get_transact().on_status_change);
    // FIXME handle editor project change
    update();
}

// (s-func) get_redo_str
// Gets the description of the specified editor's redo transaction if it can be
// redone, or null if there is some reason this cannot be done. Used by
// mni_redo::update.
static const char *get_redo_str(editor &ed)
{
    auto proj = ed.get_proj();
    if (!proj) return nullptr;

    auto &tsn = proj->get_transact();
    if (tsn.get_status() != transact::status::ready) return nullptr;
    if (!tsn.has_redo()) return nullptr;

    return tsn.get_redo().describe();
}

// declared in edit.hh
void mni_redo::update()
{
    auto str = get_redo_str(m_ed);
    if (str) {
        set_text("Redo: $"_fmt(str));
        set_enabled(true);
    } else {
        set_text("Redo");
        set_enabled(false);
    }
}

// declared in edit.hh
void mni_redo::on_activate()
{
    auto proj = m_ed.get_proj();
    if (!proj) return;

    auto &tsn = proj->get_transact();
    if (tsn.get_status() != transact::status::ready) return;
    if (!tsn.has_redo()) return;

    tsn.redo();
}

// declared in edit.hh
mni_redo::mni_redo(gui::menu &menu, editor &ed) :
    item(menu, "<redo>"),
    m_ed(ed)
{
    h_status_change <<= [this]{
        update();
    };
    h_status_change.bind(ed.get_proj()->get_transact().on_status_change);
    // FIXME handle editor project change
    update();
}

}
}
}
