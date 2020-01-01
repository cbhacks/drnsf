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
#include <glm/gtc/matrix_transform.hpp>
#include "render.hh"
#include "gl.hh"

namespace drnsf {
namespace render {

// (inner class) impl
// Implementation class for viewport (PIMPL).
class viewport::impl final : private gui::widget_gl, private core::worker {
    friend class viewport;

private:
    // (var) m_outer
    // A reference to the outer viewport.
    viewport &m_outer;

    // (var) m_camera
    // The configuration used for this viewport.
    camera m_camera;

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

    // (var) m_key_w_down, m_key_a_down, m_key_s_down, m_key_d_down,
    //       m_key_q_down, m_key_e_down, m_key_shift_down
    // True if the respective key is currently down for this widget; false
    // otherwise.
    bool m_key_w_down = false;
    bool m_key_a_down = false;
    bool m_key_s_down = false;
    bool m_key_d_down = false;
    bool m_key_q_down = false;
    bool m_key_e_down = false;
    bool m_key_shift_down = false;

    // (var) m_key_uarrow_down, m_key_darrow_down,
    //       m_key_larrow_down, m_key_rarrow_down
    // True if the respective arrow key is currently down for this widget;
    // false otherwise.
    bool m_key_uarrow_down = false;
    bool m_key_darrow_down = false;
    bool m_key_larrow_down = false;
    bool m_key_rarrow_down = false;

    // (var) m_stopwatch
    // A tool for measuring time for changes which apply over time, such as
    // WASDQE camera movement inputs.
    util::stopwatch m_stopwatch;

    // FIXME non-pimpl later on
    void draw_gl(int width, int height, unsigned int rbo) override;
    void mousemove(int x, int y) override;
    void mousewheel(int delta_y) override;
    void mousebutton(gui::mousebtn btn, bool down) override;
    void key(gui::keycode code, bool down) override;
    int work() noexcept override;

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
    // Detach from the scene first.
    set_scene(nullptr);

    delete M;
}

// declared above FIXME
void viewport::impl::draw_gl(int width, int height, unsigned int rbo)
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

    // Exit early if no scene is attached.
    if (!m_outer.m_scene) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        return;
    }

    // Enable Z-buffer / depth testing.
    glEnable(GL_DEPTH_TEST);

    scene::env e;

    // Build the projection matrix.
    e.projection = glm::infinitePerspective(
        glm::radians(80.0f),
        static_cast<float>(width) / height,
        200.0f
    );
    e.projection = glm::translate(
        e.projection,
        glm::vec3(0.0f, 0.0f, -200.0f)
    );
    e.projection *= glm::mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    // Build the view matrix.
    e.view = glm::translate(
        glm::mat4(1.0f),
        glm::vec3(0.0f, -m_camera.distance, 0.0f)
    );
    e.view = glm::rotate(
        e.view,
        glm::radians(m_camera.pitch),
        glm::vec3(1.0f, 0.0f, 0.0f)
    );
    e.view = glm::rotate(
        e.view,
        glm::radians(90.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    e.view = glm::rotate(
        e.view,
        glm::radians(m_camera.yaw),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );
    e.view_nomove = e.view;
    e.view = glm::translate(e.view, m_camera.pivot);

    // Render the visible figures in the scene.
    m_outer.m_scene->draw(e);

    // Restore the default GL state.
    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

// declared above FIXME
void viewport::impl::mousemove(int x, int y)
{
    int delta_x = x - m_mouse_x_prev;
    int delta_y = y - m_mouse_y_prev;

    if (m_mouse1_down && m_mouse2_down) {
        // Zoom if both buttons are held.

        m_camera.distance -= m_camera.distance * 0.01 * delta_y;
        if (m_camera.distance < camera::min_distance) {
            m_camera.distance = camera::min_distance;
        }
        invalidate();//FIXME remove
    } else if (m_mouse1_down) {
        // Rotate if only "left" button is held.

        m_camera.yaw -= delta_x;
        m_camera.pitch -= delta_y;
        if (m_camera.pitch > 90.0f) {
            m_camera.pitch = 90.0f;
        } else if (m_camera.pitch < -90.0f) {
            m_camera.pitch = -90.0f;
        }
        invalidate();//FIXME remove
    } else if (m_mouse2_down) {
        // Rotate if only "right" button is held.

        using glm::sin;
        using glm::cos;
        using glm::radians;

        glm::vec3 camera_pos(
            m_camera.pivot.x -
                m_camera.distance *
                cos(radians(m_camera.yaw)) *
                cos(radians(m_camera.pitch)),
            m_camera.pivot.y +
                m_camera.distance *
                sin(radians(m_camera.yaw)) *
                cos(radians(m_camera.pitch)),
            m_camera.pivot.z +
                m_camera.distance *
                sin(radians(m_camera.pitch))
        );

        m_camera.yaw -= delta_x;
        m_camera.pitch -= delta_y;
        if (m_camera.pitch > 90.0f) {
            m_camera.pitch = 90.0f;
        } else if (m_camera.pitch < -90.0f) {
            m_camera.pitch = -90.0f;
        }

        m_camera.pivot = {
            camera_pos.x -
                m_camera.distance *
                cos(radians(m_camera.yaw + 180)) *
                cos(radians(m_camera.pitch)),
            camera_pos.y +
                m_camera.distance *
                sin(radians(m_camera.yaw + 180)) *
                cos(radians(m_camera.pitch)),
            camera_pos.z +
                m_camera.distance *
                sin(radians(m_camera.pitch + 180))
        };

        invalidate(); //FIXME remove
    }

    m_mouse_x_prev = x;
    m_mouse_y_prev = y;
}

// declared above FIXME
void viewport::impl::mousewheel(int delta_y)
{
    m_camera.distance -= m_camera.distance * 0.1 * delta_y;
    if (m_camera.distance < camera::min_distance) {
        m_camera.distance = camera::min_distance;
    }
    invalidate();//FIXME remove
}

// declared above FIXME
void viewport::impl::mousebutton(gui::mousebtn btn, bool down)
{
    switch (btn) {
    case gui::mousebtn::left:
        work();
        m_mouse1_down = down;
        break;
    case gui::mousebtn::right:
        work();
        m_mouse2_down = down;
        break;
    default:
        // Silence gcc warning for -Wswitch.
        break;
    }
}

// declared above FIXME
void viewport::impl::key(gui::keycode code, bool down)
{
    switch (code) {
    case gui::keycode::W:
        work();
        m_key_w_down = down;
        break;
    case gui::keycode::A:
        work();
        m_key_a_down = down;
        break;
    case gui::keycode::S:
        work();
        m_key_s_down = down;
        break;
    case gui::keycode::D:
        work();
        m_key_d_down = down;
        break;
    case gui::keycode::Q:
        work();
        m_key_q_down = down;
        break;
    case gui::keycode::E:
        work();
        m_key_e_down = down;
        break;
    case gui::keycode::up_arrow:
        work();
        m_key_uarrow_down = down;
        break;
    case gui::keycode::down_arrow:
        work();
        m_key_darrow_down = down;
        break;
    case gui::keycode::left_arrow:
        work();
        m_key_larrow_down = down;
        break;
    case gui::keycode::right_arrow:
        work();
        m_key_rarrow_down = down;
        break;
    case gui::keycode::l_shift:
    case gui::keycode::r_shift:
        work();
        m_key_shift_down = down;
        break;
    default:
        // Silence gcc warning for -Wswitch.
        break;
    }
}

// declared above FIXME
int viewport::impl::work() noexcept
{
    auto delta_ms = m_stopwatch.lap();
    float vp_speed = 10000.0f;

    // Apply fast-movement speed if the appropriate key is held.
    if (m_key_shift_down) {
        vp_speed *= 8;
    }

    int move_x = 0;
    int move_y = 0;
    int move_z = 0;

    // Calculate the total movement vectors based on keyboard input status.
    if (m_key_w_down)
        move_y--;
    if (m_key_s_down)
        move_y++;
    if (m_key_a_down || m_key_larrow_down)
        move_x--;
    if (m_key_d_down || m_key_rarrow_down)
        move_x++;
    if (m_key_q_down || m_key_darrow_down)
        move_z--;
    if (m_key_e_down || m_key_uarrow_down)
        move_z++;

    // Exit early with no need for future updates if there is no movement, or
    // all movement is cancelled (e.g. W + S results in forward and backward
    // movement cancelling eachother out).
    if (!move_x && !move_y && !move_z)
        return INT_MAX;

    auto absolute_delta =
        glm::vec3(float(move_x), float(move_y), float(move_z)) *
        (delta_ms * vp_speed / 1000.0f);

    if (m_mouse1_down && !m_mouse2_down) {
        // WASDQE moves on the absolute X/Y/Z axis if only the "left" button
        // is held.

        m_camera.pivot -= absolute_delta;

        invalidate();
        return 0;
    } else if (m_mouse2_down && !m_mouse1_down) {
        // WASDQE moves with respect to the camera's orientation if only the
        // "right" button is held.

        glm::mat4 rotation;
        rotation = glm::rotate(
            glm::mat4(1.0f),
            glm::radians(m_camera.yaw),
            glm::vec3(0.0f, 0.0f, -1.0f)
        );
        rotation = glm::rotate(
            rotation,
            glm::radians(90.0f),
            glm::vec3(0.0f, 0.0f, -1.0f)
        );
        rotation = glm::rotate(
            rotation,
            glm::radians(m_camera.pitch),
            glm::vec3(-1.0f, 0.0f, 0.0f)
        );

        auto relative_delta = glm::vec3(rotation * glm::vec4(absolute_delta, 1.0f));

        m_camera.pivot -= relative_delta;

        invalidate();
        return 0;
    }

    return INT_MAX;
}

// declared in render.hh
void viewport::invalidate()
{
    M->invalidate();
}

// declared in render.hh
scene * const &viewport::get_scene() const
{
    return m_scene;
}

// declared in render.hh
void viewport::set_scene(scene *scene)
{
    if (scene == m_scene)
        return;

    if (scene) {
        // Do the insertion first. If this fails (out of memory), no change
        // should occur to the viewport's attachment.
        scene->m_viewports.insert(this);
    }

    if (m_scene) {
        m_scene->m_viewports.erase(this);
    }

    m_scene = scene;
    invalidate();
}

}
}
