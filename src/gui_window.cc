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
#include "../imgui/imgui.h"
#include "gui.hh"

#define DISPLAYWIDTH  800
#define DISPLAYHEIGHT 600

namespace gui {

window::window(const std::string &title,int width,int height) :
	sys::window(title,width,height),
	m_io(ImGui::GetIO())
{
	// FIXME window_resize(DISPLAYWIDTH,DISPLAYHEIGHT);
}

void window::on_key(SDL_Keysym keysym,bool down)
{
	switch (keysym.sym) {
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

void window::on_text(const char *text)
{
	m_io.AddInputCharactersUTF8(text);
}

void window::on_mousemove(int x,int y)
{
	m_io.MousePos.x = x;
	m_io.MousePos.y = y;
}

void window::on_mousewheel(int y)
{
	m_io.MouseWheel += y;
}

void window::on_mousebutton(int button,bool down)
{
	switch (button) {
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

void window::on_resize(int width,int height)
{
	window_resize(width,height);
}

void window::on_frame(int delta_time)
{
	frame(delta_time);
}

}
