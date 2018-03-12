//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2018  DRNSF contributors
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
class viewport::impl final : private gui::widget_gl {
    friend class viewport;

private:
    // (var) m_outer
    // A reference to the outer viewport.
    viewport &m_outer;

    // (var) m_mouse1_down
    // True if the mouse button (primary/"left") is currently down on this
    // widget; false if not.
    bool m_mouse1_down = false;

    // (var) m_mouse2_down
    // True if the alternate mouse button (secondary/"right") is currently down
    // on this widget; false if not.
    bool m_mouse2_down = false;

    // (var) m_mouse_x_prev, m_mouse_y_prev
    // The location of the mouse at the last on_mousemove or on_mousebutton
    // event.
    int m_mouse_x_prev;
    int m_mouse_y_prev;

    // FIXME non-pimpl later on
    void draw_gl(int width, int height, gl::renderbuffer &rbo) override;
    void mousemove(int x, int y) override;
    void mousewheel(int delta_y) override;
    void mousebutton(int number, bool down) override;

public:
    // (explicit ctor)
    // Initializes the viewport widget.
    explicit impl(viewport &outer) :
        widget_gl(outer, gui::layout::fill()),
        m_outer(outer)
    {
    }
};

// declared in render.hh
viewport::viewport(gui::container &parent, gui::layout layout) :
    composite(parent, layout)
{
    M = new impl(*this);
    M->show();
}

// declared in render.hh
viewport::~viewport()
{
    delete M;
}

// declared above FIXME
void viewport::impl::draw_gl(int width, int height, gl::renderbuffer &rbo)
{
    // Prepare a depth buffer.
    gl::renderbuffer depth_rbo;
    glBindRenderbuffer(GL_RENDERBUFFER, depth_rbo);
    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH_COMPONENT16,
        width,
        height
    );
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Prepare the framebuffer for this render job.
    gl::framebuffer fbo;
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glFramebufferRenderbuffer(
        GL_DRAW_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_RENDERBUFFER,
        rbo
    );
    glFramebufferRenderbuffer(
        GL_DRAW_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER,
        depth_rbo
    );

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
        glm::vec3(0.0f, 0.0f, -800.0f)
    );

    // Build the view matrix.
    glm::mat4 view(1.0f);
    view = glm::translate(
        view,
        glm::vec3(0.0f, 0.0f, -edit::g_camera_zoom)
    );
    view = glm::rotate(
        view,
        glm::radians(edit::g_camera_pitch),
        glm::vec3(1.0f, 0.0f, 0.0f)
    );
    view = glm::rotate(
        view,
        glm::radians(edit::g_camera_yaw),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // Render the viewport's visible figures.
    figure::env e;
    e.view = view;
    e.projection = projection;
    for (auto &&fig : m_outer.m_figs) {
        if (!fig->m_visible) continue;
        fig->draw(e);
    }

    // Restore the default GL state.
    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

// declared above FIXME
void viewport::impl::mousemove(int x, int y)
{
    if (m_mouse1_down && m_mouse2_down) {
        // Zoom if both buttons are held.
        int delta_y = y - m_mouse_y_prev;

        edit::g_camera_zoom -= edit::g_camera_zoom * 0.01 * delta_y;
        if (edit::g_camera_zoom < 500.0f) {
            edit::g_camera_zoom = 500.0f;
        }
        invalidate();//FIXME remove
    } else if (m_mouse1_down) {
        // Rotate if only "left" button is held.
        int delta_x = x - m_mouse_x_prev;
        int delta_y = y - m_mouse_y_prev;

        edit::g_camera_yaw += delta_x;

        edit::g_camera_pitch += delta_y;
        if (edit::g_camera_pitch > 90.0f) {
            edit::g_camera_pitch = 90.0f;
        } else if (edit::g_camera_pitch < -90.0f) {
            edit::g_camera_pitch = -90.0f;
        }
        invalidate();//FIXME remove
    }

    m_mouse_x_prev = x;
    m_mouse_y_prev = y;
}

// declared above FIXME
void viewport::impl::mousewheel(int delta_y)
{
    edit::g_camera_zoom -= edit::g_camera_zoom * 0.1 * delta_y;
    if (edit::g_camera_zoom < 500.0f) {
        edit::g_camera_zoom = 500.0f;
    }
    invalidate();//FIXME remove
}

// declared above FIXME
void viewport::impl::mousebutton(int number, bool down)
{
    switch (number) {
    case 1:
        m_mouse1_down = down;
        break;
    case 3:
        m_mouse2_down = down;
        break;
    }
}

// declared in render.hh
void viewport::invalidate()
{
    M->invalidate();
}

}
}
