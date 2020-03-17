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
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../imgui/imgui.h"
#include "gui.hh"
#include "gl.hh"

DRNSF_DECLARE_EMBED(shaders::imgui::vertex_glsl);
DRNSF_DECLARE_EMBED(shaders::imgui::fragment_glsl);

namespace drnsf {
namespace gui {

static gl::texture s_font_tex;
static ImFontAtlas s_font_atlas;
static unsigned char *s_font_pixels = nullptr;
static int s_font_width;
static int s_font_height;
static gl::vert_array s_vao;
static gl::buffer s_ibo;
static gl::buffer s_vbo;
static gl::program s_prog;

// declared in gui.hh
void widget_im::draw_gl(int width, int height, unsigned int rbo)
{
    if (!s_font_tex.ok) {
        glBindTexture(GL_TEXTURE_2D, s_font_tex);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            s_font_width,
            s_font_height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            s_font_pixels
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindTexture(GL_TEXTURE_2D, 0);

        s_font_tex.ok = true;
    }

    if (!s_vao.ok) {
        glBindVertexArray(s_vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_ibo);
        glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            2,
            GL_FLOAT,
            false,
            sizeof(ImDrawVert),
            reinterpret_cast<void *>(offsetof(ImDrawVert, pos))
        );
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1,
            2,
            GL_FLOAT,
            false,
            sizeof(ImDrawVert),
            reinterpret_cast<void *>(offsetof(ImDrawVert, uv))
        );
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2,
            4,
            GL_UNSIGNED_BYTE,
            true,
            sizeof(ImDrawVert),
            reinterpret_cast<void *>(offsetof(ImDrawVert, col))
        );
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        s_vao.ok = true;
    }

    if (!s_prog.ok) {
        gl::vert_shader vert_shader;
        gl::shader_source(vert_shader, {
            "#version 140",
            embed::shaders::imgui::vertex_glsl::str
        });
        gl::compile_shader(vert_shader);

        gl::frag_shader frag_shader;
        gl::shader_source(frag_shader, {
            "#version 140",
            embed::shaders::imgui::fragment_glsl::str
        });
        gl::compile_shader(frag_shader);

        glAttachShader(s_prog, vert_shader);
        glAttachShader(s_prog, frag_shader);
        glBindAttribLocation(s_prog, 0, "a_Position");
        glBindAttribLocation(s_prog, 1, "a_TexCoord");
        glBindAttribLocation(s_prog, 2, "a_Color");
        glBindFragDataLocation(s_prog, 0, "f_Color");
        gl::link_program(s_prog);

        s_prog.ok = true;
    }

    gl::framebuffer fbo;
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glFramebufferRenderbuffer(
        GL_DRAW_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_RENDERBUFFER,
        rbo
    );

    glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // Exit now after clearing if ImGui::Render has not been called yet. This
    // happens in work().
    if (!m_render_ready) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        return;
    }

    ImGui::SetCurrentContext(m_im);
    ImDrawData *draw_data = ImGui::GetDrawData();

    glUseProgram(s_prog);
    int uni_screenortho = glGetUniformLocation(s_prog, "u_ScreenOrtho");
    glUniform1i(glGetUniformLocation(s_prog, "u_Font"), 0);

    // Prepare a simple 2D orthographic projection.
    //
    // This adjusts the GL vertex coordinates to be:
    // X left-to-right as 0 to +<width>
    // Y top-to-bottom as 0 to +<height>
    // ImGui expects this layout.
    //
    // Without this, instead you get -1 to +1 left-to-right *and also
    // bottom-to-top* which is the inverse of what we want.
    //
    // The Z coordinates are left as-is; they are not meaningful for ImGui.
    glUniformMatrix4fv(
        uni_screenortho,
        1,
        false,
        &glm::ortho<float>(
            0,
            width,
            height,
            0,
            -1,
            +1
        )[0][0]
    );

    glBindVertexArray(s_vao);

    // Enable alpha blending. ImGui requires this for its fonts.
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable the GL scissor test. This is used to clip gui widgets to only
    // render within their given area.
    glEnable(GL_SCISSOR_TEST);

    // Draw each of the command lists. Each list contains a set of draw
    // commands associated with a particular set of vertex arrays.
    for (int i = 0;i < draw_data->CmdListsCount;i++) {
        ImDrawList *draw_list = draw_data->CmdLists[i];

        glBindBuffer(GL_COPY_WRITE_BUFFER, s_vbo);
        glBufferData(
            GL_COPY_WRITE_BUFFER,
            draw_list->VtxBuffer.Size * sizeof(ImDrawVert),
            draw_list->VtxBuffer.Data,
            GL_DYNAMIC_DRAW
        );
        glBindBuffer(GL_COPY_WRITE_BUFFER, s_ibo);
        glBufferData(
            GL_COPY_WRITE_BUFFER,
            draw_list->IdxBuffer.Size * sizeof(ImDrawIdx),
            draw_list->IdxBuffer.Data,
            GL_DYNAMIC_DRAW
        );
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);

        // Draw each of the commands in the list.
        int index = 0;
        for (auto &c : draw_list->CmdBuffer) {
            glBindTexture(GL_TEXTURE_2D, s_font_tex);
            glScissor(
                c.ClipRect.x,
                height - c.ClipRect.w,
                c.ClipRect.z - c.ClipRect.x,
                c.ClipRect.w - c.ClipRect.y
            );
            glDrawElements(
                GL_TRIANGLES,
                c.ElemCount,
                sizeof(ImDrawIdx) == 2 ?
                    GL_UNSIGNED_SHORT :
                    GL_UNSIGNED_INT,
                reinterpret_cast<void *>(index * sizeof(ImDrawIdx))
            );

            // Move past the indices which have already been used
            // for drawing.
            index += c.ElemCount;
        }
    }

    // Re-disable scissor test.
    glDisable(GL_SCISSOR_TEST);

    // Unbind whatever texture was bound by the last draw command.
    glBindTexture(GL_TEXTURE_2D, 0);

    // Restore the previous blending setup.
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);

    glBindVertexArray(0);
    glUseProgram(0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    ImGui::SetCurrentContext(nullptr);
}

// declared in gui.hh
void widget_im::mousemove(int x, int y)
{
    m_io->MousePos.x = x;
    m_io->MousePos.y = y;
    m_remaining_time = 0;
}

// declared in gui.hh
void widget_im::mouseleave()
{
    m_io->MousePos.x = -FLT_MAX;
    m_io->MousePos.y = -FLT_MAX;
    m_remaining_time = 0;
}

// declared in gui.hh
void widget_im::mousewheel(int delta_y)
{
    m_io->MouseWheel += delta_y;
    m_remaining_time = 0;
}

// declared in gui.hh
void widget_im::mousebutton(mousebtn btn, bool down, keymods mods)
{
    switch (btn) {
    case mousebtn::left:
        m_io->MouseDown[0] = down;
        break;
    case mousebtn::middle:
        m_io->MouseDown[2] = down;
        break;
    case mousebtn::right:
        m_io->MouseDown[1] = down;
        break;
    }
    m_remaining_time = 0;
}

// declared in gui.hh
void widget_im::key(keycode code, bool down, keymods mods)
{
    switch (code) {
    case keycode::l_shift:
    case keycode::r_shift:
        m_io->KeyShift = down;
        break;
    case keycode::l_ctrl:
    case keycode::r_ctrl:
        m_io->KeyCtrl = down;
        break;
    case keycode::l_alt:
    case keycode::r_alt:
        m_io->KeyAlt = down;
        break;
    case keycode::l_super:
    case keycode::r_super:
        m_io->KeySuper = down;
        break;
    case keycode::tab:
        m_io->KeysDown[ImGuiKey_Tab] = down;
        break;
    case keycode::left_arrow:
        m_io->KeysDown[ImGuiKey_LeftArrow] = down;
        break;
    case keycode::right_arrow:
        m_io->KeysDown[ImGuiKey_RightArrow] = down;
        break;
    case keycode::up_arrow:
        m_io->KeysDown[ImGuiKey_UpArrow] = down;
        break;
    case keycode::down_arrow:
        m_io->KeysDown[ImGuiKey_DownArrow] = down;
        break;
    case keycode::page_up:
        m_io->KeysDown[ImGuiKey_PageUp] = down;
        break;
    case keycode::page_down:
        m_io->KeysDown[ImGuiKey_PageDown] = down;
        break;
    case keycode::home:
        m_io->KeysDown[ImGuiKey_Home] = down;
        break;
    case keycode::end:
        m_io->KeysDown[ImGuiKey_End] = down;
        break;
    case keycode::del:
        m_io->KeysDown[ImGuiKey_Delete] = down;
        break;
    case keycode::backspace:
        m_io->KeysDown[ImGuiKey_Backspace] = down;
        break;
    case keycode::enter:
        m_io->KeysDown[ImGuiKey_Enter] = down;
        break;
    case keycode::escape:
        m_io->KeysDown[ImGuiKey_Escape] = down;
        break;
    case keycode::A:
        m_io->KeysDown[ImGuiKey_A] = down;
        break;
    case keycode::C:
        m_io->KeysDown[ImGuiKey_C] = down;
        break;
    case keycode::V:
        m_io->KeysDown[ImGuiKey_V] = down;
        break;
    case keycode::X:
        m_io->KeysDown[ImGuiKey_X] = down;
        break;
    case keycode::Y:
        m_io->KeysDown[ImGuiKey_Y] = down;
        break;
    case keycode::Z:
        m_io->KeysDown[ImGuiKey_Z] = down;
        break;
    default:
        // Unused. This empty case silences a GCC warning about unhandled enum
        // values in the switch statement.
        break;
    }
    m_remaining_time = 0;
}

// declared in gui.hh
void widget_im::text(const char *str)
{
    m_io->AddInputCharactersUTF8(str);
    m_remaining_time = 0;
}

// declared in gui.hh
void widget_im::on_resize(int width, int height)
{
    m_render_ready = false;
    m_remaining_time = 0;
}

// declared in gui.hh
int widget_im::work() noexcept
{
    // Prevent recursion.
    if (m_busy) return INT_MAX;
    m_busy = true;
    DRNSF_ON_EXIT { m_busy = false; };

    auto delta_ms = m_stopwatch.lap();
    m_pending_time += delta_ms;
    m_remaining_time -= delta_ms;
    if (m_remaining_time <= 0) {
        // Don't allow rendering recursively while running the frame.
        m_render_ready = false;

        int width, height;
        get_real_size(width, height);

        m_io->DisplaySize.x = width;
        m_io->DisplaySize.y = height;

        m_io->DeltaTime = m_pending_time / 1000.0;
        m_pending_time = 0;

        ImGui::SetCurrentContext(m_im);
        ImGui::NewFrame();

        ImGui::SetNextWindowPos({0, 0});
        ImGui::SetNextWindowSize({
            static_cast<float>(width),
            static_cast<float>(height)
        });
        ImGui::Begin(
            "###Contents",
            nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse);
        frame();
        ImGui::End();

        ImGui::Render();
        ImGui::SetCurrentContext(nullptr);

        m_render_ready = true;
        m_remaining_time = 100;
        invalidate();
    }
    return m_remaining_time;
}

// declared in gui.hh
widget_im::widget_im(container &parent, layout layout) :
    widget_gl(parent, layout)
{
    // Grab the font if this is the first widget_im to be created. It is
    // important that this not be done while inside the frame() handler for a
    // widget_im, as that would get the font for an ImGui context which only has
    // the lifetime of that one widget_im. For the first construction, there is
    // no chance of this happening, so the font will be from the global context
    // provided by default.
    if (!s_font_pixels) {
        s_font_atlas.GetTexDataAsRGBA32(
            &s_font_pixels,
            &s_font_width,
            &s_font_height
        );
    }

    m_im = ImGui::CreateContext(&s_font_atlas);
    m_io = &ImGui::GetIO();

    m_io->IniFilename = "imgui.ini";

    // Configure the ImGui keymap.
    for (int i = 0;i < ImGuiKey_COUNT;i++) {
        m_io->KeyMap[i] = i;
    }

    // Set up an appropriate ImGui style. The default is white-on-black which
    // clashes with most other defaults.
    auto &style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    ImVec4 default_color = { 1.0f, 0.0f, 1.0f, 0.25f };
    style.Colors[ImGuiCol_Text]                  = { 0.0f, 0.0f, 0.0f, 1.0f };
    style.Colors[ImGuiCol_TextDisabled]          = default_color;
    style.Colors[ImGuiCol_WindowBg]              = { 0.0f, 0.0f, 0.0f, 0.0f };
    style.Colors[ImGuiCol_ChildBg]               = default_color;
    style.Colors[ImGuiCol_PopupBg]               = default_color;
    style.Colors[ImGuiCol_Border]                = default_color;
    style.Colors[ImGuiCol_BorderShadow]          = default_color;
    style.Colors[ImGuiCol_FrameBg]               = default_color;
    style.Colors[ImGuiCol_FrameBgHovered]        = default_color;
    style.Colors[ImGuiCol_FrameBgActive]         = default_color;
    style.Colors[ImGuiCol_TitleBg]               = default_color;
    style.Colors[ImGuiCol_TitleBgActive]         = default_color;
    style.Colors[ImGuiCol_TitleBgCollapsed]      = default_color;
    style.Colors[ImGuiCol_MenuBarBg]             = default_color;
    style.Colors[ImGuiCol_ScrollbarBg]           = { 0.0f, 0.0f, 0.0f, 0.125f };
    style.Colors[ImGuiCol_ScrollbarGrab]         = { 1.0f, 0.5f, 0.0f, 0.25f };
    style.Colors[ImGuiCol_ScrollbarGrabHovered]  = { 1.0f, 0.5f, 0.0f, 0.5f };
    style.Colors[ImGuiCol_ScrollbarGrabActive]   = { 1.0f, 0.5f, 0.0f, 1.0f };
    style.Colors[ImGuiCol_CheckMark]             = default_color;
    style.Colors[ImGuiCol_SliderGrab]            = default_color;
    style.Colors[ImGuiCol_SliderGrabActive]      = default_color;
    style.Colors[ImGuiCol_Button]                = { 1.0f, 0.5f, 0.0f, 0.25f };
    style.Colors[ImGuiCol_ButtonHovered]         = { 1.0f, 0.5f, 0.0f, 0.5f };
    style.Colors[ImGuiCol_ButtonActive]          = { 1.0f, 0.5f, 0.0f, 1.0f };
    style.Colors[ImGuiCol_Header]                = { 1.0f, 0.5f, 0.0f, 0.25f };
    style.Colors[ImGuiCol_HeaderHovered]         = { 1.0f, 0.5f, 0.0f, 0.5f };
    style.Colors[ImGuiCol_HeaderActive]          = { 1.0f, 0.5f, 0.0f, 1.0f };
    style.Colors[ImGuiCol_Separator]             = default_color;
    style.Colors[ImGuiCol_SeparatorHovered]      = default_color;
    style.Colors[ImGuiCol_SeparatorActive]       = default_color;
    style.Colors[ImGuiCol_ResizeGrip]            = default_color;
    style.Colors[ImGuiCol_ResizeGripHovered]     = default_color;
    style.Colors[ImGuiCol_ResizeGripActive]      = default_color;
    style.Colors[ImGuiCol_Tab]                   = default_color;
    style.Colors[ImGuiCol_TabHovered]            = default_color;
    style.Colors[ImGuiCol_TabActive]             = default_color;
    style.Colors[ImGuiCol_TabUnfocused]          = default_color;
    style.Colors[ImGuiCol_TabUnfocusedActive]    = default_color;
    style.Colors[ImGuiCol_PlotLines]             = default_color;
    style.Colors[ImGuiCol_PlotLinesHovered]      = default_color;
    style.Colors[ImGuiCol_PlotHistogram]         = default_color;
    style.Colors[ImGuiCol_PlotHistogramHovered]  = default_color;
    style.Colors[ImGuiCol_TextSelectedBg]        = default_color;
    style.Colors[ImGuiCol_DragDropTarget]        = default_color;
    style.Colors[ImGuiCol_NavHighlight]          = default_color;
    style.Colors[ImGuiCol_NavWindowingHighlight] = default_color;
    style.Colors[ImGuiCol_NavWindowingDimBg]     = default_color;
    style.Colors[ImGuiCol_ModalWindowDimBg]      = default_color;
    static_assert(ImGuiCol_COUNT == 48, "ImGui color palette has changed!");

    ImGui::SetCurrentContext(nullptr);
}

// declared in gui.hh
widget_im::~widget_im()
{
    ImGui::DestroyContext(m_im);
}

}
}
