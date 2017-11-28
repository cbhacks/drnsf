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
        glm::mat4 projection_matrix;
        glm::mat4 modelview_matrix;
        glm::mat4 matrix;
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

    // (var) m_matrix
    // The "model" matrix for this figure Together with the viewport's view
    // matrix, this forms the typical "model-view" matrix. Read about 3D
    // graphics and OpenGL for more details.
    glm::mat4 m_matrix = glm::mat4(1.0f);

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
    // (func) draw
    // Implements `figure::draw'.
    void draw(const env &e) override;

public:
    // (explicit ctor)
    // Constructs the reticle figure.
    explicit reticle_fig(viewport &vp) :
        figure(vp) {}
};

/*
 * render::meshframe_fig
 *
 * This figure draws a gfx::mesh / gfx::frame pairing. The mesh and frame are
 * specified by res::ref-style references. The figure automatically handles
 * the appearance and disappearance of assets under those names.
 */
class meshframe_fig : public figure {
private:
    // (var) m_mesh
    // The reference to the mesh used by this figure.
    gfx::mesh::ref m_mesh;

    // (var) m_frame
    // The reference to the frame used by this figure.
    gfx::frame::ref m_frame;

    // (func) draw
    // Implements `figure::draw'.
    void draw(const env &e) override;

public:
    // (explicit ctor)
    // Constructs the figure. Initially, it does not reference any mesh
    // or frame.
    explicit meshframe_fig(viewport &vp) :
        figure(vp) {}
};

}
}
