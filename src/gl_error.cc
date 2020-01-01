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
#include "gl.hh"

namespace drnsf {
namespace gl {

// declared in gl.hh
void error::collect()
{
    // Check if GL is active.
    if (!is_init()) {
        return;
    }
    m_has_details = true;

    // Collect details.
    m_vendor       = reinterpret_cast<const char *>(glGetString(GL_VENDOR));
    m_renderer     = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
    m_version      = reinterpret_cast<const char *>(glGetString(GL_VERSION));
    m_glsl_version = reinterpret_cast<const char *>(
        glGetString(GL_SHADING_LANGUAGE_VERSION)
    );

    // Check if GL_EXTENSIONS is supported.
    if (epoxy_gl_version() < 30) {
        return;
    }
    m_has_extensions = true;

    // Build the extension list.
    int ext_count;
    glGetIntegerv(GL_NUM_EXTENSIONS, &ext_count);
    m_extensions.resize(ext_count);
    for (int i = 0; i < ext_count; i++) {
        m_extensions[i] = reinterpret_cast<const char *>(
            glGetStringi(GL_EXTENSIONS, i)
        );
    }
}

// declared in gl.hh
error::error(const std::string &what_arg) :
    runtime_error(what_arg)
{
    collect();
}

// declared in gl.hh
error::error(const char *what_arg) :
    runtime_error(what_arg)
{
    collect();
}

// declared in gl.hh
bool error::has_details() noexcept
{
    return m_has_details;
}

// declared in gl.hh
const std::string &error::gl_vendor()
{
    if (!m_has_details) {
        throw std::logic_error("gl::error: no details");
    }
    return m_vendor;
}

// declared in gl.hh
const std::string &error::gl_renderer()
{
    if (!m_has_details) {
        throw std::logic_error("gl::error: no details");
    }
    return m_renderer;
}

// declared in gl.hh
const std::string &error::gl_version()
{
    if (!m_has_details) {
        throw std::logic_error("gl::error: no details");
    }
    return m_version;
}

// declared in gl.hh
const std::string &error::gl_glsl_version()
{
    if (!m_has_details) {
        throw std::logic_error("gl::error: no details");
    }
    return m_glsl_version;
}

// declared in gl.hh
bool error::has_extensions() noexcept
{
    return m_has_extensions;
}

// declared in gl.hh
const std::vector<std::string> &error::gl_extensions()
{
    if (!m_has_extensions) {
        throw std::logic_error("gl::error: no extensions");
    }
    return m_extensions;
}

// declared in gl.hh
void error::dump(std::ostream &out)
{
    if (m_has_details) {
        out
            << "========================================\n"
            << "  OpenGL Information\n"
            << "\n"
            << "VENDOR:   " << m_vendor << "\n"
            << "\n"
            << "RENDERER: " << m_renderer << "\n"
            << "\n"
            << "VERSION:  " << m_version << "\n"
            << "\n"
            << "GLSL VER: " << m_glsl_version << "\n"
            << "\n"
            << "Extensions:\n";
            if (m_has_extensions) {
                for (auto &&ext : m_extensions) {
                    out << "+ " << ext << "\n";
                }
            } else {
                out << "< NO INFORMATION >\n";
            }
        out
            << "========================================"
            << std::endl;
    } else {
        out
            << "========================================\n"
            << "  OpenGL Information\n"
            << "\n"
            << "< NO INFORMATION > \n"
            << "========================================"
            << std::endl;
    }
}

}
}
