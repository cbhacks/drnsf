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
#include <cstring>

DRNSF_DECLARE_EMBED(imprints::infousa_dat);
DRNSF_DECLARE_EMBED(imprints::infoeur_dat);
DRNSF_DECLARE_EMBED(imprints::infojap_dat);

namespace drnsf {
namespace core {

// FIXME explain
int cmd_cdxa_imprint(cmdenv e)
{
    uint8_t sysinfo[37632];
    bool sysinfo_set = false;

    argparser o;
    o.add_opt("help", [&]{ e.help_requested = true; });
    o.alias_opt('h', "help");
    o.add_opt("info-file", [&](std::string filename) {
        util::file sysinfo_file;
        sysinfo_file.open(filename, "rb");

        sysinfo_file.seek(0, SEEK_END);
        auto sysinfo_size = sysinfo_file.tell();
        sysinfo_file.seek(0, SEEK_SET);

        if (std::streamoff(sysinfo_size) != sizeof(sysinfo)) {
            throw arg_error("--info-file: file is the wrong size");
        }
        sysinfo_file.read(sysinfo, sizeof(sysinfo));
        sysinfo_set = true;
    }, true);
    o.add_opt("psx-scea", [&]{
        std::memcpy(sysinfo, embed::imprints::infousa_dat::data, 37632);
        sysinfo_set = true;
    }, true);
    o.add_opt("psx-scee", [&]{
        std::memcpy(sysinfo, embed::imprints::infoeur_dat::data, 37632);
        sysinfo_set = true;
    }, true);
    o.add_opt("psx-scei", [&]{
        std::memcpy(sysinfo, embed::imprints::infojap_dat::data, 37632);
        sysinfo_set = true;
    }, true);
    o.begin(e.argv);

    if (e.help_requested) {
        std::cout << R"(Usage:

    drnsf :cdxa-imprint --info-file=<file> <file>...
    drnsf :cdxa-imprint --psx-scea <file>...
    drnsf :cdxa-imprint --psx-scee <file>...
    drnsf :cdxa-imprint --psx-scei <file>...

Overwrites the system information section (the first 16 sectors) of the
specified CD-XA BIN images with the given system file. The following
options are available:

    --info-file=<file>
        Use the specified system information file. It must be 37632
        bytes in size.

    --psx-scea
    --psx-scee
    --psx-scei
        Use a standard system information image for PlayStation 1 game
        discs. You must choose between the three available regions:

        (SCEA) Sony Computer Entertainment America
            For consoles primarily sold in North America. Seen on game
            discs as "NTSC-U/C".

        (SCEE) Sony Computer Entertainment Europe
            For consoles primarily sold in Europe and some other areas
            such as Australia. Seen on game discs as "PAL".

        (SCEI) Sony Computer Entertainment, Inc.
            For consoles sold primarily in Japan and some other areas.
            Sometimes referred to as "NTSC-J" or "JAP".

        Some PlayStation console boot methods (and some emulators) will
        refuse to boot a game disc unless it contains a proper system
        information section as provided by these options.

The system information data (--info-file, --psx-..., etc) to use must
be specified before any BIN image arguments are given. You may specify
multiple such options to apply to different images; see the examples
below for more details.

Example usage:

    # Apply a custom system information image to a BIN file.
    drnsf :cdxa-imprint --info-file=myinfo.dat my_excellent_game.bin

    # Apply an NTSC-U image to several BIN files.
    drnsf :cdxa-imprint --scea disc_1.bin disc_2.bin disc_3.bin

    # Apply several sysinfo images to different BIN files.
    drnsf :cdxa-imprint --scea mymod_us.bin --scee mymod_eu.bin \
        --scei mymod_jp.bin --info-file=/dev/urandom mymod_garbage.bin
)"
            << std::endl;
        return EXIT_SUCCESS;
    }

    if (o.pump_eof()) {
        throw arg_error("No files specified.");
    }

    bool ok = true;

    while (!o.pump_eof()) {
        std::string arg;
        o >> arg;

        if (!sysinfo_set) {
            throw arg_error("No system information data specified.");
        }

        std::cout << arg << ": imprinting..." << std::endl;

        try {
            util::file file;
            file.open(arg, "rb+");
            file.write(sysinfo, sizeof(sysinfo));
        } catch (std::exception &ex) {
            std::cerr
                << arg
                << ": "
                << ex.what()
                << std::endl;
            ok = false;
        }
    }

    o.end();

    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

}
}
