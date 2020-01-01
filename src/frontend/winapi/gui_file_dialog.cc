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
#if USE_NATIVE_FILE_DIALOG
#define DRNSF_FRONTEND_IMPLEMENTATION
#include "gui.hh"

namespace drnsf {
namespace gui {

// (const) max_filename_chars
// The maximum number of characters in the user's selected filename, including
// the null terminator. Most path support on Windows is limited near 256 due to
// legacy limitations.
const int max_filename_chars = 1024;

// declared in gui.hh
struct file_dialog::impl : OPENFILENAMEW {
    wchar_t m_filename[max_filename_chars];
};

// declared in gui.hh
file_dialog::file_dialog()
{
    M = new impl{};
    M->lStructSize = sizeof(OPENFILENAMEW);
    M->lpstrFile = M->m_filename;
    M->nMaxFile = max_filename_chars;
    M->Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
}

// declared in gui.hh
file_dialog::~file_dialog()
{
    delete M;
}

// declared in gui.hh
std::string file_dialog::get_dir() const
{
    // TODO
    return "";
}

// declared in gui.hh
void file_dialog::set_dir(std::string dir)
{
    // TODO
}

// declared in gui.hh
std::string file_dialog::get_filename() const
{
    return util::wstr_to_u8str(M->m_filename);
}

// declared in gui.hh
void file_dialog::set_filename(std::string filename)
{
    // Convert the filename to windows wide-string UNICODE format.
    auto filename_w = util::u8str_to_wstr(filename);

    // Apply the character limit to the input filename.
    if (filename_w.size() >= max_filename_chars) {
        filename_w.erase(max_filename_chars);
    }

    // Copy the filename and the null terminator to the buffer.
    std::memcpy(
        M->m_filename,
        filename.data(),
        (filename.size() + 1) * sizeof(wchar_t)
    );
}

// declared in gui.hh
bool file_dialog::run_open()
{
    M->Flags |= OFN_FILEMUSTEXIST;
    DRNSF_ON_EXIT { M->Flags &= ~OFN_FILEMUSTEXIST; };

    return GetOpenFileNameW(M);
}

// declared in gui.hh
bool file_dialog::run_save()
{
    return GetSaveFileNameW(M);
}

}
}

#endif
