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
#include <glm/gtc/matrix_transform.hpp>
#include "render.hh"

#include "edit.hh" // FIXME temporary

namespace drnsf {
namespace render {

// (inner class) impl
// Implementation class for viewport (PIMPL).
class viewport::impl : private util::nocopy {
    friend class viewport;

private:
    // (var) m_outer
    // A reference to the outer viewport.
    viewport &m_outer;

    // (var) m_canvas
    // The viewport's OpenGL canvas.
    gui::gl_canvas m_canvas;

    // (var) m_mouse_down
    // True if the mouse button (primary/"left") is currently down on this
    // widget; false if not.
    bool m_mouse_down = false;

    // (var) m_mouse_x_prev, m_mouse_y_prev
    // The location of the mouse at the last on_mousemove or on_mousebutton
    // event.
    int m_mouse_x_prev;
    int m_mouse_y_prev;

    // (handler) h_render
    // Hook for the GL canvas on_render event.
    decltype(m_canvas.on_render)::watch h_render;

    // (handler) h_mousemove
    // Hook for the GL canvas on_mousemove event.
    decltype(m_canvas.on_mousemove)::watch h_mousemove;

    // (handle) h_mousewheel
    // Hook for the GL canvas on_mousewheel event.
    decltype(m_canvas.on_mousewheel)::watch h_mousewheel;

    // (handle) h_mousebutton
    // Hook for the GL canvas on_mousebutton event.
    decltype(m_canvas.on_mousebutton)::watch h_mousebutton;

public:
    // (explicit ctor)
    // Initializes the viewport widget.
    explicit impl(
        viewport &outer,
        gui::container &parent) :
        m_outer(outer),
        m_canvas(parent)
    {
        h_render <<= [this](int width,int height) {
            // Clear the display and reset the depth buffer.
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Enable Z-buffer / depth testing.
            glEnable(GL_DEPTH_TEST);

            // Build the projection matrix.
            auto projection = glm::perspective(
                70.0f,
                static_cast<float>(width) / height,
                500.0f,
                200000.0f
            );
            projection = glm::translate(
                projection,
                glm::vec3(0.0f,0.0f,-800.0f)
            );
            projection = glm::translate(
                projection,
                glm::vec3(0.0f,0.0f,-edit::g_camera_zoom)
            );

            // Build the view matrix.
            glm::mat4 view(1.0f);
            view = glm::rotate(
                view,
                glm::radians(edit::g_camera_pitch),
                glm::vec3(1.0f,0.0f,0.0f)
            );
            view = glm::rotate(
                view,
                glm::radians(edit::g_camera_yaw),
                glm::vec3(0.0f,1.0f,0.0f)
            );

            // Render the viewport's visible figures.
            figure::env e;
            e.projection_matrix = projection;
            for (auto &&fig : m_outer.m_figs) {
                if (!fig->m_visible) continue;
                e.modelview_matrix = view * fig->m_matrix;
                e.matrix = projection * e.modelview_matrix;
                fig->draw(e);
            }

            // Restore the default GL state.
            glDisable(GL_DEPTH_TEST);
        };
        h_render.bind(m_canvas.on_render);

        h_mousemove <<= [this](int x,int y) {
            if (m_mouse_down) {
                int delta_x = x - m_mouse_x_prev;
                int delta_y = y - m_mouse_y_prev;

                edit::g_camera_yaw += delta_x;

                edit::g_camera_pitch += delta_y;
                if (edit::g_camera_pitch > 90.0f) {
                    edit::g_camera_pitch = 90.0f;
                } else if (edit::g_camera_pitch < -90.0f) {
                    edit::g_camera_pitch = -90.0f;
                }
                m_canvas.invalidate();//FIXME remove
            }

            m_mouse_x_prev = x;
            m_mouse_y_prev = y;
        };
        h_mousemove.bind(m_canvas.on_mousemove);

        h_mousewheel <<= [this](int delta_y) {
            edit::g_camera_zoom -= edit::g_camera_zoom * 0.1 * delta_y;
            if (edit::g_camera_zoom < 500.0f) {
                edit::g_camera_zoom = 500.0f;
            }
            m_canvas.invalidate();//FIXME remove
        };
        h_mousewheel.bind(m_canvas.on_mousewheel);

        h_mousebutton <<= [this](int button,bool down) {
            if (button == 1) {
                m_mouse_down = down;
            }
        };
        h_mousebutton.bind(m_canvas.on_mousebutton);
    }
};

// declared in render.hh
viewport::viewport(gui::container &parent)
{
    M = new impl(*this,parent);
}

// declared in render.hh
viewport::~viewport()
{
    delete M;
}

// declared in render.hh
GtkWidget *viewport::get_handle()
{
    return M->m_canvas.get_handle();
}

// declared in render.hh
void viewport::invalidate()
{
    M->m_canvas.invalidate();
}

}
}
