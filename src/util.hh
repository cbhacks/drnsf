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

#include <vector>
#include <string>

namespace util {

class nocopy {
private:
	nocopy(const nocopy &) = delete;
	nocopy(nocopy &&) = delete;

	nocopy &operator =(const nocopy &) = delete;
	nocopy &operator =(nocopy &&) = delete;

protected:
	nocopy() = default;
};

std::string to_string(std::string s);
std::string to_string(long long ll);

std::string format(std::string fmt);

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

			bool operator !=(const iterator &other) const
			{
				return m_value != other.m_value;
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

template <typename T>
inline auto range_of(T &container)
{
	return range<typename T::size_type>(0,container.size() - 1);
}

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
