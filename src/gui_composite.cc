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

namespace drnsf {
namespace gui {

// declared in gui.hh
void composite::on_resize(int width, int height)
{
    apply_layouts();
}

// declared in gui.hh
composite::composite(container &parent, layout layout) :
    widget(parent)
{
#if USE_X11
    XSetWindowAttributes attr{};
    attr.background_pixel = WhitePixel(g_display, DefaultScreen(g_display));
    attr.event_mask = StructureNotifyMask;
    m_handle = XCreateWindow(
        g_display,
        parent.get_container_handle(),
        0, 0,
        1, 1,
        0,
        CopyFromParent,
        InputOutput,
        CopyFromParent,
        CWBackPixel | CWEventMask,
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
        auto wdg = reinterpret_cast<composite *>(
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
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    };
    static std::once_flag wndclass_flag;
    std::call_once(wndclass_flag, [] {
        WNDCLASSW wndclass{};
        wndclass.lpfnWndProc = wndproc;
        wndclass.hInstance = GetModuleHandleW(nullptr);
        wndclass.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wndclass.hbrBackground = HBRUSH(COLOR_3DFACE + 1);
        wndclass.lpszClassName = L"DRNSF_GUI_COMPOSITE";
        if (!RegisterClassW(&wndclass)) {
            throw std::runtime_error("gui::composite: failed to register");
        }
    });

    m_handle = CreateWindowExW(
        0,
        L"DRNSF_GUI_COMPOSITE",
        nullptr,
        WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        0, 0,
        1, 1,
        HWND(parent.get_container_handle()),
        nullptr,
        GetModuleHandleW(nullptr),
        this
    );
    if (!m_handle) {
        throw std::runtime_error("gui::composite: failed to create window");
    }
    // m_handle is released by the base class destructor on exception.
#else
#error Unimplemented UI frontend code.
#endif

    set_layout(layout);
}

// declared in gui.hh
sys_handle composite::get_container_handle()
{
    return m_handle;
}

// declared in gui.hh
void composite::get_child_area(int &ctn_x, int &ctn_y, int &ctn_w, int &ctn_h)
{
    ctn_x = 0;
    ctn_y = 0;
    get_real_size(ctn_w, ctn_h);
}

}
}
