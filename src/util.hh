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

namespace drnsf {
namespace util {

/*
 * util::byte
 *
 * This is an alias for `unsigned char', but could be replaced with `std::byte'
 * in future versions.
 */
using byte = unsigned char;

/*
 * util::blob
 *
 * This is an alias for a byte vector. This is a common type, used to represent
 * raw data, raw items, etc.
 */
using blob = std::vector<byte>;

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
 * util::event
 *
 * FIXME: explain
 */
template <typename... Args>
class event : private nocopy {
public:
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
				throw 0;//FIXME
			}

			m_func = func;
		}

		// (func) bind
		// Binds the watch to an event. The watch must not be currently
		// bound to an event, and it must already have a function set
		// (see operator <<=).
		void bind(event &ev)
		{
			if (!m_func) {
				throw 0;//FIXME
			}

			if (m_event) {
				throw 0;//FIXME
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
				throw 0;//FIXME
			}

			m_hold.splice(
				m_hold.end(),
				m_event->m_watchers,
				m_iter
			);
			m_event = nullptr;
		}
	};

private:
	// (var) m_watchers
	// A list of pointers to the watches bound to this event.
	std::list<watch *> m_watchers;

public:
	// (call operator)
	// Raises the event with the specified arguments. This means triggering
	// all of the watches bound to this event.
	void operator ()(Args... args)
	{
		for (auto &&watcher : m_watchers) {
			watcher->m_func(args...);
		}
	}
};

/*
 * util::to_string
 *
 * These functions provide a mechanism for converting some known common types
 * to strings. Any code using these should consider allowing for ADL using the
 * same idiom as the `using std::swap; swap(x,y);' concept.
 *
 * This code actually already exists in the standard library, so it should be
 * removed from here. [ FIXME ]
 */
std::string to_string(std::string s);
std::string to_string(long long ll);

/*
 * util::fmt
 *
 * An instance of this class represents a "format string", similar to as used
 * in C `printf'. Format strings can be constructed from standard strings using
 * the explicit constructor provided in this class.
 *
 * A format string object is used by applying the arguments to it using the call
 * operator (as one would call a function, `f(a,b,c)') which yields the string
 * produced by the formatting. The format string is not altered or destroyed by
 * this operation, and may be reused as many times as desired.
 *
 * The format string's format is any sequence of text, with dollar signs ($) as
 * placeholders for arguments taken in successive order like %s or %d in printf
 * but without regard to type. No format specifiers (e.g. %05d) are supported
 * yet.
 *
 * Example: `fmt("pos: $,$  size: $,$")(x,y,width,height)'
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
	template <typename T,typename... Args>
	std::string operator()(T t,Args... args) const
	{
		auto delim_pos = m_format.find('$');
		if (delim_pos == std::string::npos)
			throw 0; // FIXME

		return m_format.substr(0,delim_pos)
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
			throw 0; // FIXME

		return m_format;
	}
};

/*
 * util::range
 *
 * This function returns an instance of an object which represents an iterable
 * range of values, generally intended for use in for-range loops. This
 * construct works like this:
 *
 * `for (auto x : range(1,9,2)) { ... }'
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
inline auto range(T lbound,T ubound,T step = 1)
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
			explicit iterator(T value,T step) :
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
		range_type(T start,T end,T step) :
			m_start(start),
			m_end(end),
			m_step(step) {}

		// (func) begin
		// FIXME explain
		iterator begin() const
		{
			return iterator(m_start,m_step);
		}

		// (func) end
		// FIXME explain
		iterator end() const
		{
			return iterator(m_end + m_step,m_step);
		}
	};

	return range_type(lbound,ubound,step);
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
	return range<typename T::size_type>(0,container.size() - 1);
}

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
private:
	// (var) m_data
	// FIXME explain
	const unsigned char *m_data;

	// (var) m_size
	// FIXME explain
	std::size_t m_size;

	// (var) m_bitbuf
	// FIXME explain
	unsigned char m_bitbuf;

	// (var) m_bitbuf_len
	// FIXME explain
	int m_bitbuf_len;

public:
	// (default ctor)
	// FIXME explain
	binreader();

	// (func) begin
	// FIXME explain
	void begin(const unsigned char *data,std::size_t size);

	// (func) begin
	// FIXME explain
	void begin(const util::blob &data);

	// (func) end
	// FIXME explain
	void end();

	// (func) end_early
	// FIXME explain
	void end_early();

	// (func) read_u8
	// FIXME explain
	std::uint8_t read_u8();

	// (func) read_u16
	// FIXME explain
	std::uint16_t read_u16();

	// (func) read_u32
	// FIXME explain
	std::uint32_t read_u32();

	// (func) read_ubits
	// FIXME explain
	std::int64_t read_ubits(int bits);

	// (func) read_s8
	// FIXME explain
	std::int8_t read_s8();

	// (func) read_s16
	// FIXME explain
	std::int16_t read_s16();

	// (func) read_s32
	// FIXME explain
	std::int32_t read_s32();

	// (func) read_sbits
	// FIXME explain
	std::int64_t read_sbits(int bits);

	// (func) expect_u8
	// FIXME explain
	void expect_u8(std::uint8_t value);

	// (func) expect_u16
	// FIXME explain
	void expect_u16(std::uint16_t value);

	// (func) expect_u32
	// FIXME explain
	void expect_u32(std::uint32_t value);

	// (func) expect_ubits
	// FIXME explain
	void expect_ubits(int bits,std::int64_t value);

	// (func) expect_u8
	// FIXME explain
	void expect_s8(std::int8_t value);

	// (func) expect_s16
	// FIXME explain
	void expect_s16(std::int16_t value);

	// (func) expect_s32
	// FIXME explain
	void expect_s32(std::int32_t value);

	// (func) expect_sbits
	// FIXME explain
	void expect_sbits(int bits,std::int64_t value);

	// (func) discard
	// FIXME explain
	void discard(int bytes);

	// (func) discard_bits
	// FIXME explain
	void discard_bits(int bits);
};

}
}
