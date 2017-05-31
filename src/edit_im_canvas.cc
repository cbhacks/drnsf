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
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../imgui/imgui.h"
#include "edit.hh"

// FIXME FIXME this file will be obsolete; as such there are no real comments

namespace drnsf {
namespace edit {

static gl::program s_prog;
static gl::texture s_font_tex;
static gl::vert_array s_vao;
static gl::buffer s_ibo;
static gl::buffer s_vbo;

static void init()
{
    static bool s_is_init = false;
    if (s_is_init) return;
    s_is_init = true;

    // Get the ImGui font.
    unsigned char *font_pixels;
    int font_width;
    int font_height;
    ImGui::GetIO().Fonts->GetTexDataAsRGBA32(
        &font_pixels,
        &font_width,
        &font_height
    );

    // Upload the font as a texture to OpenGL.
    glBindTexture(GL_TEXTURE_2D,s_font_tex);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        font_width,
        font_height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        font_pixels
    );
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D,0);

    const char vert_code[] = R"(

#version 150 core

uniform mat4 u_ScreenOrtho;

in vec4 a_Position;
in vec2 a_TexCoord;
in vec4 a_Color;

out vec2 v_TexCoord;
out vec4 v_Color;

void main()
{
    gl_Position = u_ScreenOrtho * a_Position;
    v_TexCoord = a_TexCoord;
    v_Color = a_Color;
}

)";

    gl::vert_shader vert_shader;
    vert_shader.compile(vert_code);

    const char frag_code[] = R"(

#version 150 core

uniform sampler2D u_Font;

in vec2 v_TexCoord;
in vec4 v_Color;

void main()
{
    gl_FragColor = v_Color * texture2D(u_Font,v_TexCoord);
}

)";

    gl::frag_shader frag_shader;
    frag_shader.compile(frag_code);

    glAttachShader(s_prog,vert_shader);
    glAttachShader(s_prog,frag_shader);
    glBindAttribLocation(s_prog,0,"a_Position");
    glBindAttribLocation(s_prog,1,"a_TexCoord");
    glBindAttribLocation(s_prog,2,"a_Color");
    glLinkProgram(s_prog);

    glBindVertexArray(s_vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,s_ibo);
    glBindBuffer(GL_ARRAY_BUFFER,s_vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        false,
        sizeof(ImDrawVert),
        reinterpret_cast<void *>(offsetof(ImDrawVert,pos))
    );
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        false,
        sizeof(ImDrawVert),
        reinterpret_cast<void *>(offsetof(ImDrawVert,uv))
    );
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,
        4,
        GL_UNSIGNED_BYTE,
        true,
        sizeof(ImDrawVert),
        reinterpret_cast<void *>(offsetof(ImDrawVert,col))
    );
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindVertexArray(0);
}

void im_canvas::render(int width,int height)
{
    m_io->DisplaySize.x = width;
    m_io->DisplaySize.y = height;
    glViewport(0,0,width,height);

    long current_time = g_get_monotonic_time();
    long delta_time = current_time - m_last_update;
    m_last_update = current_time;
    m_io->DeltaTime = delta_time / 1000000.0;

    auto previous_im = ImGui::GetCurrentContext();
    ImGui::SetCurrentContext(m_im);
    ImGui::NewFrame();

    on_frame(width,height,delta_time / 1000);

    ImGui::Render();
    ImDrawData *draw_data = ImGui::GetDrawData();

    glUseProgram(s_prog);
    int uni_screenortho = glGetUniformLocation(s_prog,"u_ScreenOrtho");
    glUniform1i(glGetUniformLocation(s_prog,"u_Font"),0);

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
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    // Enable the GL scissor test. This is used to clip gui widgets to only
    // render within their given area.
    glEnable(GL_SCISSOR_TEST);

    // Draw each of the command lists. Each list contains a set of draw
    // commands associated with a particular set of vertex arrays.
    for (int i = 0;i < draw_data->CmdListsCount;i++) {
        ImDrawList *draw_list = draw_data->CmdLists[i];

        glBindBuffer(GL_COPY_WRITE_BUFFER,s_vbo);
        glBufferData(
            GL_COPY_WRITE_BUFFER,
            draw_list->VtxBuffer.Size * sizeof(ImDrawVert),
            draw_list->VtxBuffer.Data,
            GL_DYNAMIC_DRAW
        );
        glBindBuffer(GL_COPY_WRITE_BUFFER,s_ibo);
        glBufferData(
            GL_COPY_WRITE_BUFFER,
            draw_list->IdxBuffer.Size * sizeof(ImDrawIdx),
            draw_list->IdxBuffer.Data,
            GL_DYNAMIC_DRAW
        );
        glBindBuffer(GL_COPY_WRITE_BUFFER,0);

        // Get the index array (IBO-like). Each draw command pulls some
        // amount of these elements out from the front.
        ImDrawIdx *index_array = 0;

        // Draw each of the commands in the list.
        for (auto &c : draw_list->CmdBuffer) {
            glBindTexture(GL_TEXTURE_2D,s_font_tex);
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
                index_array
            );

            // Move past the indices which have already been used
            // for drawing.
            index_array += c.ElemCount;
        }
    }

    // Re-disable scissor test.
    glDisable(GL_SCISSOR_TEST);

    // Unbind whatever texture was bound by the last draw command.
    glBindTexture(GL_TEXTURE_2D,0);

    // Restore the previous blending setup.
    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_ZERO);

    glBindVertexArray(0);
    glUseProgram(0);

    ImGui::SetCurrentContext(previous_im);
}

im_canvas::im_canvas(gui::container &parent,gui::layout layout) :
    m_canvas(parent,layout)
{
    init();

    m_timer = g_timeout_add(
        10,
        [](gpointer user_data) -> gboolean {
            auto self = static_cast<im_canvas *>(user_data);
            self->m_canvas.invalidate();
            return G_SOURCE_CONTINUE;
        },
        this
    );

    m_im = ImGui::CreateContext();
    auto previous_im = ImGui::GetCurrentContext();
    ImGui::SetCurrentContext(m_im);
    m_io = &ImGui::GetIO();

    m_io->IniFilename = "imgui.ini";

    // Configure the ImGui keymap.
    for (int i = 0;i < ImGuiKey_COUNT;i++) {
        m_io->KeyMap[i] = i;
    }

    m_last_update = g_get_monotonic_time();

    h_render <<= [this](int width,int height) {
        render(width,height);
    };
    h_render.bind(m_canvas.on_render);

    h_mousemove <<= [this](int x,int y) {
        m_io->MousePos.x = x;
        m_io->MousePos.y = y;
    };
    h_mousemove.bind(m_canvas.on_mousemove);

    h_mousewheel <<= [this](int delta_y) {
        m_io->MouseWheel += delta_y;
    };
    h_mousewheel.bind(m_canvas.on_mousewheel);

    h_mousebutton <<= [this](int button,bool down) {
        switch (button) {
        case 1:
            m_io->MouseDown[0] = down;
            break;
        case 2:
            m_io->MouseDown[2] = down;
            break;
        case 3:
            m_io->MouseDown[1] = down;
            break;
        }
    };
    h_mousebutton.bind(m_canvas.on_mousebutton);

    h_key <<= [this](int key,bool down) {
        switch (key) {
        case GDK_KEY_Shift_L:
        case GDK_KEY_Shift_R:
            m_io->KeyShift = down;
            break;
        case GDK_KEY_Control_L:
        case GDK_KEY_Control_R:
            m_io->KeyCtrl = down;
            break;
        case GDK_KEY_Alt_L:
        case GDK_KEY_Alt_R:
            m_io->KeyAlt = down;
            break;
        case GDK_KEY_Meta_L:
        case GDK_KEY_Meta_R:
            m_io->KeySuper = down;
            break;
        case GDK_KEY_Tab:
            m_io->KeysDown[ImGuiKey_Tab] = down;
            break;
        case GDK_KEY_Left:
            m_io->KeysDown[ImGuiKey_LeftArrow] = down;
            break;
        case GDK_KEY_Right:
            m_io->KeysDown[ImGuiKey_RightArrow] = down;
            break;
        case GDK_KEY_Up:
            m_io->KeysDown[ImGuiKey_UpArrow] = down;
            break;
        case GDK_KEY_Down:
            m_io->KeysDown[ImGuiKey_DownArrow] = down;
            break;
        case GDK_KEY_Page_Up:
            m_io->KeysDown[ImGuiKey_PageUp] = down;
            break;
        case GDK_KEY_Page_Down:
            m_io->KeysDown[ImGuiKey_PageDown] = down;
            break;
        case GDK_KEY_Home:
            m_io->KeysDown[ImGuiKey_Home] = down;
            break;
        case GDK_KEY_End:
            m_io->KeysDown[ImGuiKey_End] = down;
            break;
        case GDK_KEY_Delete:
            m_io->KeysDown[ImGuiKey_Delete] = down;
            break;
        case GDK_KEY_BackSpace:
            m_io->KeysDown[ImGuiKey_Backspace] = down;
            break;
        case GDK_KEY_Return:
            m_io->KeysDown[ImGuiKey_Enter] = down;
            break;
        case GDK_KEY_Escape:
            m_io->KeysDown[ImGuiKey_Escape] = down;
            break;
        case GDK_KEY_a:
            m_io->KeysDown[ImGuiKey_A] = down;
            break;
        case GDK_KEY_c:
            m_io->KeysDown[ImGuiKey_C] = down;
            break;
        case GDK_KEY_v:
            m_io->KeysDown[ImGuiKey_V] = down;
            break;
        case GDK_KEY_x:
            m_io->KeysDown[ImGuiKey_X] = down;
            break;
        case GDK_KEY_y:
            m_io->KeysDown[ImGuiKey_Y] = down;
            break;
        case GDK_KEY_z:
            m_io->KeysDown[ImGuiKey_Z] = down;
            break;
        }
    };
    h_key.bind(m_canvas.on_key);

    h_text <<= [this](const char *text) {
        m_io->AddInputCharactersUTF8(text);
    };
    h_text.bind(m_canvas.on_text);

    ImGui::SetCurrentContext(previous_im);
}

im_canvas::~im_canvas()
{
    g_source_remove(m_timer);

    auto previous_im = ImGui::GetCurrentContext();
    ImGui::SetCurrentContext(m_im);
    ImGui::DestroyContext(m_im);
    ImGui::SetCurrentContext(previous_im);
}

void im_canvas::show()
{
    m_canvas.show();
}

}
}
