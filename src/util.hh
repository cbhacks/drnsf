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

#pragma once

/*
 * util.hh
 *
 * This header file provides, under the `util' namespace, numerous types and
 * functions which may be useful for general programming. This code is intended
 * to fill in any "gaps" or "missing features" from the standard library, such
 * as less-terrible string formatting.
 */

#include <vector>
#include <string>
#include <list>
#include <cstring>

namespace drnsf {
namespace util {

/*
 * util::blob
 *
 * This is an alias for a byte vector. This is a common type, used to represent
 * raw data, raw items, etc.
 */
using blob = std::vector<uint8_t>;

/*
 * util::nocopy
 *
 * This is a base class for any type which should not be copyable or movable.
 * By default, a type in C++ gets an implicitly defined copy constructor, move
 * constructor, copy assignment operator, and move assignment operator. This
 * type removes those default behaviors by defining them as deleted.
 *
 * Very common base class. Does not provide any behavior.
 *
 * Inherit as private.
 */
class nocopy {
private:
    nocopy(const nocopy &) = delete;
    nocopy(nocopy &&) = delete;

    nocopy &operator =(const nocopy &) = delete;
    nocopy &operator =(nocopy &&) = delete;

protected:
    nocopy() = default;
};

/*
 * util::polymorphic
 *
 * FIXME explain
 */
class polymorphic {
public:
    virtual ~polymorphic() = default;
};

/*
 * util::event
 *
 * FIXME: explain
 */
template <typename... Args>
class event : private nocopy {
public:
    // (inner class) watch
    // FIXME explain
    class watch : private nocopy {
        friend class event;

    private:
        // (var) m_event
        // A pointer to the event this watch is bound to, or null if
        // not bound.
        event *m_event;

        // (var) m_hold
        // A list for holding the bound event's pointer to this watch
        // object. Rather than inserting or removing to the event's
        // list (which could throw on out-of-memory), we can splice
        // the entry around (which is noexcept; FIXME is it?).
        std::list<watch *> m_hold;

        // (var) m_iter
        // FIXME explain
        typename std::list<watch *>::iterator m_iter;

        // (var) m_func
        // The function attached to this event. This could be a "null"
        // function if it has not been set yet; however, a watch may
        // not be bound to an event until then.
        std::function<void(Args...)> m_func;

    public:
        // (default ctor)
        // Constructs a watch with no set function and not bound to any
        // event.
        watch() :
            m_event(nullptr)
        {
            m_hold.push_front(this);
            m_iter = m_hold.begin();
        }

        // (dtor)
        // Destroys the watch. If the watch is currently bound to an
        // event, it is unbound.
        ~watch()
        {
            if (m_event) {
                unbind();
            }
        }

        // (feed operator)
        // Sets the function associated with this watch. It is an error
        // to set the function if one is already set.
        void operator <<=(std::function<void(Args...)> func)
        {
            if (m_func) {
                throw std::logic_error("util::watch::(shift op): bad func");
            }

            m_func = func;
        }

        // (call operator)
        // Calls the function associated with this watch. There must already be
        // a function set on the watch.
        void operator ()(Args... args)
        {
            if (!m_func) {
                throw std::logic_error("util::watch::(call op): no func");
            }

            m_func(std::forward<Args>(args)...);
        }

        // (func) bind
        // Binds the watch to an event. The watch must not be currently
        // bound to an event, and it must already have a function set
        // (see operator <<=).
        void bind(event &ev)
        {
            if (!m_func) {
                throw std::logic_error("util::watch::bind: no function yet");
            }

            if (m_event) {
                throw std::logic_error("util::watch::bind: already bound");
            }

            m_event = &ev;
            m_event->m_watchers.splice(
                m_event->m_watchers.end(),
                m_hold,
                m_iter
            );
        }

        // (func) unbind
        // Unbinds the watch from its currently bound event. This may
        // not be called while the watch is not bound.
        void unbind()
        {
            if (!m_event) {
                throw std::logic_error("util::watch::unbind: not bound");
            }

            m_hold.splice(
                m_hold.end(),
                m_event->m_watchers,
                m_iter
            );
            m_event = nullptr;
        }

        // (func) is_bound
        // Returns true if the watch is currently bound to an event, or false
        // otherwise.
        bool is_bound() const
        {
            return bool(m_event);
        }
    };

private:
    // (var) m_watchers
    // A list of pointers to the watches bound to this event.
    std::list<watch *> m_watchers;

    // (var) m_direct_handler
    // A direct handler function which also responds to this event. This may be
    // null if no direct handler has been installed.
    std::function<void(Args...)> m_direct_handler;

public:
    // (feed operator)
    // Sets the direct handler function for this event. This may be done only
    // once for the lifetime of the event. The given function must be valid.
    void operator <<=(std::function<void(Args...)> func)
    {
        if (m_direct_handler) {
            throw std::logic_error("util::event::(shift op): already set");
        }

        m_direct_handler = std::move(func);
    }

    // (call operator)
    // Raises the event with the specified arguments. This means triggering
    // all of the watches bound to this event.
    void operator ()(Args... args)
    {
        if (m_direct_handler) {
            m_direct_handler(args...);
        }
        for (auto &&watcher : m_watchers) {
            watcher->m_func(args...);
        }
    }
};

/*
 * util::to_string
 *
 * These functions provide a conversion from various types to std::string. When
 * using `to_string', do so in an ADL-enabling manner similar to std::swap:
 *
 *   using util::to_string;
 *   auto s = to_string(foo);
 *
 * You can make a `to_string' for any new type by defining such a function
 * within the same namespace as the new type. The above usage pattern enables
 * other code to find your `to_string' function.
 *
 * If the type to be implemented is pre-existing, such as a primitive type or
 * external library (e.g. standard library) type, place the to_string function
 * here instead.
 */
using std::to_string;
std::string to_string(char c);
std::string to_string(std::string s);

/*
 * util::fmt
 *
 * An instance of this class represents a "format string", similar to as used
 * in C `printf'. Format strings can be constructed from standard strings using
 * the explicit constructor provided in this class.
 *
 * A format string object is used by applying the arguments to it using the call
 * operator (as one would call a function, `f(a, b, c)') which yields the string
 * produced by the formatting. The format string is not altered or destroyed by
 * this operation, and may be reused as many times as desired.
 *
 * The format string's format is any sequence of text, with dollar signs ($) as
 * placeholders for arguments taken in successive order like %s or %d in printf
 * but without regard to type. No format specifiers (e.g. %05d) are supported
 * yet.
 *
 * Example: `fmt("pos: $,$  size: $,$")(x, y, width, height)'
 *
 * This class is also available using the _fmt user-defined literal defined in
 * `common.hh'. See that definition for more details.
 */
class fmt {
private:
    // (var) m_format
    // FIXME explain
    std::string m_format;

public:
    // (explicit ctor)
    // Constructs the fmt object with the specified format string.
    explicit fmt(std::string format) :
        m_format(format) {}

    // (call operator)
    // FIXME explain
    template <typename T, typename... Args>
    std::string operator()(T t, Args... args) const
    {
        auto delim_pos = m_format.find('$');
        if (delim_pos == std::string::npos)
            throw std::logic_error("util::fmt::(call op): too many params");

        return m_format.substr(0, delim_pos)
            + to_string(t)
            + fmt(m_format.substr(delim_pos + 1))
                (std::forward<Args>(args)...);
    }

    // (call operator)
    // FIXME explain
    std::string operator()() const
    {
        auto delim_pos = m_format.find('$');
        if (delim_pos != std::string::npos)
            throw std::logic_error("util::fmt::(call op): not enough params");

        return m_format;
    }
};

/*
 * util::dynamic_call
 *
 * This function takes a functor `f', a pointer `arg', and a list of types. For
 * the first type in the list which `arg' can successfully be dynamic_cast to,
 * `f' is called with a pointer to that type, and dynamic_call returns true. If
 * no matching type is found, dynamic_call returns false.
 */
template <
    typename F,
    typename Arg,
    typename HeadType,
    typename... TailTypes>
bool dynamic_call(F &&f, Arg *arg)
{
    auto *ptr = dynamic_cast<HeadType *>(arg);
    if (ptr) {
        f(ptr);
        return true;
    } else {
        return dynamic_call<F, Arg, TailTypes...>(std::forward<F>(f), arg);
    }
}
template <typename F, typename Arg>
bool dynamic_call(F &&f, Arg *arg)
{
    return false;
}

/*
 * util::range
 *
 * This function returns an instance of an object which represents an iterable
 * range of values, generally intended for use in for-range loops. This
 * construct works like this:
 *
 * `for (auto x : range(1, 9, 2)) { ... }'
 *
 * Which would work the same as the common BASIC language for loop:
 *
 * `FOR x = 1 TO 9 STEP 2'
 *
 * This loop would yield the values 1, 3, 5, 7, and 9 in order.
 *
 * The concept is to define an iterable object which returns the values from the
 * lower bound to the upper bound, inclusive, in whatever increments desired (by
 * default this is 1). Note that if the lower bound is greater than the upper
 * bound, in most such cases you should provide a negative step value.
 *
 * range<T> is generic, and could be used presumably for any type which properly
 * implements the operators `!=', `+', `+=', and is copyable.
 *
 * FIXME: Stepping over the upper range will actually not end the loop, as `end'
 * returns `ubound + step' which will only match exact hits.
 */
template <typename T>
inline auto range(T lbound, T ubound, T step = 1)
{
    // (inner class) range_type
    // FIXME explain
    class range_type {
    private:
        // (var) m_start
        // FIXME explain
        T m_start;

        // (var) m_end
        // FIXME explain
        T m_end;

        // (var) m_step
        // FIXME explain
        T m_step;

    public:
        // (inner class) iterator
        // FIXME explain
        class iterator {
            friend class range_type;

        private:
            // (var) m_value
            // FIXME explain
            T m_value;

            // (var) m_step
            // FIXME explain
            T m_step;

            // (explicit ctor)
            // FIXME explain
            explicit iterator(T value, T step) :
                m_value(value),
                m_step(step) {}

        public:
            // (dereference operator)
            // FIXME explain
            const T &operator *() const
            {
                return m_value;
            }

            // (prefix increment operator)
            // FIXME explain
            iterator &operator ++()
            {
                m_value += m_step;
                return *this;
            }

            // (inequality operator)
            // FIXME explain
            bool operator !=(const iterator &rhs) const
            {
                return m_value != rhs.m_value;
            }
        };

        // (ctor)
        // FIXME explain
        range_type(T start, T end, T step) :
            m_start(start),
            m_end(end),
            m_step(step) {}

        // (func) begin
        // FIXME explain
        iterator begin() const
        {
            return iterator(m_start, m_step);
        }

        // (func) end
        // FIXME explain
        iterator end() const
        {
            return iterator(m_end + m_step, m_step);
        }
    };

    return range_type(lbound, ubound, step);
}

/*
 * util::range_of
 *
 * This function returns a range type, similar to `util::range' described above,
 * but which uses a range matching the range of a given container. This is most
 * useful in for-range loops over `std::vector' types where one wishes to get
 * the index rather than a reference to the element.
 */
template <typename T>
inline auto range_of(T &container)
{
    return range<typename T::size_type>(0, container.size() - 1);
}

/*
 * util::reverse_of
 *
 * FIXME explain
 */
template <typename T>
inline auto reverse_of(T &container)
{
    // (inner class) reverse_type
    // FIXME explain
    class reverse_type {
    private:
        // (var) m_container
        // FIXME explain
        T &m_container;

    public:
        // (ctor)
        // FIXME explain
        reverse_type(T &container) :
            m_container(container) {}

        // (func) begin
        // FIXME explain
        auto begin() {
            return m_container.rbegin();
        }

        // (func) end
        // FIXME explain
        auto end() {
            return m_container.rend();
        }
    };

    return reverse_type(container);
}

class binwriter;

/*
 * util::readdir
 *
 * FIXME explain
*/ 
enum class read_dir {
    rtl,
    ltr
};

/*
 * util::binreader
 *
 * This class provides a mechanism for reading and parsing binary data.
 *
 * Usage of this class goes as follows:
 *
 * 1. `begin' is called on an instance of `binreader'. This binds the reader to
 * a particular block of bytes, which it will read from.
 *
 * 2. Various methods on the binreader are called, such as `read_u8', `read_s32'
 * or `read_ubits(n)'. The binreader automatically range-checks these against
 * the size given to `begin'.
 *
 * 3. The binreader is closed by calling `end'. This will throw an exception if
 * the entire input data has not been read completely. Alternatively, the method
 * `end_early' may be called if this is an expected case.
 *
 * 4. At this point, the object may be reused by repeating from step 1 if
 * desired.
 *
 * When reading, additional methods are available, such as `discard' for reading
 * data but discarding it, and various `expect_' methods for reading data and
 * throwing an exception if the data does not match the given value.
 *
 * When reading bit values, one must not call a non-bit read/expect/discard
 * method while part-way into a given byte of data. This use case is not
 * supported, and an exception will be thrown.
 */
class binreader : private nocopy {
    friend class binwriter;

private:
    // (var) m_data
    // FIXME explain
    const unsigned char *m_data;

    // (var) m_size
    // FIXME explain
    size_t m_size;

    // (var) m_data_base
    // FIXME explain
    const unsigned char* m_data_base;

    // (var) m_bitbuf
    // FIXME explain
    unsigned char m_bitbuf;

    // (var) m_bitbuf_len
    // FIXME explain
    int m_bitbuf_len;

    // (var) m_read_dir
    // Direction in which to read bits
    read_dir m_read_dir;

public:
    // (default ctor)
    // FIXME explain
    binreader(const read_dir read_dir = read_dir::rtl);

    // (func) begin
    // FIXME explain
    void begin(const unsigned char *data, size_t size);

    // (func) begin
    // FIXME explain
    void begin(const util::blob &data);

    // (func) begin
    // Bind the reader at offset bytes from the start or end of the block bound by 
    // the given writer. 
    void begin(const util::binwriter &writer, int offset);

    // (func) end
    // FIXME explain
    void end();

    // (func) end_early
    // FIXME explain
    void end_early();

    // (func) read_u8
    // FIXME explain
    uint8_t read_u8();

    // (func) read_u16
    // FIXME explain
    uint16_t read_u16();

    // (func) read_u32
    // FIXME explain
    uint32_t read_u32();

    // (func) read_ubits
    // FIXME explain
    int64_t read_ubits(int bits);

    // (func) read_s8
    // FIXME explain
    int8_t read_s8();

    // (func) read_s16
    // FIXME explain
    int16_t read_s16();

    // (func) read_s32
    // FIXME explain
    int32_t read_s32();

    // (func) read_sbits
    // FIXME explain
    int64_t read_sbits(int bits);

    // (func) read_bytes
    // Reads a given number of bytes and returns a vector of them.
    blob read_bytes(int bytes);

    // (func) discard
    // FIXME explain
    void discard(int bytes);

    // (func) discard_bits
    // FIXME explain
    void discard_bits(int bits);
};

/*
 * util::binwriter
 *
 * This class provides a mechanism for writing binary data.
 *
 * FIXME explain
 */
class binwriter : private nocopy {
    friend class binreader;

private:
    // (var) m_active
    // FIXME explain
    bool m_active;

    // (var) m_data
    // FIXME explain
    util::blob m_data;

    // (var) m_bitbuf
    // FIXME explain
    unsigned char m_bitbuf;

    // (var) m_bitbuf_len
    // FIXME explain
    int m_bitbuf_len;

public:
    // (default ctor)
    // FIXME explain
    binwriter();

    // (func) begin
    // FIXME explain
    void begin();

    // (func) end
    // FIXME explain
    util::blob end();

    // (func) write_u8
    // fixme explain
    void write_u8(uint8_t value);

    // (func) write_u16
    // fixme explain
    void write_u16(uint16_t value);

    // (func) write_u32
    // fixme explain
    void write_u32(uint32_t value);

    // (func) write_ubits
    // fixme explain
    void write_ubits(int bits, int64_t value);

    // (func) write_s8
    // fixme explain
    void write_s8(int8_t value);

    // (func) write_s16
    // fixme explain
    void write_s16(int16_t value);

    // (func) write_s32
    // fixme explain
    void write_s32(int32_t value);

    // (func) write_sbits
    // fixme explain
    void write_sbits(int bits, int64_t value);

    // (func) write_bytes
    // Writes the given blob to the output buffer.
    void write_bytes(util::blob data);

    // (func) pad
    // Writes zero-value bytes to the output buffer until the size of the buffer
    // is a multiple of the specified alignment. If the buffer size already
    // meets the given alignment requirements, no change occurs.
    void pad(int alignment);

    // (func) length
    // fixme explain
    int length() { return m_data.size(); }
};

/*
 * util::on_exit_helper
 *
 * This type is provided for the DRNSF_ON_EXIT macro defined in common.hh.
 *
 * Generally, avoid using this type directly. Use DRNSF_ON_EXIT instead. Using
 * this type directly is done by giving it a functor like so:
 *
 *   auto R = util::on_exit_helper % my_functor;
 *
 * When R is destroyed, the functor is invoked with no parameters.
 */
static struct {} on_exit_helper;
template <typename T>
auto operator %(decltype(on_exit_helper), T &&t)
{
    struct handler {
        bool live = true;
        T t;

        handler(T t) : t(std::move(t)) {}

        handler(handler &&src) :
            live(src.live),
            t(std::move(src.t))
        {
            // MSVC workaround; no move elision yet
            src.live = false;
        }

        ~handler() { if (live) { t(); } }
    };
    return handler{std::forward<T>(t)};
}

#ifdef _WIN32
/*
 * util::u8str_to_wstr
 * util::wstr_to_u8str
 *
 * Provided on Windows only, these functions provide conversion between UTF-8
 * strings and wchar_t strings. This is necessary because many Windows API
 * functions require wide strings ('W' functions). Using non-unicode functions
 * ('A' functions) is highly discouraged as it results in problems for many
 * non-english texts and filenames.
 */
std::wstring u8str_to_wstr(std::string u8str);
std::string wstr_to_u8str(std::wstring wstr);
#endif

/*
 * util::file
 *
 * This class provides a C++-style interface around the standard C `FILE' API.
 * Primarily, this is intended to be used for binary files, as it is not usable
 * with `fprintf', `fputs', `fgets', etc.
 *
 * When initially constructed, the file object is in a closed state. To use the
 * object, call `open' with a filename and mode ("rb", "wb", "rb+", etc). The
 * file is closed when the object is destroyed or when `close' is called.
 */
class file : private util::nocopy {
private:
    // (var) m_handle
    // The internal C API file handle. This is null if no file is open.
    FILE *m_handle = nullptr;

public:
    // (default ctor)
    // Constructs the object in a "closed" state.
    file() = default;

    // (dtor)
    // Closes the file if it is open.
    ~file();

    // (func) open
    // Based on `std::fopen'. Opens the given filename with the specified mode
    // and associates that file handle with this object. If an error occurs, an
    // exception is thrown and the object is not modified.
    //
    // If the file is already open, an exception is thrown.
    void open(const std::string &path, const char *mode);

    // (func) close
    // Closes the currently open file. If the file is not currently open, an
    // exception is thrown.
    void close();

    // (func) read
    // Based on `std::fread'. Reads data from the currently open file.
    //
    // Returns true if the entire buffer was read successfully. Returns false
    // if no data was read because EOF was reached. If an error occurred during
    // reading (`std::ferror') or if only some bytes could be read before EOF,
    // an exception is thrown.
    //
    // If the file is not currently open, an exception is thrown.
    bool read(void *buffer, size_t len);

    // (func) write
    // Based on `std::fwrite'. Writes data to the currently open file. If an
    // error occurs, an exception is thrown.
    //
    // If the file is not currently open, an exception is thrown.
    void write(const void *buffer, size_t len);

    // (func) seek
    // Based on `std::fseek'. Moves the current read/write position in the open
    // file.
    //
    // Whence should be one of these values:
    //
    //   SEEK_SET: Offset is relative to the start of the file.
    //   SEEK_CUR: Offset is relative to the current position in the file.
    //   SEEK_END: Offset is relative to the end of the file.
    //
    // A common pattern for finding the size of an open file is to seek to the
    // end, call `tell' (see below) to find that position, then seek back to
    // the beginning.
    //
    // If an error occurs, an exception is thrown.
    //
    // If the file is not currently open, an exception is thrown.
    void seek(long offset, int whence);

    // (func) tell
    // Based on `std::ftell'. Returns the current read/write position in the
    // open file, relative to the start of the file. If an error occurs, an
    // exception is thrown.
    //
    // If the file is not currently open, an exception is thrown.
    long tell() const;
};

/*
 * util::get_time
 *
 * Returns the difference in milliseconds between the current time and some
 * unspecified origin time. A single call to this function is not meaningful,
 * but the difference in return value between two such calls may be used to
 * measure the time between them.
 *
 * This function uses a monotonic clock source, meaning it is not affected by
 * system time changes (e.g. due to NTP, DST, leap seconds, etc) and should
 * never rewind. The clock may be less precise than 1ms, however.
 *
 * Callers should be prepared for the possibility of get_time overflowing, as
 * `long' may be a 32-bit type on some implementations.
 */
long get_time();

/*
 * util::stopwatch
 *
 * FIXME explain
 */
class stopwatch : private util::nocopy {
private:
    // (var) m_last_time
    // The time measured when first constructed or at the last `lap' call.
    long m_last_time;

public:
    // (default ctor)
    // Constructs the object and begins timing.
    stopwatch();

    // (func) lap
    // Returns the number of milliseconds since the last `lap' call on this
    // object, or since the stopwatch was constructed if this is the first
    // such call.
    //
    // This function may return zero if the global time from `get_time' has
    // overflowed since the last call to `lap'.
    long lap();
};

}
}
