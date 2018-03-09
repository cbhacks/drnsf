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
 * gl.hh
 *
 * FIXME explain
 */

#include <epoxy/gl.h>
#include <glm/glm.hpp>
#include <unordered_set>

namespace drnsf {
namespace gl {

/*
 * gl::init
 *
 * FIXME explain
 */
void init();

/*
 * gl::shutdown
 *
 * Resets all of the gl::object handles (see below), releases the GL context,
 * and closes any background windows or other resources which were in use by
 * gl::init.
 *
 * This function must not be called unless gl::init has been called previously
 * with no error (no exceptions thrown).
 */
void shutdown();

/*
 * gl::any_object
 *
 * FIXME explain
 */
class any_object : private util::nocopy {
private:
    // (s-func) get_all_objects
    // FIXME explain
    static std::unordered_set<any_object *> &get_all_objects();

public:
    // (s-func) reset_all
    // FIXME explain
    static void reset_all();

    // (default ctor)
    // FIXME explain
    any_object()
    {
        get_all_objects().insert(this);
    }

    // (dtor)
    // FIXME explain
    virtual ~any_object()
    {
        get_all_objects().erase(this);
    }

    // (pure func) reset
    // FIXME explain
    virtual void reset() = 0;
};

/*
 * gl::object
 *
 * FIXME explain
 */
template <typename traits>
class object : any_object {
private:
    // (var) m_id
    // FIXME explain
    typename traits::type m_id = traits::null_value;

    // (var) m_dirty
    // This flag is intended to be set when the object needs to be updated, for
    // instance (for buffers and textures) if the object's contents are out of
    // date. An object also starts out as dirty, which can be used for objects
    // such as shaders to indicate that they need to be uploaded and compiled.
    //
    // This flag has no internally meaningful status, and is only intended for
    // use by code which uses these objects. This flag can be read by ok() and
    // set using set_ok(). Any call to reset() will also set this flag, such as
    // in the case of GL context invalidation or context loss (however rare).
    bool m_dirty = true;

public:
    // (default ctor)
    // FIXME explain
    object() = default;

    // (move ctor)
    // FIXME explain
    object(object &&src)
    {
        using std::swap;
        swap(m_id, src.m_id);
        swap(m_dirty, src.m_dirty);
    }

    // (dtor)
    // FIXME explain
    ~object()
    {
        if (m_id != traits::null_value) {
            traits::destroy(m_id);
        }
    }

    // (func) reset
    // FIXME explain
    void reset() final override
    {
        if (m_id != traits::null_value) {
            traits::destroy(m_id);
            m_id = traits::null_value;
        }
        m_dirty = true;
    }

    // (conversion operator)
    // FIXME explain
    operator typename traits::type() &
    {
        if (m_id == traits::null_value) {
            traits::create(m_id);
        }
        return m_id;
    }

    // (func) ok
    // Returns true if the object is not marked as dirty. See m_dirty for more
    // details.
    bool ok() const
    {
        return !m_dirty;
    }

    // (func) set_ok
    // Sets the dirty status. True will clear the flag, marking the object as
    // ready for use, false will set the flag marking it as dirty. See m_dirty
    // for more details.
    void set_ok(bool ok = true)
    {
        m_dirty = !ok;
    }
};

/*
 * gl::base_traits
 *
 * FIXME explain
 */
struct base_traits {
    using type = GLuint;
    constexpr static type null_value = 0;
};

/*
 * gl::texture
 * gl::texture_traits
 *
 * FIXME explain
 */
struct texture_traits : base_traits {
    static void create(type &v)
    {
        glGenTextures(1, &v);
    }
    static void destroy(type v)
    {
        glDeleteTextures(1, &v);
    }
};
using texture = object<texture_traits>;

/*
 * gl::buffer
 * gl::buffer_traits
 *
 * FIXME explain
 */
struct buffer_traits : base_traits {
    static void create(type &v)
    {
        glGenBuffers(1, &v);
    }
    static void destroy(type v)
    {
        glDeleteBuffers(1, &v);
    }
};
using buffer = object<buffer_traits>;

/*
 * gl::vert_array
 * gl::vert_array_traits
 *
 * FIXME explain
 */
struct vert_array_traits : base_traits {
    static void create(type &v)
    {
        glGenVertexArrays(1, &v);
    }
    static void destroy(type v)
    {
        glDeleteVertexArrays(1, &v);
    }
};
using vert_array = object<vert_array_traits>;

/*
 * gl::renderbuffer
 * gl::renderbuffer_traits
 *
 * FIXME explain
 */
struct renderbuffer_traits : base_traits {
    static void create(type &v)
    {
        glGenRenderbuffers(1, &v);
    }
    static void destroy(type v)
    {
        glDeleteRenderbuffers(1, &v);
    }
};
using renderbuffer = object<renderbuffer_traits>;

/*
 * gl::framebuffer
 * gl::framebuffer_traits
 *
 * FIXME explain
 */
struct framebuffer_traits : base_traits {
    static void create(type &v)
    {
        glGenFramebuffers(1, &v);
    }
    static void destroy(type v)
    {
        glDeleteFramebuffers(1, &v);
    }
};
using framebuffer = object<framebuffer_traits>;

/*
 * gl::program
 * gl::program_traits
 *
 * FIXME explain
 */
struct program_traits : base_traits {
    static void create(type &v)
    {
        v = glCreateProgram();
    }
    static void destroy(type v)
    {
        glDeleteProgram(v);
    }
};
using program = object<program_traits>;

/*
 * gl::shader
 * gl::shader_traits
 * gl::vert_shader
 * gl::frag_shader
 *
 * FIXME explain
 */
template <int ShaderType>
struct shader_traits : base_traits {
    static void create(type &v)
    {
        v = glCreateShader(ShaderType);
    }
    static void destroy(type v)
    {
        glDeleteShader(v);
    }
};
template <int ShaderType>
using shader = object<shader_traits<ShaderType>>;
using vert_shader = shader<GL_VERTEX_SHADER>;
using frag_shader = shader<GL_FRAGMENT_SHADER>;

/*
 * gl::compile_shader
 *
 * FIXME explain
 */
template <int ShaderType>
inline void compile_shader(shader<ShaderType> &sh, const std::string &code)
{
    const char *code_cstr = code.c_str();
    glShaderSource(sh, 1, &code_cstr, nullptr);
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

        throw std::runtime_error("gl::compile_shader: compile failed");
    }
}
template <int ShaderType>
inline void compile_shader(
    shader<ShaderType> &sh,
    const void *code,
    size_t size)
{
    compile_shader(sh, {reinterpret_cast<const char *>(code), size});
}

}
}
