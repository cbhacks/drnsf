//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2018  DRNSF contributors
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
binwriter::binwriter() :
    m_active(false),
    m_bitbuf(0),
    m_bitbuf_len(0)
{
}

// declared in util.hh
void binwriter::begin()
{
    if (m_active)
        throw std::logic_error("util::binwriter::begin: already started");

    m_active = true;
    m_data = {};
}

// declared in util.hh
util::blob binwriter::end()
{
    if (!m_active)
        throw std::logic_error("util::binwriter::end: not started");

    if (m_bitbuf_len > 0)
        throw std::logic_error("util::binwriter::end: bit data missing");

    m_active = false;
    return std::move(m_data);
}

// declared in util.hh
void binwriter::write_u8(uint8_t value)
{
    if (m_bitbuf_len > 0)
        throw std::logic_error("util::binwriter::write_u8: bit data remaining");

    m_data.push_back(value);
}

// declared in util.hh
void binwriter::write_u16(uint16_t value)
{
    write_u8(value);
    write_u8(value >> 8);
}

// declared in util.hh
void binwriter::write_u32(uint32_t value)
{
    write_u16(value);
    write_u16(value >> 16);
}

// declared in util.hh
void binwriter::write_ubits(int bits, int64_t value)
{
    if (bits < 0 || bits > 63)
        throw std::logic_error("util::binwriter::write_ubits: bad bit count");

    if (value < 0)
        throw std::logic_error("util::binwriter::write_ubits: negative value");

    if (value >= (1 << bits))
        throw std::logic_error(
            "util::binwriter::write_ubits: value out of range"
        );

    if (bits == 0)
        return;

    // TODO
}

// declared in util.hh
void binwriter::write_s8(int8_t value)
{
    write_u8(value);
}

// declared in util.hh
void binwriter::write_s16(int16_t value)
{
    write_u16(value);
}

// declared in util.hh
void binwriter::write_s32(int32_t value)
{
    write_u32(value);
}

// declared in util.hh
void binwriter::write_sbits(int bits, int64_t value)
{
    if (bits < 0 || bits > 63)
        throw std::logic_error("util::binwriter::write_ubits: bad bit count");

    if (bits == 0)
        return;

    uint64_t u_value = value;

    u_value <<= 64 - bits;
    u_value >>= 64 - bits;

    write_ubits(bits, u_value);
}

}
}
