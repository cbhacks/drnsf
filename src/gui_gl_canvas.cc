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
#include <iostream>
#include "gui.hh"

namespace drnsf {
namespace gui {

// declared in gui.hh
void gl_canvas::draw_gl(int width, int height, gl::renderbuffer &rbo)
{
    gl::framebuffer fbo;
    gl::renderbuffer rbo_depth;

    glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
    glRenderbufferStorage(
        GL_RENDERBUFFER,
        GL_DEPTH_COMPONENT16,
        width,
        height
    );
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

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
        rbo_depth
    );

    on_render(width, height);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

// declared in gui.hh
void gl_canvas::mousemove(int x, int y)
{
    on_mousemove(x, y);
}

// declared in gui.hh
void gl_canvas::mousewheel(int delta_y)
{
    on_mousewheel(delta_y);
}

// declared in gui.hh
void gl_canvas::mousebutton(int number, bool down)
{
    on_mousebutton(number, down);
}

// declared in gui.hh
void gl_canvas::key(keycode code, bool down)
{
    on_key(int(code), down);
}

// declared in gui.hh
void gl_canvas::text(const char *str)
{
    on_text(str);
}

// declared in gui.hh
gl_canvas::gl_canvas(container &parent, layout layout) :
    widget_gl(parent, layout)
{
}

}
}
