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
#include <iostream>
#include <map>
#include <glm/glm.hpp>
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
	friend class window;

private:
	sdl_window m_wnd;
	sdl_glcontext m_glctx;
	decltype(s_windows)::iterator m_iter;
	ImGuiContext *m_im;
	ImGuiIO *m_io;
	int m_width;
	int m_height;
	bool m_textinput_active = false;
	std::function<void(int)> m_frame_proc;

	void on_event(const SDL_Event &ev);
	void on_key(SDL_Keysym keysym,bool down);
	void on_text(const char *text);
	void on_mousemove(int x,int y);
	void on_mousewheel(int y);
	void on_mousebutton(int button,bool down);
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
	case SDL_KEYDOWN:
		with_windowid(
			ev.key.windowID,
			[ev](window_impl *wnd){
				wnd->on_key(ev.key.keysym,true);
			});
		break;
	case SDL_KEYUP:
		with_windowid(
			ev.key.windowID,
			[ev](window_impl *wnd){
				wnd->on_key(ev.key.keysym,false);
			});
		break;
	case SDL_TEXTINPUT:
		with_windowid(
			ev.text.windowID,
			[ev](window_impl *wnd){
				wnd->on_text(ev.text.text);
			});
		break;
	case SDL_MOUSEMOTION:
		with_windowid(
			ev.motion.windowID,
			[ev](window_impl *wnd){
				wnd->on_mousemove(ev.motion.x,ev.motion.y);
			});
		break;
	case SDL_MOUSEWHEEL:
		with_windowid(
			ev.wheel.windowID,
			[ev](window_impl *wnd){
				wnd->on_mousewheel(ev.wheel.y);
			});
		break;
	case SDL_MOUSEBUTTONDOWN:
		with_windowid(
			ev.button.windowID,
			[ev](window_impl *wnd){
				wnd->on_mousebutton(ev.button.button,true);
			});
		break;
	case SDL_MOUSEBUTTONUP:
		with_windowid(
			ev.button.windowID,
			[ev](window_impl *wnd){
				wnd->on_mousebutton(ev.button.button,false);
			});
		break;
	case SDL_WINDOWEVENT:
		with_windowid(
			ev.window.windowID,
			[ev](window_impl *wnd){
				wnd->on_windowevent(ev.window);
			});
		break;
	}
}

void window_impl::on_key(SDL_Keysym keysym,bool down)
{
	switch (keysym.sym) {
	case SDLK_LSHIFT:
	case SDLK_RSHIFT:
		m_io->KeyShift = down;
		break;
	case SDLK_LCTRL:
	case SDLK_RCTRL:
		m_io->KeyCtrl = down;
		break;
	case SDLK_LALT:
	case SDLK_RALT:
		m_io->KeyAlt = down;
		break;
	case SDLK_LGUI:
	case SDLK_RGUI:
		m_io->KeySuper = down;
		break;
	case SDLK_TAB:
		m_io->KeysDown[ImGuiKey_Tab] = down;
		break;
	case SDLK_LEFT:
		m_io->KeysDown[ImGuiKey_LeftArrow] = down;
		break;
	case SDLK_RIGHT:
		m_io->KeysDown[ImGuiKey_RightArrow] = down;
		break;
	case SDLK_UP:
		m_io->KeysDown[ImGuiKey_UpArrow] = down;
		break;
	case SDLK_DOWN:
		m_io->KeysDown[ImGuiKey_DownArrow] = down;
		break;
	case SDLK_PAGEUP:
		m_io->KeysDown[ImGuiKey_PageUp] = down;
		break;
	case SDLK_PAGEDOWN:
		m_io->KeysDown[ImGuiKey_PageDown] = down;
		break;
	case SDLK_HOME:
		m_io->KeysDown[ImGuiKey_Home] = down;
		break;
	case SDLK_END:
		m_io->KeysDown[ImGuiKey_End] = down;
		break;
	case SDLK_DELETE:
		m_io->KeysDown[ImGuiKey_Delete] = down;
		break;
	case SDLK_BACKSPACE:
		m_io->KeysDown[ImGuiKey_Backspace] = down;
		break;
	case SDLK_RETURN:
		m_io->KeysDown[ImGuiKey_Enter] = down;
		break;
	case SDLK_ESCAPE:
		m_io->KeysDown[ImGuiKey_Escape] = down;
		break;
	case SDLK_a:
		m_io->KeysDown[ImGuiKey_A] = down;
		break;
	case SDLK_c:
		m_io->KeysDown[ImGuiKey_C] = down;
		break;
	case SDLK_v:
		m_io->KeysDown[ImGuiKey_V] = down;
		break;
	case SDLK_x:
		m_io->KeysDown[ImGuiKey_X] = down;
		break;
	case SDLK_y:
		m_io->KeysDown[ImGuiKey_Y] = down;
		break;
	case SDLK_z:
		m_io->KeysDown[ImGuiKey_Z] = down;
		break;
	}
}

void window_impl::on_text(const char *text)
{
	m_io->AddInputCharactersUTF8(text);
}

void window_impl::on_mousemove(int x,int y)
{
	m_io->MousePos.x = x;
	m_io->MousePos.y = y;
}

void window_impl::on_mousewheel(int y)
{
	m_io->MouseWheel += y;
}

void window_impl::on_mousebutton(int button,bool down)
{
	switch (button) {
	case SDL_BUTTON_LEFT:
		m_io->MouseDown[0] = down;
		break;
	case SDL_BUTTON_RIGHT:
		m_io->MouseDown[1] = down;
		break;
	case SDL_BUTTON_MIDDLE:
		m_io->MouseDown[2] = down;
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

static void render_impl(int width,int height);

void window_impl::on_frame(int delta_time)
{
	// Begin/end text input according to ImGui.
	if (m_io->WantTextInput && !m_textinput_active) {
		SDL_StartTextInput();
		m_textinput_active = true;
	} else if (!m_io->WantTextInput && m_textinput_active) {
		SDL_StopTextInput();
		m_textinput_active = false;
	}

	// Start the new frame in ImGui.
	m_io->DeltaTime = delta_time / 1000.0;

	ImGui::SetCurrentContext(m_im);
	ImGui::NewFrame();

	glViewport(0,0,m_width,m_height);

	m_frame_proc(delta_time);

	ImGui::Render();

	render_impl(m_width,m_height);
}

static void render_impl(int width,int height)
{
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
	glm::mat4 projection = glm::ortho(0,width,height,0,-1,+1);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glOrtho(0,width,height,0,-1,+1);
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
}

window_impl::~window_impl()
{
	s_windows.erase(m_iter);
	ImGui::DestroyContext(m_im);
}

template <typename MFT,MFT MF>
struct proxy;

template <typename T,typename R,typename... Args,R (T::*MF)(Args...)>
struct proxy<R (T::*)(Args...),MF> {
	static R call(Args... args,gpointer user_data)
	{
		return (static_cast<T *>(user_data)->*MF)(args...);
	}
};

gboolean window::on_render(GtkGLArea *area,GdkGLContext *context)
{
	glClearColor(1,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui::SetCurrentContext(M->m_im);
	render_impl(m_canvas_width,m_canvas_height);
	return true;
}

void window::on_resize(GtkGLArea *area,int width,int height)
{
	m_canvas_width = width;
	m_canvas_height = height;
	glViewport(0,0,width,height);
}

window::window(const std::string &title,int width,int height)
{
	M = new window_impl(
		title,
		width,
		height,
		[this](int delta_time) {
			frame(delta_time);
			gtk_gl_area_queue_render(GTK_GL_AREA(m_canvas));
		}
	);

	m_wnd = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(m_wnd),title.c_str());
	gtk_window_set_default_size(GTK_WINDOW(m_wnd),width,height);
	m_canvas = gtk_gl_area_new();
	g_signal_connect(
		m_canvas,
		"render",
		G_CALLBACK((proxy<
			decltype(&window::on_render),
			&window::on_render
		>::call)),
		this
	);
	g_signal_connect(
		m_canvas,
		"resize",
		G_CALLBACK((proxy<
			decltype(&window::on_resize),
			&window::on_resize
		>::call)),
		this
	);
	gtk_container_add(GTK_CONTAINER(m_wnd),m_canvas);
	gtk_widget_show(m_canvas);
	gtk_widget_show(m_wnd);
}

window::~window()
{
	delete M;
	gtk_widget_destroy(m_wnd);
}

int window::get_width() const
{
	return m_canvas_width;
}

int window::get_height() const
{
	return m_canvas_height;
}

void window::run_once()
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
