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
#define DRNSF_FRONTEND_IMPLEMENTATION
#include "gl.hh"
#include "gui.hh"

namespace drnsf {
namespace gl {

// declared in gl.hh
HWND g_hwnd;

// declared in gl.hh
HDC g_hdc;

// declared in gl.hh
HGLRC g_hglrc;

// declared in gl.hh
PIXELFORMATDESCRIPTOR g_pfd;

// declared in gl.hh
int g_pfid;

namespace impl {

// declared in gl.cc
void init()
{
    static std::once_flag wndclass_flag;
    std::call_once(wndclass_flag, [] {
        WNDCLASSW wndclass{};
        wndclass.lpfnWndProc = DefWindowProcW;
        wndclass.hInstance = GetModuleHandleW(nullptr);
        wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wndclass.hbrBackground = HBRUSH(COLOR_3DFACE + 1);
        wndclass.lpszClassName = L"DRNSF_GL";
        if (!RegisterClassW(&wndclass)) {
            throw error("gl::init: failed to register window class");
        }
    });

    g_hwnd = CreateWindowW(
        L"DRNSF_GL",
        nullptr,
        0,
        0, 0,
        0, 0,
        nullptr,
        nullptr,
        GetModuleHandleW(nullptr),
        nullptr
    );
    if (!g_hwnd) {
        throw error("gl::init: failed to create window");
    }
    // FIXME - close window on error

    g_hdc = GetDC(g_hwnd);
    if (!g_hdc) {
        throw error("gl::init: failed to get window hdc");
    }
    // FIXME - release dc on error

    // Find an appropriate pixel format for use with OpenGL.
    g_pfd = {};
    g_pfd.nSize = sizeof(g_pfd);
    g_pfd.nVersion = 1;
    g_pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    g_pfd.iPixelType = PFD_TYPE_RGBA;
    g_pfd.iLayerType = PFD_MAIN_PLANE;
    g_pfid = ChoosePixelFormat(g_hdc, &g_pfd);
    if (!g_pfid) {
        throw error("gl::init: cannot find compatible pixel format");
    }

    // Set the window to use the discovered pixel format.
    if (!SetPixelFormat(g_hdc, g_pfid, &g_pfd)) {
        throw error("gl::init: could not set pixel format");
    }

    // Create the initial GL context. We will use this to create the actual GL
    // context (if possible).
    g_hglrc = wglCreateContext(g_hdc);
    if (!g_hglrc) {
        throw error("gl::init: could not create OpenGL context");
    }
    // FIXME - destroy context on error

    // Activate the context.
    if (!wglMakeCurrent(g_hdc, g_hglrc)) {
        throw error("gl::init: could not use OpenGL context");
    }

    // Get the function for creating newer GL contexts, if available.
    auto wglCreateContextAttribs =
        reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress(
            "wglCreateContextAttribsARB"
        ));
    if (!wglCreateContextAttribs) {
        throw error("gl::init: could not find wglCreateContextAttribsARB");
    }

    int core_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 2,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        0
    };
    auto core_ctx = wglCreateContextAttribs(g_hdc, false, core_attribs);
    if (core_ctx) {
        // Switch to the core context.
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(g_hglrc);
        g_hglrc = core_ctx;
        if (!wglMakeCurrent(g_hdc, g_hglrc)) {
            throw error("gl::init: failed to activate core context");
        }

        if (epoxy_gl_version() < 32) {
            throw error("gl::init: core context version less than 3.2");
        }

        return;
    }

    int fwd_attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 1,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        0
    };
    auto fwd_ctx = wglCreateContextAttribs(g_hdc, false, fwd_attribs);
    if (fwd_ctx) {
        // Switch to the forward context.
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(g_hglrc);
        g_hglrc = fwd_ctx;
        if (!wglMakeCurrent(g_hdc, g_hglrc)) {
            throw error("gl::init: failed to activate forward context");
        }

        if (epoxy_gl_version() < 31) {
            throw error("gl::init: forward context version less than 3.1");
        }

        return;
    }

    throw error(
        "gl::init: failed to create a 3.1 forward or 3.2 core context"
    );
}

// declared in gl.cc
void shutdown() noexcept
{
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(g_hglrc);
    ReleaseDC(g_hwnd, g_hdc);
    DestroyWindow(g_hwnd);
}

}

}
}
