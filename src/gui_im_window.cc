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
#include <epoxy/gl.h>
#include <iostream>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../imgui/imgui.h"
#include "gui.hh"

#define DISPLAYWIDTH  800
#define DISPLAYHEIGHT 600

namespace drnsf {

namespace {

class sdl_window : private util::nocopy {
private:
	SDL_Window *m_wnd;

public:
	explicit sdl_window(const std::string &title,int width,int height)
	{
		m_wnd = SDL_CreateWindow(
			title.c_str(),
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			width,
			height,
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
		);
		if (!m_wnd) {
			std::cerr <<
				"Error creating window: " <<
				SDL_GetError() <<
				std::endl;
			throw 0; // FIXME
		}
	}

	~sdl_window()
	{
		SDL_DestroyWindow(m_wnd);
	}

	operator SDL_Window *()
	{
		return m_wnd;
	}
};

class sdl_glcontext : private util::nocopy {
private:
	SDL_GLContext m_glctx;

public:
	explicit sdl_glcontext(sdl_window &wnd)
	{
		m_glctx = SDL_GL_CreateContext(wnd);
		if (!m_glctx) {
			std::cerr <<
				"Error creating OpenGL context: " <<
				SDL_GetError() <<
				std::endl;
			throw 0; // FIXME
		}

		// Enable v-sync.
		if (SDL_GL_SetSwapInterval(1) != 0) {
			std::cerr <<
				"Error enabling v-sync: " <<
				SDL_GetError() <<
				std::endl;
		}
	}

	~sdl_glcontext()
	{
		SDL_GL_DeleteContext(m_glctx);
	}

	operator SDL_GLContext()
	{
		return m_glctx;
	}
};

}

namespace gui {

static std::map<Uint32,window_impl*> s_windows;

template <typename T>
static void with_windowid(Uint32 id,const T &f)
{
	auto it = s_windows.find(id);

	if (it != s_windows.end()) {
		f(it->second);
	}
}

class window_impl : private util::nocopy {
	friend class im_window;

private:
	sdl_window m_wnd;
	sdl_glcontext m_glctx;
	decltype(s_windows)::iterator m_iter;
	ImGuiContext *m_im;
	ImGuiIO *m_io;
	int m_width;
	int m_height;
	std::function<void(int)> m_frame_proc;

	void on_event(const SDL_Event &ev);
	void on_windowevent(const SDL_WindowEvent &ev);
	void on_resize(int width,int height);
	void on_frame(int delta_time);

public:
	window_impl(
		const std::string &title,
		int width,
		int height,
		decltype(m_frame_proc) frame_proc
	);
	~window_impl();
};

void window_impl::on_event(const SDL_Event &ev)
{
	switch (ev.type) {
	case SDL_WINDOWEVENT:
		with_windowid(
			ev.window.windowID,
			[ev](window_impl *wnd){
				wnd->on_windowevent(ev.window);
			});
		break;
	}
}

void window_impl::on_windowevent(const SDL_WindowEvent &ev)
{
	switch (ev.event) {
	case SDL_WINDOWEVENT_SIZE_CHANGED:
		on_resize(ev.data1,ev.data2);
		break;
	}
}

void window_impl::on_resize(int width,int height)
{
	m_width = width;
	m_height = height;

	m_io->DisplaySize.x = width;
	m_io->DisplaySize.y = height;
}

void window_impl::on_frame(int delta_time)
{
	// Start the new frame in ImGui.
	m_io->DeltaTime = delta_time / 1000.0;

	ImGui::SetCurrentContext(m_im);
	ImGui::NewFrame();

	glViewport(0,0,m_width,m_height);

	m_frame_proc(delta_time);

	ImGui::Render();
	ImDrawData *draw_data = ImGui::GetDrawData();

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
	glOrtho(0,m_width,m_height,0,-1,+1);
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

	// Enable the GL scissor test. This is used to clip gui widgets to only
	// render within their given area.
	glEnable(GL_SCISSOR_TEST);

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
				glScissor(
					c.ClipRect.x,
					m_height - c.ClipRect.w,
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
			}

			// Move past the indices which have already been used
			// for drawing.
			index_array += c.ElemCount;
		}
	}

	// Re-disable scissor test.
	glDisable(GL_SCISSOR_TEST);

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

window_impl::window_impl(
	const std::string &title,
	int width,
	int height,
	decltype(m_frame_proc) frame_proc
) :
	m_wnd(title,width,height),
	m_glctx(m_wnd),
	m_width(width),
	m_height(height),
	m_frame_proc(frame_proc)
{
	auto window_id = SDL_GetWindowID(m_wnd);
	auto insert_result = s_windows.insert({window_id,this});
	if (!insert_result.second) {
		std::cerr <<
			"Error with newly created window; window ID already in"
			" use!" <<
			std::endl;
		throw 0; // FIXME
	}
	m_iter = insert_result.first;

	m_im = ImGui::CreateContext();
	ImGui::SetCurrentContext(m_im);
	m_io = &ImGui::GetIO();

	m_io->IniFilename = "imgui.ini";

	m_io->DisplaySize.x = width;
	m_io->DisplaySize.y = height;

	// Configure the ImGui keymap.
	for (int i = 0;i < ImGuiKey_COUNT;i++) {
		m_io->KeyMap[i] = i;
	}

	// Get the ImGui font.
	unsigned char *font_pixels;
	int font_width;
	int font_height;
	m_io->Fonts->GetTexDataAsRGBA32(&font_pixels,&font_width,&font_height);

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
	m_io->Fonts->TexID = reinterpret_cast<void*>(font_gltex);

	ImGui::NewFrame();
	ImGui::Render();
}

window_impl::~window_impl()
{
	s_windows.erase(m_iter);
	ImGui::DestroyContext(m_im);
}

void im_window::render()
{
	glClearColor(1,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui::SetCurrentContext(M->m_im);
	ImDrawData *draw_data = ImGui::GetDrawData();

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
	glOrtho(0,m_canvas_width,m_canvas_height,0,-1,+1);
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

	// Enable the GL scissor test. This is used to clip gui widgets to only
	// render within their given area.
	glEnable(GL_SCISSOR_TEST);

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
				glBindTexture(GL_TEXTURE_2D,m_canvas_font);
				glScissor(
					c.ClipRect.x,
					m_canvas_height - c.ClipRect.w,
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
			}

			// Move past the indices which have already been used
			// for drawing.
			index_array += c.ElemCount;
		}
	}

	// Re-disable scissor test.
	glDisable(GL_SCISSOR_TEST);

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

im_window::im_window(const std::string &title,int width,int height) :
	m_wnd(title,width,height),
	m_canvas(m_wnd)
{
	M = new window_impl(
		title,
		width,
		height,
		[this](int delta_time) {
			on_frame(delta_time);
			gtk_gl_area_queue_render(GTK_GL_AREA(m_canvas.M));
		}
	);

	h_init <<= [this]() {
		// Get the ImGui font.
		unsigned char *font_pixels;
		int font_width;
		int font_height;
		M->m_io->Fonts->GetTexDataAsRGBA32(
			&font_pixels,
			&font_width,
			&font_height
		);

		// Upload the font as a texture to OpenGL.
		glGenTextures(1,&m_canvas_font);
		glBindTexture(GL_TEXTURE_2D,m_canvas_font);
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
	};
	h_init.bind(m_canvas.on_init);

	h_cleanup <<= [this]() {
		glDeleteTextures(1,&m_canvas_font);
	};
	h_cleanup.bind(m_canvas.on_cleanup);

	h_render <<= [this]() {
		render();
	};
	h_render.bind(m_canvas.on_render);

	h_resize <<= [this](int width,int height) {
		m_canvas_width = width;
		m_canvas_height = height;
		glViewport(0,0,width,height);
	};
	h_resize.bind(m_canvas.on_resize);

	h_mousemove <<= [this](int x,int y) {
		M->m_io->MousePos.x = x;
		M->m_io->MousePos.y = y;
	};
	h_mousemove.bind(m_canvas.on_mousemove);

	h_mousewheel <<= [this](int delta_y) {
		M->m_io->MouseWheel += delta_y;
	};
	h_mousewheel.bind(m_canvas.on_mousewheel);

	h_mousebutton <<= [this](int button,bool down) {
		switch (button) {
		case 1:
			M->m_io->MouseDown[0] = down;
			break;
		case 2:
			M->m_io->MouseDown[2] = down;
			break;
		case 3:
			M->m_io->MouseDown[1] = down;
			break;
		}
	};
	h_mousebutton.bind(m_canvas.on_mousebutton);

	h_key <<= [this](int key,bool down) {
		switch (key) {
		case GDK_KEY_Shift_L:
		case GDK_KEY_Shift_R:
			M->m_io->KeyShift = down;
			break;
		case GDK_KEY_Control_L:
		case GDK_KEY_Control_R:
			M->m_io->KeyCtrl = down;
			break;
		case GDK_KEY_Alt_L:
		case GDK_KEY_Alt_R:
			M->m_io->KeyAlt = down;
			break;
		case GDK_KEY_Meta_L:
		case GDK_KEY_Meta_R:
			M->m_io->KeySuper = down;
			break;
		case GDK_KEY_Tab:
			M->m_io->KeysDown[ImGuiKey_Tab] = down;
			break;
		case GDK_KEY_Left:
			M->m_io->KeysDown[ImGuiKey_LeftArrow] = down;
			break;
		case GDK_KEY_Right:
			M->m_io->KeysDown[ImGuiKey_RightArrow] = down;
			break;
		case GDK_KEY_Up:
			M->m_io->KeysDown[ImGuiKey_UpArrow] = down;
			break;
		case GDK_KEY_Down:
			M->m_io->KeysDown[ImGuiKey_DownArrow] = down;
			break;
		case GDK_KEY_Page_Up:
			M->m_io->KeysDown[ImGuiKey_PageUp] = down;
			break;
		case GDK_KEY_Page_Down:
			M->m_io->KeysDown[ImGuiKey_PageDown] = down;
			break;
		case GDK_KEY_Home:
			M->m_io->KeysDown[ImGuiKey_Home] = down;
			break;
		case GDK_KEY_End:
			M->m_io->KeysDown[ImGuiKey_End] = down;
			break;
		case GDK_KEY_Delete:
			M->m_io->KeysDown[ImGuiKey_Delete] = down;
			break;
		case GDK_KEY_BackSpace:
			M->m_io->KeysDown[ImGuiKey_Backspace] = down;
			break;
		case GDK_KEY_Return:
			M->m_io->KeysDown[ImGuiKey_Enter] = down;
			break;
		case GDK_KEY_Escape:
			M->m_io->KeysDown[ImGuiKey_Escape] = down;
			break;
		case GDK_KEY_a:
			M->m_io->KeysDown[ImGuiKey_A] = down;
			break;
		case GDK_KEY_c:
			M->m_io->KeysDown[ImGuiKey_C] = down;
			break;
		case GDK_KEY_v:
			M->m_io->KeysDown[ImGuiKey_V] = down;
			break;
		case GDK_KEY_x:
			M->m_io->KeysDown[ImGuiKey_X] = down;
			break;
		case GDK_KEY_y:
			M->m_io->KeysDown[ImGuiKey_Y] = down;
			break;
		case GDK_KEY_z:
			M->m_io->KeysDown[ImGuiKey_Z] = down;
			break;
		}
	};
	h_key.bind(m_canvas.on_key);

	h_text <<= [this](const char *text) {
		M->m_io->AddInputCharactersUTF8(text);
	};
	h_text.bind(m_canvas.on_text);

	m_canvas.show();
	m_wnd.show();
}

im_window::~im_window()
{
	delete M;
}

int im_window::get_width() const
{
	return m_canvas_width;
}

int im_window::get_height() const
{
	return m_canvas_height;
}

void im_window::run_once()
{
	// Handle all of the pending events.
	SDL_Event ev;
	while (SDL_PollEvent(&ev)) {
		M->on_event(ev);
	}

	// Calculate the time passed since the last frame.
	static Uint32 last_update = 0;
	Uint32 current_update = SDL_GetTicks();
	Uint32 delta_time = current_update - last_update;
	last_update = current_update;

	// Update each window.
	for (auto &&kv : s_windows) {
		auto &&wnd = kv.second;
		SDL_GL_MakeCurrent(wnd->m_wnd,wnd->m_glctx);
		wnd->on_frame(delta_time);
		SDL_GL_SwapWindow(wnd->m_wnd);
	}
}

}
}
