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

namespace drnsf {
namespace util {

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
 * util::format
 *
 * This function is the base overload for `format(std::string,...)' when there
 * are no more arguments present. See below.
 */
std::string format(std::string fmt);

/*
 * util::format
 *
 * This templated function provides string formatting. The format string uses
 * individual dollar signs ($) as placeholders for arguments. Arguments are
 * taken in successive order like %s or %d in printf but without regard to
 * type. No format specifiers are supported yet.
 *
 * Example usage: `format("Pos: $,$  Size: $,$",x,y,width,height)'
 */
template <typename T,typename... Args>
inline std::string format(std::string fmt,T t,Args... args)
{
	auto delim_pos = fmt.find('$');
	if (delim_pos == std::string::npos)
		throw 0; // FIXME

	return fmt.substr(0,delim_pos) +
		to_string(t) +
		format(fmt.substr(delim_pos + 1),std::forward<Args>(args)...);
}

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
 * The format string's format is described in `util::format' above.
 *
 * This class is also available using the _fmt user-defined literal defined in
 * `common.hh'. See that definition for more details.
 */
class fmt {
private:
	std::string m_format;

public:
	explicit fmt(std::string format) :
		m_format(format) {}

	template <typename... Args>
	std::string operator()(Args... args) const
	{
		return format(m_format,std::forward<Args>(args)...);
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
	class range_type {
	private:
		T m_start;
		T m_end;
		T m_step;

	public:
		class iterator {
			friend class range_type;

		private:
			T m_value;
			T m_step;

			explicit iterator(T value,T step) :
				m_value(value),
				m_step(step) {}

		public:
			const T &operator *() const
			{
				return m_value;
			}

			iterator &operator ++()
			{
				m_value += m_step;
				return *this;
			}

			bool operator !=(const iterator &rhs) const
			{
				return m_value != rhs.m_value;
			}
		};

		range_type(T start,T end,T step) :
			m_start(start),
			m_end(end),
			m_step(step) {}

		iterator begin() const
		{
			return iterator(m_start,m_step);
		}

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
	const unsigned char *m_data;
	std::size_t m_size;

	unsigned char m_bitbuf;
	int m_bitbuf_len;

public:
	binreader();

	void begin(const unsigned char *data,std::size_t size);
	void begin(const std::vector<unsigned char> &data);

	void end();
	void end_early();

	std::uint8_t read_u8();
	std::uint16_t read_u16();
	std::uint32_t read_u32();

	std::int64_t read_ubits(int bits);

	std::int8_t read_s8();
	std::int16_t read_s16();
	std::int32_t read_s32();

	std::int64_t read_sbits(int bits);

	void expect_u8(std::uint8_t value);
	void expect_u16(std::uint16_t value);
	void expect_u32(std::uint32_t value);

	void expect_ubits(int bits,std::int64_t value);

	void expect_s8(std::int8_t value);
	void expect_s16(std::int16_t value);
	void expect_s32(std::int32_t value);

	void expect_sbits(int bits,std::int64_t value);

	void discard(int bytes);
	void discard_bits(int bits);
};

}
}
