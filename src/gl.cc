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

namespace impl {

// (func) init
// FIXME explain
void init();

// (func) shutdown
// FIXME explain
void shutdown();

}

// (s-var) s_init_count
// The number of times GL has been initialized. This is increased by calls to
// `init' and decreased by calls to `shutdown'.
static int s_init_count = 0;

// declared in gl.hh
void init()
{
    // Exit early if GL is already initialized.
    if (s_init_count) {
        s_init_count++;
        return;
    }

    impl::init();
    s_init_count++;
}

// declared in gl.hh
void shutdown() noexcept
{
    // Exit immediately with a warning if GL was never initialized.
    if (s_init_count <= 0) {
        // TODO - log warning
        return;
    }

    // Drop the initialization count by one if we are multiple inits deep.
    if (s_init_count > 1) {
        s_init_count--;
        return;
    }

    s_init_count = 0;
    any_object::reset_all();

    impl::shutdown();
}

// declared in gl.hh
bool is_init() noexcept
{
    return s_init_count;
}

// declared in gl.hh
std::unordered_set<any_object *> &any_object::get_all_objects()
{
    static std::unordered_set<any_object *> s_all_objects;
    return s_all_objects;
}

// declared in gl.hh
void any_object::reset_all()
{
    for (auto &&p : get_all_objects()) {
        p->reset();
    }
}

// declared in gl.hh
void link_program(unsigned int prog)
{
    glLinkProgram(prog);

    int status;
    glGetProgramiv(prog, GL_LINK_STATUS, &status);

    if (!status) {
        int log_size;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &log_size);

        std::vector<char> log_buffer(log_size);
        glGetProgramInfoLog(
            prog,
            log_size,
            nullptr,
            log_buffer.data()
        );

        fprintf(stderr, " == BEGIN PROGRAM LINKER LOG ==\n");
        fprintf(stderr, "%s\n", log_buffer.data());
        fprintf(stderr, " === END PROGRAM LINKER LOG ===\n");

        throw error("gl::link_program: link failed");
    }
}

// declared in gl.hh
void shader_source(
    unsigned int sh,
    std::initializer_list<std::string_view> sources)
{
    std::vector<const char *>  source_ptrs;
    std::vector<int>           source_lens;

    for (const auto &source : sources) {
        if (source.size() > INT_MAX) {
            throw gl::error("gl::shader_source: source file too large");
        }

        source_ptrs.push_back(source.data());
        source_lens.push_back(source.size());
    }

    glShaderSource(sh, sources.size(), source_ptrs.data(), source_lens.data());
}

// declared in gl.hh
void compile_shader(unsigned int sh)
{
    glCompileShader(sh);

    int status;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &status);

    if (!status) {
        int log_size;
        glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &log_size);

        std::vector<char> log_buffer(log_size);
        glGetShaderInfoLog(
            sh,
            log_size,
            nullptr,
            log_buffer.data()
        );

        fprintf(stderr, " == BEGIN SHADER COMPILE LOG ==\n");
        fprintf(stderr, "%s\n", log_buffer.data());
        fprintf(stderr, " === END SHADER COMPILE LOG ===\n");

        throw error("gl::compile_shader: compile failed");
    }
}

}
}
