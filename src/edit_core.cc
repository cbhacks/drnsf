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
#include <SDL.h>
#include <GL/gl.h>
#include "../imgui/imgui.h"
#include "edit.hh"
#include "res.hh"
#include "gfx.hh"

namespace edit {

core::core()
{
	// Disable the ImGui settings INI.
	m_io.IniFilename = nullptr;

	// Configure the ImGui keymap.
	for (int i = 0;i < ImGuiKey_COUNT;i++) {
		m_io.KeyMap[i] = i;
	}

	// Get the ImGui font.
	unsigned char *font_pixels;
	int font_width;
	int font_height;
	m_io.Fonts->GetTexDataAsRGBA32(&font_pixels,&font_width,&font_height);

	// Upload the font as a texture to OpenGL.
	GLuint font_gltex;
	glGenTextures(1,&font_gltex);
	glBindTexture(GL_TEXTURE_2D,font_gltex);
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

	// Save the font texture's name (id) in ImGui to be used later when
	// rendering the GUI's.
	m_io.Fonts->TexID = reinterpret_cast<void*>(font_gltex);

	// Create all of the editor modules.
	for (auto &&info : module_info::get_set()) {
		m_modules[info] = info->create(*this);
	}
}

void core::frame(int delta)
{
	// Begin/end text input according to ImGui.
	if (m_io.WantTextInput && !m_wnd_textinput_active) {
		SDL_StartTextInput();
	} else if (!m_io.WantTextInput && m_wnd_textinput_active) {
		SDL_StopTextInput();
	}

	// Start the new frame in ImGui.
	m_io.DeltaTime = delta / 1000.0;
	ImGui::NewFrame();

	// Inform all of the modules if this is the first frame. FIXME doesn't work anymore
	if (m_firstframe) {
		for (auto &&kv : m_modules) {
			auto &&mod = kv.second;
			mod->firstframe();
		}
		m_firstframe = false;
	}

	// Clear the screen.
	glClearColor(0.5,0.5,0.5,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Calculate the normalized display size.
	double norm_width = 1;
	double norm_height = (double)m_wnd_height / m_wnd_width;
	if (norm_height < 1) {
		norm_width /= norm_height;
		norm_height = 1;
	}

	// Set up the 3D perspective.
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glFrustum(
		-norm_width * m_cam.lens_focus * m_cam.lens_near,
		+norm_width * m_cam.lens_focus * m_cam.lens_near,
		-norm_height * m_cam.lens_focus * m_cam.lens_near,
		+norm_height * m_cam.lens_focus * m_cam.lens_near,
		m_cam.lens_near,
		m_cam.lens_far
	);
	glTranslatef(0,0,-m_cam.lens_near);
	glTranslatef(0,0,-m_cam.lens_adjust);
	glMatrixMode(GL_MODELVIEW);

	// Enable z-buffering.
	glEnable(GL_DEPTH_TEST);

	// Run the core GUI menus.
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Quit","Ctrl+Q")) {
				SDL_Quit();
				std::exit(EXIT_SUCCESS);
			}
			for (auto &&kv : m_modules) {
				auto &&mod = kv.second;
				mod->show_file_menu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			for (auto &&kv : m_modules) {
				auto &&mod = kv.second;
				mod->show_edit_menu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View")) {
			for (auto &&kv : m_modules) {
				auto &&mod = kv.second;
				for (auto &&pn : mod->get_panels()) {
					ImGui::PushID(pn);
					ImGui::MenuItem(
						pn->get_title().c_str(),
						nullptr,
						&pn->visible
					);
					ImGui::PopID();
				}
				mod->show_view_menu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools")) {
			for (auto &&kv : m_modules) {
				auto &&mod = kv.second;
				mod->show_tools_menu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	// Show all of the visible editor panels.
	for (auto &&kv : m_modules) {
		auto &&mod = kv.second;
		for (auto &&pn : mod->get_panels()) {
			if (!pn->visible)
				continue;

			ImGui::PushID(pn);

			if (!ImGui::Begin(pn->get_title().c_str(),&pn->visible)) { //FIXME 80 col
				ImGui::End();
				ImGui::PopID();
				continue;
			}

			pn->show();

			ImGui::End();
			ImGui::PopID();
		}
	}

	// Run all of the enabled editor modules.
	for (auto &&kv : m_modules) {
		auto &&mod = kv.second;
		mod->frame(delta);
	}

	// Disable the previously-enabled z-buffering.
	glDisable(GL_DEPTH_TEST);

	// Restore the default projection matrix.
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	// Render the ImGui overlay.
	ImGui::Render();
	render(ImGui::GetDrawData());
}

void core::render(ImDrawData *draw_data)
{
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
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glOrtho(0,m_wnd_width,m_wnd_height,0,-1,+1);
	glMatrixMode(GL_MODELVIEW);

	// Enable the relevant vertex arrays.
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	// Enable alpha blending. ImGui requires this for its fonts.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	// Enable texture-mapping. ImGui also uses this for fonts.
	glEnable(GL_TEXTURE_2D);

	// Draw each of the command lists. Each list contains a set of draw
	// commands associated with a particular set of vertex arrays.
	for (int i = 0;i < draw_data->CmdListsCount;i++) {
		ImDrawList *draw_list = draw_data->CmdLists[i];

		// Prepare the vertex arrays for this list.
		glVertexPointer(
			2,
			GL_FLOAT,
			sizeof(ImDrawVert),
			&draw_list->VtxBuffer.Data[0].pos
		);
		glTexCoordPointer(
			2,
			GL_FLOAT,
			sizeof(ImDrawVert),
			&draw_list->VtxBuffer.Data[0].uv
		);
		glColorPointer(
			4,
			GL_UNSIGNED_BYTE,
			sizeof(ImDrawVert),
			&draw_list->VtxBuffer.Data[0].col
		);

		// Get the index array (IBO-like). Each draw command pulls some
		// amount of these elements out from the front.
		ImDrawIdx *index_array = draw_list->IdxBuffer.Data;

		// Draw each of the commands in the list.
		for (auto &c : draw_list->CmdBuffer) {
			// Use this command's custom drawing implementation,
			// if there is one. Otherwise, draw it as you would
			// normally.
			if (c.UserCallback) {
				c.UserCallback(draw_list,&c);
			} else {
				glBindTexture(
					GL_TEXTURE_2D,
					reinterpret_cast<std::uintptr_t>(
						c.TextureId
					)
				);
				glDrawElements(
					GL_TRIANGLES,
					c.ElemCount,
					sizeof(ImDrawIdx) == 2 ?
						GL_UNSIGNED_SHORT :
						GL_UNSIGNED_INT,
					index_array
				);
			}

			// Move past the indices which have already been used
			// for drawing.
			index_array += c.ElemCount;
		}
	}

	// Unbind whatever texture was bound by the last draw command.
	glBindTexture(GL_TEXTURE_2D,0);

	// Re-disable texture mapping.
	glDisable(GL_TEXTURE_2D);

	// Restore the previous blending setup.
	glDisable(GL_BLEND);
	glBlendFunc(GL_ONE,GL_ZERO);

	// Un-enable the vertex arrays.
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	// Restore the previous (identity) projection.
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void core::key(int key,bool down)
{
	switch (key) {
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
		m_io.KeyShift = down;
		break;
	case SDLK_LCTRL:
	case SDLK_RCTRL:
		m_io.KeyCtrl = down;
		break;
	case SDLK_LALT:
	case SDLK_RALT:
		m_io.KeyAlt = down;
		break;
	case SDLK_LGUI:
	case SDLK_RGUI:
		m_io.KeySuper = down;
		break;
	case SDLK_TAB:
		m_io.KeysDown[ImGuiKey_Tab] = down;
		break;
	case SDLK_LEFT:
		m_io.KeysDown[ImGuiKey_LeftArrow] = down;
		break;
	case SDLK_RIGHT:
		m_io.KeysDown[ImGuiKey_RightArrow] = down;
		break;
	case SDLK_UP:
		m_io.KeysDown[ImGuiKey_UpArrow] = down;
		break;
	case SDLK_DOWN:
		m_io.KeysDown[ImGuiKey_DownArrow] = down;
		break;
	case SDLK_PAGEUP:
		m_io.KeysDown[ImGuiKey_PageUp] = down;
		break;
	case SDLK_PAGEDOWN:
		m_io.KeysDown[ImGuiKey_PageDown] = down;
		break;
	case SDLK_HOME:
		m_io.KeysDown[ImGuiKey_Home] = down;
		break;
	case SDLK_END:
		m_io.KeysDown[ImGuiKey_End] = down;
		break;
	case SDLK_DELETE:
		m_io.KeysDown[ImGuiKey_Delete] = down;
		break;
	case SDLK_BACKSPACE:
		m_io.KeysDown[ImGuiKey_Backspace] = down;
		break;
	case SDLK_RETURN:
		m_io.KeysDown[ImGuiKey_Enter] = down;
		break;
	case SDLK_ESCAPE:
		m_io.KeysDown[ImGuiKey_Escape] = down;
		break;
	case SDLK_a:
		m_io.KeysDown[ImGuiKey_A] = down;
		break;
	case SDLK_c:
		m_io.KeysDown[ImGuiKey_C] = down;
		break;
	case SDLK_v:
		m_io.KeysDown[ImGuiKey_V] = down;
		break;
	case SDLK_x:
		m_io.KeysDown[ImGuiKey_X] = down;
		break;
	case SDLK_y:
		m_io.KeysDown[ImGuiKey_Y] = down;
		break;
	case SDLK_z:
		m_io.KeysDown[ImGuiKey_Z] = down;
		break;
	}
}

void core::mouse_button(int btn,bool down)
{
	// Update the mouse button status in ImGui.
	switch (btn) {
	case SDL_BUTTON_LEFT:
		m_io.MouseDown[0] = down;
		break;
	case SDL_BUTTON_RIGHT:
		m_io.MouseDown[1] = down;
		break;
	case SDL_BUTTON_MIDDLE:
		m_io.MouseDown[2] = down;
		break;
	}
}

void core::window_resize(int width,int height)
{
	// Save the new window size.
	m_wnd_width = width;
	m_wnd_height = height;

	// Update the window size in ImGui.
	m_io.DisplaySize.x = width;
	m_io.DisplaySize.y = height;

	// Set the GL viewport to match the new window size.
	glViewport(0,0,m_wnd_width,m_wnd_height);
}

const decltype(core::m_modules) &core::get_modules() const
{
	return m_modules;
}

}
