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

#pragma once

/*
 * render.hh
 *
 * FIXME explain
 */

#include <unordered_set>
#include "res.hh"
#include "gfx.hh"
#include "gui.hh"

namespace drnsf {
namespace render {

/*
 * render::camera
 *
 * A camera configuration for use in viewport widgets. Note that this is not
 * some kind of in-game camera structure, only a camera for rendering inside
 * the application.
 */
struct camera {
    static constexpr float default_yaw = 30.0f;
    static constexpr float default_pitch = 30.0f;
    static constexpr float default_zoom = 5000.0f;

    float yaw = default_yaw;
    float pitch = default_pitch;
    float zoom = default_zoom;;
};

// defined later in this file
class figure;

/*
 * render::viewport
 *
 * FIXME explain
 */
class viewport : private gui::composite {
    friend class figure;

private:
    // inner class defined in render_viewport.cc
    class impl;

    // (var) M
    // The pointer to the internal implementation object (PIMPL).
    impl *M;

    // (var) m_figs
    // The set of all figures associated with the viewport, both visible
    // and invisible.
    std::unordered_set<figure *> m_figs;

    // (func) invalidate
    // Used by `render::figure'. Marks the viewport's display as invalid
    // or "dirty" so that it will be re-rendered when necessary.
    void invalidate();

public:
    // (ctor)
    // Constructs an empty viewport widget and places it in the given
    // parent container.
    viewport(gui::container &parent, gui::layout layout);

    // (dtor)
    // Destroys the widget, removing it from the parent container.
    ~viewport();

    using composite::show;
    using composite::hide;
    using composite::get_layout;
    using composite::set_layout;
    using composite::get_real_size;
    using composite::get_screen_pos;
};

/*
 * render::figure
 *
 * FIXME explain
 */
class figure : private util::nocopy {
    friend class viewport;

protected:
    // (inner class) env
    // This structure is used to pass information to the `draw' method
    // without the need to change the function signature every time new
    // information needs to be passed through.
    struct env {
        glm::mat4 projection;
        glm::mat4 view;
    };

private:
    // (var) m_vp
    // A reference to the viewport this figure exists within.
    viewport &m_vp;

    // (var) m_visible
    // True if the figure is visible; false otherwise. A figure which is
    // visible must invalidate its viewport whenever it changes, however a
    // hidden one need not do this (as it is invisible, and therefore does
    // not affect the scene). The viewport must also be invalidated when
    // the visibility of a figure changes.
    bool m_visible = false;

    // (pure func) draw
    // Derived classes must implement this method to draw themselves in
    // whatever way is appropriate. The modelview and projection matrices
    // are given as parameters.
    virtual void draw(const env &e) = 0;

protected:
    // (explicit ctor)
    // Constructs a figure which is associated with the given viewport. The
    // figure is initially not visible.
    explicit figure(viewport &vp);

    // (dtor)
    // Destroys the figure and removes it from the viewport. If the figure
    // was visible, the viewport is invalidated.
    ~figure();

    // (func) invalidate
    // Derived classes should call this function whenever their visual
    // appearance may have changed (such as a moved vertex, color change,
    // texture change, etc). Calling this function on a visible figure will
    // cause the associated viewport to be invalidated (marked as stale or
    // "dirty" so that it will be redrawn).
    void invalidate();

public:
    // (func) show
    // Makes the figure visible, if it was not visible. A change in
    // visibility will invalidate the viewport.
    void show();

    // (func) hide
    // Makes the figure invisible, if it was visible. A change in
    // visibility will invalidate the viewport.
    void hide();
};

/*
 * render::reticle_fig
 *
 * This figure draws a 400 x 400 x 400 wireframe cube with the origin at its
 * center.
 */
class reticle_fig : public figure {
private:
    // (var) m_matrix
    // The model matrix for this figure. The view and projection matrix come
    // from the viewport during a `draw' call.
    glm::mat4 m_matrix{1.0f};

    // (func) draw
    // Implements `figure::draw'.
    void draw(const env &e) override;

public:
    // (explicit ctor)
    // Constructs the reticle figure.
    explicit reticle_fig(viewport &vp) :
        figure(vp) {}

    // (func) get_matrix, set_matrix
    // Gets or sets the model matrix (m_matrix above).
    const glm::mat4 &get_matrix() const;
    void set_matrix(glm::mat4 matrix);
};

/*
 * render::frameonly_fig
 *
 * This figure draws a bunch of points corresponding to the vertex positions in
 * a given gfx::frame.
 *
 * The frame is given directly by pointer. Any code using this class must ensure
 * the current frame is valid at all times.
 */
class frameonly_fig : public figure {
private:
    // (var) m_frame
    // A non-ref pointer to the frame used by this figure.
    gfx::frame *m_frame = nullptr;

    // (var) m_matrix
    // The model matrix for this figure. The view and projection matrix come
    // from the viewport during a `draw' call.
    glm::mat4 m_matrix{1.0f};

    // (func) draw
    // Implements `figure::draw'.
    void draw(const env &e) override;

    // (handler) h_frame_vertices_change
    // Hooks the frame's vertices property change event so that the figure can
    // be updated when the frame's vertices are changed.
    decltype(decltype(gfx::frame::p_vertices)::on_change)::watch
        h_frame_vertices_change;

public:
    // (explicit ctor)
    // Constructs the figure. Initially, it does not reference any frame.
    explicit frameonly_fig(viewport &vp);

    // (func) get_frame, set_frame
    // Gets or sets the gfx::frame reference used by this figure. This may be a
    // null pointer, in which case nothing will be drawn.
    gfx::frame * const &get_frame() const;
    void set_frame(gfx::frame *frame);

    // (func) get_matrix, set_matrix
    // Gets or sets the model matrix (m_matrix above).
    const glm::mat4 &get_matrix() const;
    void set_matrix(glm::mat4 matrix);
};

/*
 * render::animonly_fig
 *
 * This figure draws a bunch of points corresponding to the vertex positions in
 * a given gfx::anim. Only the first frame of animation is drawn at this time.
 *
 * The anim is given directly by pointer. Any code using this class must ensure
 * the anim pointer is valid or null at all times. The frames in the anim need
 * not be valid, however; animonly_fig will track their validity itself and
 * handle any changes in their values or lifetime.
 *
 * This class is only a pseudo-figure. Rather than implementing the `figure'
 * class itself, this class drives an underlying frameonly_fig figure which
 * renders one frame of the given animation.
 */
class animonly_fig : private util::nocopy {
private:
    // (var) m_anim
    // A non-ref pointer to the anim used by this figure.
    gfx::anim *m_anim = nullptr;

    // (var) m_framefig
    // The underlying figure which performs the actual rendering for this class.
    // The animonly_fig drives the frameonly_fig by providing the frame pointers
    // specified in the animation.
    frameonly_fig m_framefig;

    // (handler) h_asset_appear, h_asset_disappear
    // Hooks the anim's project to watch for when the frames referenced by the
    // animation come into and out of existence. This does not, however, track
    // the lifetime of the anim itself. Users of this class must ensure m_anim
    // is either null or pointing to a valid anim at all times.
    decltype(res::project::on_asset_appear)::watch h_asset_appear;
    decltype(res::project::on_asset_disappear)::watch h_asset_disappear;

    // (handler) h_anim_frames_change
    // Hooks the anim's frames property change event to keep track of the anim's
    // frames so that the figure can update itself when they change.
    decltype(decltype(gfx::anim::p_frames)::on_change)::watch
        h_anim_frames_change;

public:
    // (explicit ctor)
    // Constructs the figure. Initially, it does not reference any animation.
    explicit animonly_fig(viewport &vp);

    // (func) show, hide
    // Changes the visibility of the figure. Since this is not a real figure,
    // the show/hide calls are passed on to the actual underlying figure.
    void show();
    void hide();

    // (func) get_anim, set_anim
    // Gets or sets the gfx::anim reference used by this figure. This may be a
    // null pointer, in which case nothing will be drawn.
    gfx::anim * const &get_anim() const;
    void set_anim(gfx::anim *anim);

    // (func) get_matrix, set_matrix
    // Gets or sets the model matrix (held inside of the frameonly_fig).
    const glm::mat4 &get_matrix() const;
    void set_matrix(glm::mat4 matrix);
};

/*
 * render::meshframe_fig
 *
 * This figure draws a gfx::mesh / gfx::frame pairing.
 *
 * The frame and mesh are given directly by pointer. Any code which uses this
 * class must ensure that any non-null frame or mesh pointers are valid at all
 * times.
 */
class meshframe_fig : public figure {
private:
    // (var) m_mesh
    // A pointer to the mesh used by this figure. This may be null, in which
    // case rendering will not occur.
    gfx::mesh *m_mesh;

    // (var) m_frame
    // A pointer to the frame used by this figure. This may be null, in which
    // case rendering will not occur.
    gfx::frame *m_frame;

    // (var) m_matrix
    // The model matrix for this figure. The view and projection matrix come
    // from the viewport during a `draw' call.
    glm::mat4 m_matrix{1.0f};

    // (func) draw
    // Implements `figure::draw'.
    void draw(const env &e) override;

public:
    // (explicit ctor)
    // Constructs the figure. Initially, it does not reference any mesh
    // or frame.
    explicit meshframe_fig(viewport &vp) :
        figure(vp) {}

    // (func) get_mesh, set_mesh
    // Gets or sets the gfx::mesh reference used by this figure. This may be a
    // null reference.
    gfx::mesh * const &get_mesh() const;
    void set_mesh(gfx::mesh *mesh);

    // (func) get_frame, set_frame
    // Gets or sets the gfx::frame reference used by this figure. This may be a
    // null reference.
    gfx::frame * const &get_frame() const;
    void set_frame(gfx::frame *frame);

    // (func) get_matrix, set_matrix
    // Gets or sets the model matrix (m_matrix above).
    const glm::mat4 &get_matrix() const;
    void set_matrix(glm::mat4 matrix);
};

}
}
