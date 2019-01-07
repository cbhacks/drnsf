//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2019  DRNSF contributors
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

// declared in edit.hh
void field<util::blob>::frame()
{
    const util::blob empty = {};
    auto &obj = m_object ? *m_object : empty;
    // If there is no blob bound to the field, display the UI as if there
    // was a zero-length blob bound to it.
    //
    // Important: Do not raise on_change if no blob is bound (m_object is
    // null).

    const int viscols = 16;       // Hex-editor body width in columns.
    const int visrows = 16;       // Hex-editor body height in rows.

    const ImVec2 inner_glyph_size   // The size of one character of text.
        = ImGui::CalcTextSize("F");
    const ImVec2 glyph_size = {     // ... plus padding.
        inner_glyph_size.x + 1,
        inner_glyph_size.y + 1
    };

    const int col_heading_space = 1;
    const int row_heading_space = 12;

    // Compute the number of rows of bytes for the hex editor. This is the
    // number of bytes divided by column count rounded *up*.
    int rows = (obj.size() + viscols - 1) / viscols;
    if (rows < visrows) {
        // If there are less rows of actual data than the visible area, pad up
        // to the full height.
        rows = visrows;
    }

    // Disable the default padding/spacing and background color.
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0, 0});
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, 0});
    ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, {});

    ImGui::BeginChild(
        "###hex_editor",
        {
            glyph_size.x * (4 + viscols * 2.5f)
                + row_heading_space
                + ImGui::GetStyle().ScrollbarSize,
            glyph_size.y * (1 + visrows)
                + col_heading_space
        }
    );

    // Display the column heading.
    ImGui::PushItemWidth(glyph_size.x * 2.5);
    for (int col = 0; col < viscols; col++) {
        ImGui::SetCursorPos({row_heading_space + glyph_size.x * (4 + col * 2.5f), 0});
        ImGui::Text("%2X", col);
    }
    ImGui::PopItemWidth();

    // Place a separator line between the column heading and the body.
    ImGui::SetCursorPos({0, glyph_size.y});
    ImGui::Separator();

    ImGui::SetCursorPos({0, glyph_size.y + col_heading_space});
    ImGui::SetNextWindowContentSize({
            glyph_size.x * (4 + viscols * 2.5f) + row_heading_space,
            glyph_size.y * rows
    });
    ImGui::BeginChild(
        "###hex_editor_body",
        {
            glyph_size.x * (4 + viscols * 2.5f)
                + row_heading_space
                + ImGui::GetStyle().ScrollbarSize,
            glyph_size.y * visrows
        },
        false,
        ImGuiWindowFlags_AlwaysVerticalScrollbar
    );

    // Move the byte selection if the user has pressed the relevant arrow keys
    // (or similar).
    if (ImGui::IsWindowFocused()) {
        int offset = 0;

        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow))) {
            offset--;
        }
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow))) {
            offset++;
        }
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow))) {
            offset -= viscols;
        }
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow))) {
            offset += viscols;
        }
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_PageUp))) {
            offset -= viscols * visrows;
        }
        if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_PageDown))) {
            offset += viscols * visrows;
        }

        if (offset) {
            // Move the selection by the computed offset. Clamp to the range of
            // the byte list.
            if (offset < 0 && size_t(-offset) > m_selected_byte) {
                // If moving before obj[0], simply move to obj[0] instead.
                m_selected_byte = 0;
            } else {
                m_selected_byte += offset;
                if (m_selected_byte >= obj.size()) {
                    m_selected_byte = obj.size() - 1;
                }
            }
            m_input_len = 0;

            // TODO - scroll to the selected byte
        }
    }

    // Process text input (0-9A-Fa-f).
    if (m_selected_byte < obj.size() && ImGui::IsWindowFocused()) {
        for (auto &&c : ImGui::GetIO().InputCharacters) {
            if (!c) break;

            int value = -1;
            if (c >= '0' && c <= '9') {
                value = c - '0';
            } else if (c >= 'A' && c <= 'F') {
                value = 0xA + c - 'A';
            } else if (c >= 'a' && c <= 'f') {
                value = 0xA + c - 'a';
            }

            if (value != -1) {
                m_input_value <<= 4;
                m_input_value |= value & 0xF;
                m_input_len++;
            }

            if (m_input_len == 2) {
                if (m_selected_byte < obj.size()) {
                    util::blob new_value = obj;
                    new_value[m_selected_byte] = m_input_value;
                    on_change(std::move(new_value));
                }

                m_selected_byte++;
                if (m_selected_byte >= obj.size()) {
                    m_selected_byte = obj.size() - 1;
                }
                m_input_len = 0;

                // TODO - scroll to the selected byte
            }
        }
        ImGui::GetIO().ClearInputCharacters();
    }


    // Display each row which is visible in the scrolling region.
    int top_row = ImGui::GetScrollY() / glyph_size.y;
    int bottom_row = top_row + 1 + visrows;
    for (int row = top_row; row < bottom_row; row++) {

        // Display the row heading for this row.
        ImGui::PushItemWidth(glyph_size.x * 4);
        ImGui::SetCursorPos({0, glyph_size.y * row});
        ImGui::Text("%4X", row * viscols);
        ImGui::PopItemWidth();

        ImGui::PushItemWidth(glyph_size.x * 2.5);
        for (int col = 0; col < 16; col++) {
            size_t i = row * 16 + col;
            if (i >= obj.size()) {
                break;
            }

            ImGui::SetCursorPos({
                glyph_size.x * (4 + col * 2.5f) + row_heading_space,
                glyph_size.y * row
            });

            if (i == m_selected_byte) {
                ImGui::PushStyleColor(ImGuiCol_Text, {
                    0.7f, 0.0f, 0.0f, 1.0f
                });
                if (m_input_len) {
                    ImGui::Text("%01X_", m_input_value & 0xF);
                } else {
                    ImGui::Text("%02X", obj[i]);
                }
                ImGui::PopStyleColor();
            } else {
                if (obj[i]) {
                    ImGui::PushStyleColor(ImGuiCol_Text, {
                        0.1f, 0.1f, 0.1f, 1.0f
                    });
                } else {
                    ImGui::PushStyleColor(ImGuiCol_Text, {
                        0.6f, 0.6f, 0.6f, 1.0f}
                    );
                }

                ImGui::Text("%02X", obj[i]);
                ImGui::PopStyleColor();

                // Switch to this byte if clicked.
                if (ImGui::IsItemClicked()) {
                    m_selected_byte = i;
                    m_input_len = 0;

                    // TODO - scroll to the selected byte
                }
            }
        }
        ImGui::PopItemWidth();
    }

    ImGui::EndChild();
    ImGui::EndChild();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);
}

}
}
