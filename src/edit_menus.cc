//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2018  DRNSF contributors
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
#include <fstream>
#include "nsf.hh"
#include "misc.hh"

// FIXME temporary hack for gtk_main_quit
#if USE_GTK3
#include <gtk/gtk.h>
#endif

namespace drnsf {
namespace edit {
namespace menus {

// declared in edit.hh
void mni_open::on_activate()
{
    // Get the file to open from the user.
    std::string path;
    if (!gui::show_open_dialog(path)) return;

    // Create the new project to import into.
    auto proj_p = m_ctx.get_proj(); //FIXME
    auto &proj = *proj_p;

    proj.get_transact().run([&](TRANSACT) {
        TS.describe("Import NSF");

        // Open the NSF file and read the data into memory.
        auto nsf_file = util::fstream_open_bin(path, std::fstream::in);
        nsf_file.exceptions(std::fstream::failbit | std::fstream::eofbit);

        nsf_file.seekg(0, std::fstream::end);
        auto nsf_size = nsf_file.tellg();
        nsf_file.seekg(0, std::fstream::beg);

        util::blob nsf_data(nsf_size);
        nsf_file.read(
            reinterpret_cast<char *>(nsf_data.data()),
            nsf_size
        );
        nsf_file.close();

        // Import the data into an NSF asset.
        nsf::archive::ref nsf_asset = proj.get_asset_root() / "nsfile";
        nsf_asset.create(TS, proj);
        nsf_asset->import_file(TS, std::move(nsf_data));

        // Process all of the pages in the new NSF asset.
        for (misc::raw_data::ref page : nsf_asset->get_pages()) {

            // Pages with type 1 cannot be processed as normal pages.
            if (page->get_data()[2] == 1)
                continue;

            nsf::spage::ref spage = page;
            page->rename(TS, page / "_PROCESSING");
            page /= "_PROCESSING";
            spage.create(TS, proj);
            spage->import_file(TS, page->get_data());
            page->destroy(TS);

            // Process all of the entries inside each standard page.
            for (misc::raw_data::ref pagelet : spage->get_pagelets()) {
                nsf::raw_entry::ref entry = pagelet;
                pagelet->rename(TS, pagelet / "_PROCESSING");
                pagelet /= "_PROCESSING";
                entry.create(TS, proj);
                entry->import_file(TS, pagelet->get_data());
                pagelet->destroy(TS);
                entry->process_by_type(TS, nsf::game_ver::crash2);
            }
        }
    });

    // Point the context to the newly opened project.
    // TODO
}

// declared in edit.hh
void mni_save_as::on_activate()
{
    // Verify that there is an open project.
    auto proj = m_ctx.get_proj();
    if (!proj) {
        // TODO - error message box?
        return;
    }

    // Verify that there is an appropriate NSF asset to be saved.
    nsf::archive::ref nsf_asset = proj->get_asset_root() / "nsfile";
    if (!nsf_asset.ok()) {
        // TODO - error message box
        return;
    }

    // Get the file to save to from the user.
    std::string path;
    if (!gui::show_save_dialog(path)) return;

    // TODO - make the remaining code asynchronous to not block the UI

    // Deserialize all of the assets referenced (directly or indirectly) into
    // one large blob of NSF file data.
    util::blob nsf_data;
    /*try*/ {
        nsf_data = nsf_asset->export_file();
    } /*catch (?) {
        TODO - handle errors
    }*/

    // Write the NSF data into the file specified by the user.
    /*try*/ {
        auto nsf_file = util::fstream_open_bin(path, std::fstream::out);
        nsf_file.exceptions(std::fstream::failbit);
        nsf_file.write(
            reinterpret_cast<char *>(nsf_data.data()),
            nsf_data.size()
        );
    } /*catch (?) {
        TODO - handle errors
    }*/
}

// declared in edit.hh
void mni_exit::on_activate()
{
    // Check for unsaved changes and prompt the user to confirm they wish to
    // exit without saving.
    // TODO

    gui::end();
}

// (s-func) get_undo_str
// Gets the description of the specified context's undo transaction if it can be
// undone, or null if there is some reason this cannot be done. Used by
// mni_undo::update.
static const char *get_undo_str(context &ctx)
{
    auto proj = ctx.get_proj();
    if (!proj) return nullptr;

    auto &tsn = proj->get_transact();
    if (tsn.get_status() != transact::status::ready) return nullptr;
    if (!tsn.has_undo()) return nullptr;

    return tsn.get_undo().describe();
}

// declared in edit.hh
void mni_undo::update()
{
    auto str = get_undo_str(m_ctx);
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
    auto proj = m_ctx.get_proj();
    if (!proj) return;

    auto &tsn = proj->get_transact();
    if (tsn.get_status() != transact::status::ready) return;
    if (!tsn.has_undo()) return;

    tsn.undo();
}

// declared in edit.hh
mni_undo::mni_undo(gui::menu &menu, context &ctx) :
    item(menu, "<undo>"),
    m_ctx(ctx)
{
    h_status_change <<= [this]{
        update();
    };
    h_status_change.bind(ctx.get_proj()->get_transact().on_status_change);
    // FIXME handle context project change
    update();
}

// (s-func) get_redo_str
// Gets the description of the specified context's redo transaction if it can be
// redone, or null if there is some reason this cannot be done. Used by
// mni_redo::update.
static const char *get_redo_str(context &ctx)
{
    auto proj = ctx.get_proj();
    if (!proj) return nullptr;

    auto &tsn = proj->get_transact();
    if (tsn.get_status() != transact::status::ready) return nullptr;
    if (!tsn.has_redo()) return nullptr;

    return tsn.get_redo().describe();
}

// declared in edit.hh
void mni_redo::update()
{
    auto str = get_redo_str(m_ctx);
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
    auto proj = m_ctx.get_proj();
    if (!proj) return;

    auto &tsn = proj->get_transact();
    if (tsn.get_status() != transact::status::ready) return;
    if (!tsn.has_redo()) return;

    tsn.redo();
}

// declared in edit.hh
mni_redo::mni_redo(gui::menu &menu, context &ctx) :
    item(menu, "<redo>"),
    m_ctx(ctx)
{
    h_status_change <<= [this]{
        update();
    };
    h_status_change.bind(ctx.get_proj()->get_transact().on_status_change);
    // FIXME handle context project change
    update();
}

}
}
}
