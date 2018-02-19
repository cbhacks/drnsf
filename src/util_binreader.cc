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
#include <sstream>
#include "util.hh"

namespace drnsf {
namespace util {

// declared in util.hh
binreader::binreader() :
    m_data(nullptr),
    m_size(0),
    m_bitbuf(0),
    m_bitbuf_len(0)
{
}

// declared in util.hh
void binreader::begin(const unsigned char *data, std::size_t size)
{
    if (m_data)
        throw std::logic_error("util::binreader::begin: already started");
    if (!data)
        throw std::logic_error("util::binreader::begin: null data arg");

    m_data = data;
    m_size = size;
}

// declared in util.hh
void binreader::begin(const util::blob &data)
{
    if (m_data)
        throw std::logic_error("util::binreader::begin: already started");

    if (data.size() == 0) {
        static unsigned char garbage[1];
        m_data = garbage;
        m_size = 0;
    } else {
        m_data = data.data();
        m_size = data.size();
    }
}

// declared in util.hh
void binreader::end()
{
    if (!m_data)
        throw std::logic_error("util::binreader::end: not started");

    if (m_size > 0)
        throw std::logic_error("util::binreader::end: data remaining");

    if (m_bitbuf_len > 0)
        throw std::logic_error("util::binreader::end: bit data remaining");

    m_data = nullptr;
    m_size = 0;
}

// declared in util.hh
void binreader::end_early()
{
    if (!m_data)
        throw std::logic_error("util::binreader::end_early: not started");

    m_data = nullptr;
    m_size = 0;
    m_bitbuf = 0;
    m_bitbuf_len = 0;
}

// declared in util.hh
std::uint8_t binreader::read_u8()
{
    if (m_size == 0)
        throw std::logic_error("util::binreader::read_u8: out of data");

    if (m_bitbuf_len > 0)
        throw std::logic_error("util::binreader::read_u8: bit data remaining");

    std::uint8_t value = *m_data;
    m_data++;
    m_size--;
    return value;
}

// declared in util.hh
std::uint16_t binreader::read_u16()
{
    std::uint16_t value;
    value  = read_u8();
    value |= read_u8() << 8;
    return value;
}

// declared in util.hh
std::uint32_t binreader::read_u32()
{
    std::uint32_t value;
    value  = read_u16();
    value |= read_u16() << 16;
    return value;
}

// declared in util.hh
std::int64_t binreader::read_ubits(int bits)
{
    if (bits < 0 || bits > 63)
        throw std::logic_error("util::binreader::read_ubits: bad bit count");

    if (bits == 0)
        return 0;

    if (m_bitbuf_len == 0) {
        m_bitbuf = read_u8();
        m_bitbuf_len = 8;
    }

    std::int_fast64_t value;
    if (bits <= m_bitbuf_len) {
        value = m_bitbuf & ((1 << bits) - 1);
        m_bitbuf >>= bits;
        m_bitbuf_len -= bits;
    } else {
        value = m_bitbuf;
        int bits_consumed = m_bitbuf_len;
        m_bitbuf_len = 0;
        value |= read_ubits(bits - bits_consumed) << bits_consumed;
    }
    return value;
}

// declared in util.hh
std::int8_t binreader::read_s8()
{
    return read_u8();
}

// declared in util.hh
std::int16_t binreader::read_s16()
{
    return read_u16();
}

// declared in util.hh
std::int32_t binreader::read_s32()
{
    return read_u32();
}

// declared in util.hh
std::int64_t binreader::read_sbits(int bits)
{
    std::int64_t value = read_ubits(bits);

    value <<= 64 - bits;
    value >>= 64 - bits;

    return value;
}

// declared in util.hh
void binreader::discard(int bytes)
{
    if (bytes < 0)
        throw std::logic_error("util::binreader::discard: bad byte count");

    if (!m_data)
        throw std::logic_error("util::binreader::discard: not started");

    if (m_size < static_cast<unsigned int>(bytes))
        throw std::logic_error("util::binreader::discard: not enough data");

    m_data += bytes;
    m_size -= bytes;
}

// declared in util.hh
void binreader::discard_bits(int bits)
{
    read_ubits(bits);
}

}
}
