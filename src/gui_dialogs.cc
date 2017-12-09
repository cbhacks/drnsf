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
#include "gui.hh"
#include <cstring>
#include "fs.hh"

namespace drnsf {
namespace gui {

namespace {

class file_dialog : private window, private widget_im {
private:
    bool m_ok;
    fs::path m_cdir;
    std::string m_filename;

    void frame() override
    {
        // Show the current filename textbox.
        char filename[2048];
        std::strncpy(filename, m_filename.c_str(), 2048);
        filename[2047] = '\0';
        if (ImGui::InputText("Filename", filename, 2048)) {
            m_filename = filename;
        }

        // "OK" confirmation button. Return successful if the path entered is
        // a non-directory, or just browse to that directory if it is one.
        if (ImGui::Button("OK")) {
            auto new_path = fs::path(m_filename);
            if (new_path.is_absolute()) {
                m_cdir = new_path;
            } else {
                m_cdir /= m_filename;
            }
            m_cdir = fs::canonical(m_cdir);
            m_filename = "";
            if (!fs::exists(m_cdir) || !fs::is_directory(m_cdir)) {
                m_ok = true;
                exit_dialog();
                return;
            }
        }

        // Cancel button to exit in failure. `m_ok' should have been cleared by
        // the caller, such as `show_open' or `show_save'.
        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            exit_dialog();
        }

        // Reset the current directory if it isn't actually a directory.
        if (!fs::exists(m_cdir) || !fs::is_directory(m_cdir)) {
            m_cdir = fs::current_path();
        }

        // Show an optional "up" button to browse to the current directory's
        // parent directory. This should not be displayed when at the root.
        if (m_cdir.has_parent_path()) {
            if (ImGui::Button("<-")) {
                m_cdir = m_cdir.parent_path();
            }
            ImGui::SameLine();
        }

        // Show the current directory as an absolute path.
        ImGui::TextUnformatted(m_cdir.c_str());

        // Display a listing of the directory's contents. "." and ".." are
        // skipped automatically by the fs iterator.
        ImGui::BeginChild("Directory Contents", ImVec2(0, 0));
        ImGui::Columns(2);
        for (auto &&de : fs::directory_iterator(m_cdir)) {
            auto de_path = fs::canonical(de.path());
            auto de_filename = de_path.filename();

            // Display this entry's name as a selectable object. If it is
            // clicked, browse into it if it is a directory, or fill it into
            // the filename textbox otherwise.
            if (ImGui::Selectable(de_filename.c_str(), de_filename == m_filename)) {
                if (fs::is_directory(de)) {
                    m_cdir = de_path;
                    m_filename = "";
                } else {
                    m_filename = de_filename.c_str();
                }
            }
            ImGui::NextColumn();

            // Display size or type information about this file in a second
            // column.
            if (fs::is_regular_file(de_path)) {
                ImGui::Text("%" PRIuMAX, fs::file_size(de_path));
            } else if (fs::is_directory(de_path)) {
                ImGui::Text("<DIR>");
            } else {
                ImGui::Text("<SPECIAL>");
            }
            ImGui::NextColumn();
        }
        ImGui::Columns(1);
        ImGui::EndChild();
    }

public:
    file_dialog() :
        window("File Browser", 400, 300),
        widget_im(*this, layout::fill())
    {
        widget_im::show();
    }

    bool show_open(std::string &path)
    {
        m_cdir = fs::canonical(path);
        if (!fs::is_directory(m_cdir)) {
            m_filename = m_cdir.filename();
            m_cdir = m_cdir.parent_path();
        } else {
            m_filename = "";
        }
        m_ok = false;
        show_dialog();
        if (m_ok) {
            path = m_cdir;
            return true;
        } else {
            return false;
        }
    }
};

}

// declared in gui.hh
bool show_open_dialog(std::string &path)
{
    return file_dialog{}.show_open(path);
}

}
}
