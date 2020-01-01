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

#if USE_X11
// (s-var) s_ending
// This flag is set by gui::end and informs gui::run to exit its loop.
static bool s_ending;

// declared in gui.hh
Display *g_display = nullptr;

// declared in gui.hh
XContext g_ctx_ptr;

// declared in gui.hh
Atom g_xa_protocols;

// declared in gui.hh
Atom g_xa_delete_window;
#endif

// declared in gui.hh
void init(int &argc, char **&argv)
{
#if USE_X11
    g_display = XOpenDisplay(nullptr);
    if (!g_display) {
        // TODO report error
        std::exit(EXIT_FAILURE);
    }

#if !NDEBUG
    // Enable auto-sync in debug builds. This ensures any X errors which occur
    // will be raised at the location they were triggered by. Without this,
    // errors may be deferred all the way until the next XPending call in run().
    XSynchronize(g_display, true);
#endif

    g_ctx_ptr = XUniqueContext();

    g_xa_protocols = XInternAtom(g_display, "WM_PROTOCOLS", false);
    g_xa_delete_window = XInternAtom(g_display, "WM_DELETE_WINDOW", false);
#endif
}

// declared in gui.hh
void run()
{
#if USE_X11
    // Save the previous gui::end status. This way, if gui::end is called but a
    // new gui::run is called before the previous gui::run exits, the previous
    // one will exit after this one.
    bool previous_ending = s_ending;
    s_ending = false;
    DRNSF_ON_EXIT { s_ending = previous_ending; };

    // Prepare values for a non-blocking or timeout-blocking `select' call.
    // XPending or XNextEvent will block if there are no events ready, but we
    // want to add a timeout to allow UI elements to execute their periodic
    // updates (such as ImGui widgets).
    int connfd = ConnectionNumber(g_display);
    fd_set readfds;
    timeval timeout{};

    while (!s_ending) {
        FD_ZERO(&readfds);
        FD_SET(connfd, &readfds);
        if (!XQLength(g_display)) {
            // XQLength checks Xlib's internal buffer for events, similar to
            // XPending below, but does not attempt to read more from the X fd
            // if there are none (a possibly blocking operation).
            //
            // Normally XQLength should return zero here, as all of the pending
            // events, aside from ones still waiting in the fd's buffer, were
            // already processed by the loop during the previous iteration.
            // However, this is NOT the case if another function has since then
            // pumped the fd for more messages and queued them up, which does
            // apparently occur if XSynchronize is enabled, which we do for
            // debug builds.
            //
            // In such a case, if there are pending events, we leave the fd set
            // in the fd_set above without calling select. This results in the
            // event handling section running again.
            int err = select(connfd + 1, &readfds, nullptr, nullptr, &timeout);
            if (err == -1) {
                // EINTR is a possible error if a signal interrupts the select()
                // call. In this case, select can simply be called again.
                if (errno == EINTR) {
                    timeout = timeval{};
                    continue;
                }
                throw std::runtime_error("gui::run: select failed");
            }
        }

        if (FD_ISSET(connfd, &readfds)) {
            // Get the number of pending X events. In the case of reentrancy,
            // the reentrant `gui::run' call will drain the evcount value to
            // zero for us.
            static int evcount;
            evcount = XPending(g_display);
            while (evcount > 0) {
                XEvent ev;
                XNextEvent(g_display, &ev);
                evcount--;

                XPointer ptr;
                if (XFindContext(g_display, ev.xany.window, g_ctx_ptr, &ptr)) {
                    // Not an X window we know or care about.
                    continue;
                }

                // Handle this event if it's a widget.
                auto wdg = reinterpret_cast<widget *>(ptr);
                if (widget::s_all_widgets.find(wdg)
                    != widget::s_all_widgets.end()) {
                    switch (ev.type) {
                    case ButtonPress:
                    case ButtonRelease:
                        if (ev.xbutton.button == 4) {
                            wdg->mousewheel(1);
                        } else if (ev.xbutton.button == 5) {
                            wdg->mousewheel(-1);
                        } else {
                            wdg->mousebutton(
                                mousebtn(ev.xbutton.button),
                                ev.type == ButtonPress
                            );
                        }
                        break;
                    case KeyPress:
                    case KeyRelease: {
                        char buf[30] = {};
                        KeySym sym = XLookupKeysym(&ev.xkey, 0);
                        int len = XLookupString(
                            &ev.xkey,
                            buf,
                            sizeof(buf),
                            nullptr,
                            nullptr
                        );
                        if (sym != NoSymbol) {
                            wdg->key(keycode(sym), ev.type == KeyPress);
                        }
                        if (len > 0 && ev.type == KeyPress) {
                            wdg->text(buf);
                        }
                        break; }
                    case EnterNotify:
                        if (wdg != widget::s_hover_widget) {
                            if (widget::s_hover_widget) {
                                widget::s_hover_widget->mouseleave();
                            }
                            widget::s_hover_widget = wdg;
                        }
                        wdg->mousemove(ev.xcrossing.x, ev.xcrossing.y);
                        break;
                    case LeaveNotify:
                        if (wdg == widget::s_hover_widget) {
                            wdg->mouseleave();
                            widget::s_hover_widget = nullptr;
                        }
                        break;
                    case MotionNotify:
                        wdg->mousemove(ev.xmotion.x, ev.xmotion.y);
                        break;
                    case Expose:
                        wdg->m_dirty = true;
                        break;
                    case ConfigureNotify:
                        wdg->m_dirty = true;
                        wdg->m_real_width = ev.xconfigure.width;
                        wdg->m_real_height = ev.xconfigure.height;
                        wdg->on_resize(wdg->m_real_width, wdg->m_real_height);
                        break;
                    }
                }

                // Handle this event if it's a window.
                auto wnd = reinterpret_cast<window *>(ptr);
                if (window::s_all_windows.find(wnd)
                    != window::s_all_windows.end()) {
                    switch (ev.type) {
                    case ConfigureNotify:
                        wnd->m_width = ev.xconfigure.width;
                        wnd->m_height = ev.xconfigure.height;
                        wnd->apply_layouts();
                        break;
                    case ClientMessage:
                        if (ev.xclient.message_type == g_xa_protocols) {
                            auto protocol = Atom(ev.xclient.data.l[0]);
                            if (protocol == g_xa_delete_window) {
                                wnd->on_close_request();
                            }
                        }
                        break;
                    }
                    continue;
                }

                // Handle this event if it's a popup.
                auto pp = reinterpret_cast<popup *>(ptr);
                if (popup::s_all_popups.find(pp)
                    != popup::s_all_popups.end()) {
                    switch (ev.type) {
                    case ConfigureNotify:
                        pp->m_width = ev.xconfigure.width;
                        pp->m_height = ev.xconfigure.height;
                        pp->apply_layouts();
                        break;
                    }
                    continue;
                }

                // Couldn't find a matching widget, window, or popup for this
                // event. Maybe the object was destroyed before this event was
                // finally delivered?
            }

            // Skip calling update on any widgets. select() will be called with
            // a zero timeout, so if there are no more X events, widgets will be
            // updated on that iteration.
            timeout = timeval{};
            continue;
        }

        // Update all of the workers. This also produces the timeout for the
        // next loop iteration, based on the shortest time received from all
        // of the workers.
        timeout = timeval{};
        long min_delay = core::update();
        if (min_delay < LONG_MAX / 1000) {
            timeout.tv_usec = min_delay * 1000L;
        } else {
            // If the shortest delay is extremely large (INT_MAX most
            // likely), set an arbitrarily long delay.
            timeout.tv_sec = 4;
        }

        // Draw all of the widgets which need to be redrawn.
        for (auto &&w : widget::s_all_widgets) {
            if (w->m_dirty) {
                XClearWindow(g_display, w->m_handle);
                w->on_draw();
                w->m_dirty = false;
            }
        }
    }
#elif USE_WINAPI
    while (true) {
        // Handle all pending window messages. WM_QUIT messages indicate the
        // loop should exit entirely. If a WM_PAINT message is encountered
        // after processing non-WM_PAINT messages, defer processing of them
        // until the next loop iteration. This ensures drawing code doesn't
        // starve widgets of any `update()' calls.
        MSG msg;
        bool had_nonpaint_message = false;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                return;
            }
            if (msg.message == WM_PAINT) {
                if (!had_nonpaint_message) {
                    DispatchMessage(&msg);
                    continue;
                } else {
                    break;
                }
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
            had_nonpaint_message = true;
        }

        // Update all of the workers. This also produces the timeout for the
        // next loop iteration, based on the shortest time received from all
        // of the workers.
        unsigned int timeout = core::update();

        // Block pending new thread or window messages. `MWMO_INPUTAVAILABLE'
        // is specified so that the function does not block if a previously
        // seen but unhandled event is pending, such as a deferred WM_PAINT
        // event from the previous loop.
        MsgWaitForMultipleObjectsEx(
            0,
            nullptr,
            timeout,
            QS_ALLEVENTS,
            MWMO_INPUTAVAILABLE
        );
    }
#else
#error Unimplemented UI frontend code.
#endif
}

// declared in gui.hh
void end()
{
#if USE_X11
    s_ending = true;
#elif USE_WINAPI
    PostQuitMessage(0);
#else
#error Unimplemented UI frontend code.
#endif
}

}
}
