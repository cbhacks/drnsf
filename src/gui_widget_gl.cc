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
#include "gui.hh"
#include "gl.hh"

namespace drnsf {
namespace gui {

#if USE_X11
// declared in gui.hh
void widget_gl::on_draw()
{
    int width, height;
    get_real_size(width, height);

    gl::renderbuffer rbo;
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glViewport(0, 0, width, height);
    glScissor(0, 0, width, height);

    draw_gl(width, height, rbo);

    gl::framebuffer fbo;
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glFramebufferRenderbuffer(
        GL_READ_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_RENDERBUFFER,
        rbo
    );
    glXMakeCurrent(g_display, m_handle, gl::g_ctx);
    glBlitFramebuffer(
        0, 0,
        width, height,
        0, 0,
        width, height,
        GL_COLOR_BUFFER_BIT,
        GL_NEAREST
    );
    glXSwapBuffers(g_display, m_handle);
    glXMakeCurrent(g_display, gl::g_wnd, gl::g_ctx);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}
#endif

// declared in gui.hh
widget_gl::widget_gl(container &parent, layout layout) :
    widget(parent)
{
#if USE_X11
    Colormap cmap = XCreateColormap(
        g_display,
        DefaultRootWindow(g_display),
        gl::g_vi->visual,
        AllocNone
    );

    XSetWindowAttributes attr{};
    attr.event_mask =
        ButtonPressMask |
        ButtonReleaseMask |
        KeyPressMask |
        KeyReleaseMask |
        EnterWindowMask |
        LeaveWindowMask |
        PointerMotionMask |
        ExposureMask |
        StructureNotifyMask;
    attr.colormap = cmap;
    m_handle = XCreateWindow(
        g_display,
        parent.get_container_handle(),
        0, 0,
        1, 1,
        0,
        gl::g_vi->depth,
        InputOutput,
        gl::g_vi->visual,
        CWEventMask | CWColormap,
        &attr
    );
    // m_handle is released by the base class destructor on exception.

    XSaveContext(g_display, m_handle, g_ctx_ptr, XPointer(this));
#elif USE_WINAPI
    static auto wndproc = [](
        HWND hwnd,
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam) -> LRESULT {
        auto wdg = reinterpret_cast<widget_gl *>(
            GetWindowLongPtrW(hwnd, GWLP_USERDATA)
        );
        if (uMsg == WM_CREATE) {
            auto create = reinterpret_cast<CREATESTRUCT *>(lParam);
            SetWindowLongPtr(
                hwnd,
                GWLP_USERDATA,
                reinterpret_cast<LONG_PTR>(create->lpCreateParams)
            );
            return 0;
        }
        if (uMsg == WM_SIZE) {
            wdg->m_real_width = LOWORD(lParam);
            wdg->m_real_height = HIWORD(lParam);
            wdg->on_resize(wdg->m_real_width, wdg->m_real_height);
            return 0;
        }
        if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP) {
            if (uMsg == WM_LBUTTONDOWN)
                SetFocus(hwnd);
            wdg->mousebutton(mousebtn::left, uMsg == WM_LBUTTONDOWN);
            return 0;
        }
        if (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP) {
            if (uMsg == WM_RBUTTONDOWN)
                SetFocus(hwnd);
            wdg->mousebutton(mousebtn::right, uMsg == WM_RBUTTONDOWN);
            return 0;
        }
        if (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP) {
            if (uMsg == WM_MBUTTONDOWN)
                SetFocus(hwnd);
            wdg->mousebutton(mousebtn::middle, uMsg == WM_MBUTTONDOWN);
            return 0;
        }
        if (uMsg == WM_MOUSEMOVE) {
            if (s_hover_widget != wdg) {
                if (s_hover_widget) {
                    s_hover_widget->mouseleave();
                }
                s_hover_widget = wdg;
            }
            wdg->mousemove(LOWORD(lParam), HIWORD(lParam));
            return 0;
        }
        if (uMsg == WM_MOUSELEAVE) {
            if (s_hover_widget == wdg) {
                wdg->mouseleave();
                s_hover_widget = nullptr;
            }
            return 0;
        }
        if (uMsg == WM_KEYDOWN || uMsg == WM_KEYUP) {
            auto code = keycode(wParam);
            if (wParam >= VK_SHIFT && wParam <= VK_MENU) {
                // Shift, ctrl, and alt need special processing in order to
                // differentiate between left-hand and right-hand keys.
                code = keycode(
                    MapVirtualKey((lParam >> 16) & 0xFF, MAPVK_VSC_TO_VK_EX)
                );
            }
            wdg->key(code, uMsg == WM_KEYDOWN);
            return 0;
        }
        if (uMsg == WM_CHAR) {
            auto str = util::wstr_to_u8str(std::wstring(1, wchar_t(wParam)));
            wdg->text(str.c_str());
            return 0;
        }
        if (uMsg == WM_PAINT) {
            PAINTSTRUCT paint;
            HDC hdc = BeginPaint(hwnd, &paint);

            int width, height;
            wdg->get_real_size(width, height);

            gl::renderbuffer rbo;
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB8, width, height);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            glViewport(0, 0, width, height);
            glScissor(0, 0, width, height);

            wdg->draw_gl(width, height, rbo);

            gl::framebuffer fbo;
            glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
            glFramebufferRenderbuffer(
                GL_READ_FRAMEBUFFER,
                GL_COLOR_ATTACHMENT0,
                GL_RENDERBUFFER,
                rbo
            );
            wglMakeCurrent(hdc, gl::g_hglrc);
            glBlitFramebuffer(
                0, 0,
                width, height,
                0, 0,
                width, height,
                GL_COLOR_BUFFER_BIT,
                GL_NEAREST
            );
            SwapBuffers(hdc);
            wglMakeCurrent(gl::g_hdc, gl::g_hglrc);
            glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

            EndPaint(hwnd, &paint);
        }
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    };
    static std::once_flag wndclass_flag;
    std::call_once(wndclass_flag, [] {
        WNDCLASSW wndclass{};
        wndclass.style = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc = wndproc;
        wndclass.hInstance = GetModuleHandleW(nullptr);
        wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wndclass.lpszClassName = L"DRNSF_GUI_WIDGET_GL";
        if (!RegisterClassW(&wndclass)) {
            throw std::runtime_error("gui::widget_gl: failed to register");
        }
    });

    m_handle = CreateWindowExW(
        0,
        L"DRNSF_GUI_WIDGET_GL",
        nullptr,
        WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        0, 0,
        0, 0,
        HWND(parent.get_container_handle()),
        nullptr,
        GetModuleHandleW(nullptr),
        this
    );
    if (!m_handle) {
        throw std::runtime_error("gui::widget_gl: failed to create window");
    }
    // m_handle is released by the base class destructor on exception.

    HDC hdc = GetDC(HWND(m_handle));
    if (!hdc) {
        throw std::runtime_error("gui::widget_gl: failed to get window hdc");
    }
    DRNSF_ON_EXIT { ReleaseDC(HWND(m_handle), hdc); };

    if (!SetPixelFormat(hdc, gl::g_pfid, &gl::g_pfd)) {
        throw std::runtime_error("gui::widget_gl: failed to set pixel format");
    }
#else
#error Unimplemented UI frontend code.
#endif

    set_layout(layout);
}

// declared in gui.hh
void widget_gl::invalidate()
{
#if USE_X11
    m_dirty = true;
#elif USE_WINAPI
    InvalidateRect(HWND(m_handle), nullptr, false);
#else
#error Unimplemented UI frontend code.
#endif
}

}
}
