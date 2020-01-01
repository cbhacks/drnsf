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
std::unordered_set<window *> window::s_all_windows;

// declared in gui.hh
window::window(const std::string &title, int width, int height) :
    m_width(width),
    m_height(height)
{
#if USE_X11
    XSetWindowAttributes attr{};
    attr.background_pixel = WhitePixel(g_display, DefaultScreen(g_display));
    attr.event_mask = StructureNotifyMask;
    m_handle = XCreateWindow(
        g_display,
        DefaultRootWindow(g_display),
        0, 0,
        width, height,
        0,
        CopyFromParent,
        InputOutput,
        CopyFromParent,
        CWBackPixel | CWEventMask,
        &attr
    );
    // FIXME on error free window

    XStoreName(g_display, m_handle, title.c_str());
    XSaveContext(g_display, m_handle, g_ctx_ptr, XPointer(this));

    // Advertise support for certain ICCCM protocols.
    Atom protocols[] = {
        g_xa_delete_window
    };
    XSetWMProtocols(
        g_display,
        m_handle,
        protocols,
        sizeof(protocols) / sizeof(*protocols)
    );
#elif USE_WINAPI
    static auto wndproc = [](
        HWND hwnd,
        UINT uMsg,
        WPARAM wParam,
        LPARAM lParam) -> LRESULT {
        auto wnd = reinterpret_cast<window *>(
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
            wnd->m_width = LOWORD(lParam);
            wnd->m_height = HIWORD(lParam);
            wnd->apply_layouts();
            return 0;
        }
        if (uMsg == WM_CLOSE) {
            wnd->on_close_request();
            return 0;
        }
        if (uMsg == WM_COMMAND) {
            command::dispatch(LOWORD(wParam));
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
        wndclass.lpszClassName = L"DRNSF_GUI_WINDOW";
        if (!RegisterClassW(&wndclass)) {
            throw std::runtime_error("gui::window: failed to register");
        }
    });

    auto title_w = util::u8str_to_wstr(title);

    m_handle = CreateWindowExW(
        0,
        L"DRNSF_GUI_WINDOW",
        title_w.c_str(),
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        nullptr,
        nullptr,
        GetModuleHandleW(nullptr),
        this
    );
    if (!m_handle) {
        throw std::runtime_error("gui::window: failed to create window");
    }
    // FIXME on error free window
#else
#error Unimplemented UI frontend code.
#endif

    s_all_windows.insert(this);
}

// declared in gui.hh
window::~window()
{
    s_all_windows.erase(this);

#if USE_X11
    XDestroyWindow(g_display, m_handle);
#elif USE_WINAPI
    DestroyWindow(HWND(m_handle));
#else
#error Unimplemented UI frontend code.
#endif
}

// declared in gui.hh
void window::show()
{
#if USE_X11
    XMapWindow(g_display, m_handle);
#elif USE_WINAPI
    ShowWindow(HWND(m_handle), SW_SHOW);
#else
#error Unimplemented UI frontend code.
#endif
}

// declared in gui.hh
void window::show_dialog()
{
#if USE_X11
    XMapWindow(g_display, m_handle);
    // FIXME modal window
    run();
    XUnmapWindow(g_display, m_handle);
#elif USE_WINAPI
    ShowWindow(HWND(m_handle), SW_SHOW);
    // FIXME modal window
    run();
    ShowWindow(HWND(m_handle), SW_HIDE);
#endif
}

// declared in gui.hh
sys_handle window::get_container_handle()
{
    return m_handle;
}

// declared in gui.hh
void window::get_child_area(int &ctn_x, int &ctn_y, int &ctn_w, int &ctn_h)
{
    ctn_x = 0;
    ctn_y = 0;
    ctn_w = m_width;
    ctn_h = m_height;

#if !USE_NATIVE_MENU
    // Adjust the area for a non-native menubar if present. The menubar's own
    // layout places it above the top of the child area.
    if (m_menubar) {
        ctn_y += 20;
        ctn_h -= 20;
    }
#endif
}

}
}
