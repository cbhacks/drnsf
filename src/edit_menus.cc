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
#include "edit.hh"

namespace drnsf {
namespace edit {
namespace menus {

// declared in edit.hh
template <nsf::game_ver GameVersion>
void mni_open<GameVersion>::on_activate()
{
    // Get the file to open from the user.
    std::string path;
    {
        gui::file_dialog dlg;
        if (!dlg.run_open()) return;
        path = dlg.get_filename();
    }

    // Create the new project to import into.
    auto proj_p = std::make_shared<res::project>();
    auto &proj = *proj_p;

    proj.get_transact().run([&](TRANSACT) {
        TS.describe("Import NSF");

        // Open the NSF file and read the data into memory.
        util::file nsf_file;
        nsf_file.open(path, "rb");

        nsf_file.seek(0, SEEK_END);
        auto nsf_size = nsf_file.tell();
        nsf_file.seek(0, SEEK_SET);

        util::blob nsf_data(nsf_size);
        nsf_file.read(nsf_data.data(), nsf_size);
        nsf_file.close();

        // Import the data into an NSF asset.
        nsf::archive::ref nsf_asset = proj.get_asset_root() / "nsfile";
        nsf_asset.create(TS, proj);
        nsf_asset->import_file(TS, std::move(nsf_data));

        // Process all of the pages in the new NSF asset.
        for (misc::raw_data::ref page : nsf_asset->get_pages()) {

            // Pages with type 1 should be processed as texture pages.
            if (page->get_data()[2] == 1) {
                nsf::tpage::ref tpage = page;
                page->rename(TS, page / "_PROCESSING");
                page /= "_PROCESSING";
                tpage.create(TS, proj);
                tpage->import_file(TS, page->get_data());
                page->destroy(TS);
                
                continue;
            }

            nsf::spage::ref spage = page;
            page->rename(TS, page / "_PROCESSING");
            page /= "_PROCESSING";
            spage.create(TS, proj);
            spage->import_file(TS, page->get_data());
            page->destroy(TS);

            // Process all of the entries inside each standard page.
            auto pagelets = spage->get_pagelets();
            for (auto &p : pagelets) {
                misc::raw_data::ref pagelet = p;

                nsf::raw_entry::ref entry = pagelet;
                pagelet->rename(TS, pagelet / "_PROCESSING");
                pagelet /= "_PROCESSING";
                entry.create(TS, proj);
                entry->import_file(TS, pagelet->get_data());
                pagelet->destroy(TS);

                // Rename the entry asset to "entries/<eid_here>". This brings
                // all of the entries together as siblings for easy access.
                auto new_path =
                    proj.get_asset_root() /
                    "entries" /
                    entry->get_eid().str();
                if (!new_path.get())
                    entry->rename(TS, new_path);
                entry = new_path;
                p = new_path;

                entry->process_by_type(TS, GameVersion);
            }
            spage->set_pagelets(TS, pagelets);
        }
    });

    // Point the context to the newly opened project.
    m_ctx.set_proj(proj_p);
}

// declared in edit.hh
mni_save_as::mni_save_as(gui::menu &menu, context &ctx) :
    item(menu, "Save As"),
    m_ctx(ctx)
{
    h_project_change <<= [this](const std::shared_ptr<res::project> &proj) {
        set_enabled(m_ctx.get_proj() != nullptr);
    };
    h_project_change.bind(ctx.on_project_change);
    h_project_change(ctx.get_proj());
}

// declared in edit.hh
void mni_save_as::on_activate()
{
    // Verify that there is an open project.
    auto proj = m_ctx.get_proj();
    if (!proj) {
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
    {
        gui::file_dialog dlg;
        if (!dlg.run_save()) return;
        path = dlg.get_filename();
    }

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
        util::file nsf_file;
        nsf_file.open(path, "wb");
        nsf_file.write(nsf_data.data(), nsf_data.size());
    } /*catch (?) {
        TODO - handle errors
    }*/
}

// declared in edit.hh
mni_close::mni_close(gui::menu &menu, context &ctx) :
    item(menu, "Close"),
    m_ctx(ctx)
{
    h_project_change <<= [this](const std::shared_ptr<res::project> &proj) {
        set_enabled(m_ctx.get_proj() != nullptr);
    };
    h_project_change.bind(ctx.on_project_change);
    h_project_change(ctx.get_proj());
}

// declared in edit.hh
void mni_close::on_activate()
{
    m_ctx.set_proj(nullptr);
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

    h_project_change <<= [this](const std::shared_ptr<res::project> &proj) {
        if (h_status_change.is_bound()) {
            h_status_change.unbind();
        }
        if (proj) {
            h_status_change.bind(proj->get_transact().on_status_change);
        }
        update();
    };
    h_project_change.bind(ctx.on_project_change);
    h_project_change(ctx.get_proj());
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

    h_project_change <<= [this](const std::shared_ptr<res::project> &proj) {
        if (h_status_change.is_bound()) {
            h_status_change.unbind();
        }
        if (proj) {
            h_status_change.bind(proj->get_transact().on_status_change);
        }
        update();
    };
    h_project_change.bind(ctx.on_project_change);
    h_project_change(ctx.get_proj());
}

// declared in edit.hh
void mni_new_window::on_activate()
{
    m_ctx.make_window<mode_window>().show();
}

template class mni_open<nsf::game_ver::crash1>;
template class mni_open<nsf::game_ver::crash2>;
template class mni_open<nsf::game_ver::crash3>;

}
}
}
