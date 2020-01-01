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
#include "core.hh"
#include <iostream>
#include "nsf.hh"
#include "misc.hh"

namespace drnsf {
namespace core {

// (s-var) s_game_ver
// The game version being loaded and unloaded.
// FIXME - this maybe should not be global, in case subcommands can be run
// simultaneously from scripts (or otherwise) in the future?
static nsf::game_ver s_game_ver;

// (s-func) equal_without_padding
// Returns true if the two blobs given are equivalent after trimming all
// trailing zero bytes from them, if any.
//
// For example, returns true for the following:
//
// lhs: { 1, 2, 3, 4 }
// rhs: { 1, 2, 3, 4, 0, 0, 0 }
//
// lhs: { 1, 2, 3, 0, 0 }
// rhs: { 1, 2, 3, 0 }
//
// This is necessary as imported data may be captured with padding data which
// is not present in re-exported data, since this padding may be applied by
// its container's export function.
bool equal_without_padding(const util::blob &lhs, const util::blob &rhs)
{
    const auto nonzero = [](uint8_t x) { return x; };

    auto mismatch = std::mismatch(
        lhs.begin(),
        lhs.end(),
        rhs.begin(),
        rhs.end()
    );

    if (mismatch.first == lhs.end()) {
        if (mismatch.second == rhs.end()) {
            return true;
        }
        return std::find_if(mismatch.second, rhs.end(), nonzero) == rhs.end();
    } else if (mismatch.second == rhs.end()) {
        return std::find_if(mismatch.first, lhs.end(), nonzero) == lhs.end();
    } else {
        return false;
    }
}

// (s-func) do_entry
// Processes a given raw_entry, exports the resulting entry, and then compares
// the exported data with the input data to ensure a lossless import->export
// operation. In the case of any mismatch, the function prints an error message
// and returns false. Otherwise, the function returne true.
static bool do_entry(TRANSACT, std::string filename, nsf::raw_entry::ref src)
{
    bool ok = true;

    uint32_t in_type = src->get_type();
    auto in_items = src->get_items();

    nsf::entry::ref entry = src;
    src->process_by_type(TS, s_game_ver);

    uint32_t out_type;
    auto out_items = entry->export_entry(out_type);

    if (in_type != out_type) {
        ok = false;
        std::cerr
            << filename
            << ": \033[46;30m  entry  \033[0m "
            << "resave \033[31mtype\033[0m mismatch on `"
            << entry.full_path()
            << "'."
            << std::endl;
    }

    if (in_items != out_items) {
        ok = false;
        std::cerr
            << filename
            << ": \033[46;30m  entry  \033[0m "
            << "resave item mismatch on `"
            << entry.full_path()
            << "'."
            << std::endl;
    }

    return ok;
}

// (s-func) do_pagelet
// FIXME explain
static bool do_pagelet(TRANSACT, std::string filename, misc::raw_data::ref src)
{
    bool ok = true;

    util::blob in_data = src->get_data();

    nsf::raw_entry::ref raw_entry = src;
    src->rename(TS, src / "_PROCESSING");
    src /= "_PROCESSING";
    raw_entry.create(TS, src->get_proj());
    raw_entry->import_file(TS, src->get_data());
    src->destroy(TS);

    util::blob out_data = raw_entry->export_file();

    if (!equal_without_padding(in_data, out_data)) {
        ok = false;
        std::cerr
            << filename
            << ": \033[45;30m pagelet \033[0m "
            << "resave data mismatch on `"
            << raw_entry.full_path()
            << "'."
            << std::endl;
    }

    ok &= do_entry(TS, filename, raw_entry);

    return ok;
}

// (s-func) do_page
// FIXME explain
static bool do_page(TRANSACT, std::string filename, misc::raw_data::ref src)
{
    bool ok = true;

    util::blob in_data = src->get_data();

    if (src->get_data()[2] == 1) {
        // This is a texture page if the type is 1.
        // TODO
    } else {
        // For all other types, this is a standard page.
        nsf::spage::ref spage = src;
        src->rename(TS, src / "_PROCESSING");
        src /= "_PROCESSING";
        spage.create(TS, src->get_proj());
        spage->import_file(TS, src->get_data());
        src->destroy(TS);

        util::blob out_data = spage->export_file();

        if (in_data != out_data) {
            ok = false;
            std::cerr
                << filename
                << ": \033[43;30m  spage  \033[0m "
                << "resave data mismatch on `"
                << spage.full_path()
                << "'."
                << std::endl;
        }

        for (misc::raw_data::ref pagelet : spage->get_pagelets()) {
            ok &= do_pagelet(TS, filename, pagelet);
        }
    }

    return ok;
}

// (s-func) do_nsf
// FIXME explain
static bool do_nsf(TRANSACT, std::string filename, misc::raw_data::ref src)
{
    bool ok = true;

    util::blob in_data = src->get_data();

    nsf::archive::ref archive = src;
    src->rename(TS, src / "_PROCESSING");
    src /= "_PROCESSING";
    archive.create(TS, src->get_proj());
    archive->import_file(TS, src->get_data());
    src->destroy(TS);

    util::blob out_data = archive->export_file();

    if (in_data != out_data) {
        ok = false;
        std::cerr
            << filename
            << ": \033[41;30m archive \033[0m "
            << "resave data mismatch on `"
            << archive.full_path()
            << "'."
            << std::endl;
    }

    for (misc::raw_data::ref page : archive->get_pages()) {
        ok &= do_page(TS, filename, page);
    }

    return ok;
}

// FIXME explain
int cmd_resave_test(cmdenv e)
{
    argparser o;
    o.add_opt("help", [&]{ e.help_requested = true; });
    o.add_opt("c1", [&]{ s_game_ver = nsf::game_ver::crash1; });
    o.add_opt("c2", [&]{ s_game_ver = nsf::game_ver::crash2; });
    o.add_opt("c3", [&]{ s_game_ver = nsf::game_ver::crash3; });
    o.alias_opt('h', "help");
    o.begin(e.argv);

    if (e.help_requested) {
        std::cout << R"(Usage:

    drnsf :resave-test {--c1 | --c2 | --c3} <file>...

Runs resave tests against the given NSF files. For each given NSF file,
DRNSF will import the NSF file and process it, then re-export the
processed data. If there is any mismatch between the input and output
data, messages will be printed for each such mismatch, and the program
will exit with a failure code.

No files are overwritten by this command. All exporting is in-memory.

This command is not intended to check an NSF file for consistency, but
is instead intended to test the internal import/export code in DRNSF
against a large set of pre-existing NSF files.

Example usage:

    # Run resave checks against Snow Go and Piston It Away
    drnsf :resave-test --c2 S000000E.NSF S0000010.NSF

    # Run resave checks against Dino Might!
    drnsf :resave-test --c3 S0000010.NSF

    # Run resave checks against various game versions
    drnsf :resave-test --c2 crash2/*/*.NSF --c3 crash3/*/*.NSF
)"
            << std::endl;
        return EXIT_SUCCESS;
    }

    if (o.pump_eof()) {
        std::cerr
            << "drnsf resave-test: No files specified.\n\n"
            << "Try: drnsf :help resave-test"
            << std::endl;
        return EXIT_FAILURE;
    }

    bool ok = true;

    while (!o.pump_eof()) {
        std::string arg;
        o >> arg;

        try {
            util::file nsf_file;
            nsf_file.open(arg, "rb");

            nsf_file.seek(0, SEEK_END);
            auto nsf_size = nsf_file.tell();
            nsf_file.seek(0, SEEK_SET);

            util::blob nsf_data(nsf_size);
            nsf_file.read(nsf_data.data(), nsf_size);
            nsf_file.close();

            res::project proj;
            proj.get_transact().run([&](TRANSACT) {
                misc::raw_data::ref nsfile = proj.get_asset_root() / "nsfile";
                nsfile.create(TS, proj);
                nsfile->set_data(TS, nsf_data);
                ok &= do_nsf(TS, arg, nsfile);
            });
        } catch (std::exception &ex) {
            std::cerr
                << arg
                << ": "
                << ex.what()
                << std::endl;
            ok = false;
        }
    }

    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

}
}
