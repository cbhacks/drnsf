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
    if (bits < 0 || bits > 32)
        throw std::logic_error("util::binwriter::write_ubits: bad bit count");

    if (value < 0)
        throw std::logic_error("util::binwriter::write_ubits: negative value");

    if (value >= (1LL << bits))
        throw std::logic_error(
            "util::binwriter::write_ubits: value out of range"
        );

    if (bits == 0)
        return;

    // Add the bits to a larger temporary bit buffer. The default bit buffer is
    // only 8 bits in size, which is not enough to store the already buffered
    // bits (up to 7) along with the new bits (up to 32).
    std::uint_fast64_t long_bitbuf = m_bitbuf | (value << m_bitbuf_len);
    m_bitbuf_len += bits;

    while (m_bitbuf_len >= 8) {
        m_data.push_back(long_bitbuf);
        long_bitbuf >>= 8;
        m_bitbuf_len -= 8;
    }
    m_bitbuf = long_bitbuf;
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

// declared in util.hh
void binwriter::write_bytes(util::blob data)
{
    m_data.insert(m_data.end(), data.begin(), data.end());
}

// declared in util.hh
void binwriter::pad(int alignment)
{
    if (alignment <= 0) {
        throw std::logic_error("util::binwriter::pad: invalid alignment");
    }

    int padding = alignment - (m_data.size() % alignment);
    if (padding != alignment) {
        while (padding--) {
            write_u8(0);
        }
    }
}

#if FEATURE_INTERNAL_TEST
namespace {

TEST(util_binwriter, U8Write)
{
    const blob data = { 0, 1, 0x7F, 0x80, 0xFF };
    binwriter w;
    w.begin();
    w.write_u8(0);
    w.write_u8(1);
    w.write_u8(0x7F);
    w.write_u8(0x80);
    w.write_u8(0xFF);
    EXPECT_EQ(w.end(), data);
}

TEST(util_binwriter, U16Write)
{
    const blob data = {
        0, 0,
        1, 0,
        0xFF, 0x7F,
        0x00, 0x80,
        0xFF, 0xFF
    };
    binwriter w;
    w.begin();
    w.write_u16(0);
    w.write_u16(1);
    w.write_u16(0x7FFF);
    w.write_u16(0x8000);
    w.write_u16(0xFFFF);
    EXPECT_EQ(w.end(), data);
}

TEST(util_binwriter, U32Write)
{
    const blob data = {
        0, 0, 0, 0,
        1, 0, 0, 0,
        0xFF, 0xFF, 0xFF, 0x7F,
        0x00, 0x00, 0x00, 0x80,
        0xFF, 0xFF, 0xFF, 0xFF
    };
    binwriter w;
    w.begin();
    w.write_u32(0);
    w.write_u32(1);
    w.write_u32(0x7FFFFFFFLL);
    w.write_u32(0x80000000LL);
    w.write_u32(0xFFFFFFFFLL);
    EXPECT_EQ(w.end(), data);
}

TEST(util_binwriter, UBitsWrite)
{
    const blob data = { 0b01000010, 0b10111010, 0b10101010, 0b10101010, 0b11101010 };
    binwriter w;
    w.begin();
    w.write_ubits( 0, 0);
    w.write_ubits( 1, 0);
    w.write_ubits( 1, 1);
    w.write_ubits( 4, 0);
    w.write_ubits( 4, 0b1001); // crosses a byte boundary
    w.write_ubits( 5, 0b01110);
    w.write_ubits(23, 0b10101010101010101010101);
    w.write_ubits( 2, 0b11);
    EXPECT_EQ(w.end(), data);
}

TEST(util_binwriter, S8Write)
{
    const blob data = { 0, 1, 0x7F, 0x80, 0xFF };
    binwriter w;
    w.begin();
    w.write_s8(0);
    w.write_s8(1);
    w.write_s8(0x7F);
    w.write_s8(-0x80);
    w.write_s8(-1);
    EXPECT_EQ(w.end(), data);
}

TEST(util_binwriter, S16Write)
{
    const blob data = {
        0, 0,
        1, 0,
        0xFF, 0x7F,
        0x00, 0x80,
        0xFF, 0xFF
    };
    binwriter w;
    w.begin();
    w.write_s16(0);
    w.write_s16(1);
    w.write_s16(0x7FFF);
    w.write_s16(-0x8000);
    w.write_s16(-1);
    EXPECT_EQ(w.end(), data);
}

TEST(util_binwriter, S32Write)
{
    const blob data = {
        0, 0, 0, 0,
        1, 0, 0, 0,
        0xFF, 0xFF, 0xFF, 0x7F,
        0x00, 0x00, 0x00, 0x80,
        0xFF, 0xFF, 0xFF, 0xFF
    };
    binwriter w;
    w.begin();
    w.write_s32(0);
    w.write_s32(1);
    w.write_s32(0x7FFFFFFFLL);
    w.write_s32(-0x80000000LL);
    w.write_s32(-1LL);
    EXPECT_EQ(w.end(), data);
}

TEST(util_binwriter, SBitsWrite)
{
    const blob data = { 0b01000010, 0b10111010, 0b10101010, 0b10101010, 0b11101010 };
    binwriter w;
    w.begin();
    w.write_sbits( 0, 0);
    w.write_sbits( 1, 0);
    w.write_sbits( 1, -1);
    w.write_sbits( 4, 0);
    w.write_sbits( 4, -0b0111); // crosses a byte boundary
    w.write_sbits( 5, 0b01110);
    w.write_sbits(23, -0b01010101010101010101011);
    w.write_sbits( 2, -0b01);
    EXPECT_EQ(w.end(), data);
}

TEST(util_binwriter, PartialBitError)
{
    binwriter w;
    w.begin();
    w.write_ubits(1, 0);
    EXPECT_THROW(w.write_u8(0), std::logic_error);
}

TEST(util_binwriter, DoubleBeginError)
{
    binwriter w;
    w.begin();
    EXPECT_THROW(w.begin(), std::logic_error);
}

TEST(util_binwriter, DoubleEndError)
{
    binwriter w;
    w.begin();
    w.end();
    EXPECT_THROW(w.end(), std::logic_error);
}

TEST(util_binwriter, Padding)
{
    binwriter w;
    w.begin();
    w.pad(4);
    EXPECT_EQ(w.end().size(), 0);

    w.begin();
    w.write_u8(0);
    w.pad(4);
    EXPECT_EQ(w.end().size(), 4);

    w.begin();
    w.write_u8(0);
    w.pad(1);
    EXPECT_EQ(w.end().size(), 1);

    w.begin();
    w.write_u32(0);
    w.write_u32(0);
    w.write_u32(0);
    w.pad(8);
    EXPECT_EQ(w.end().size(), 16);

    w.begin();
    w.write_u32(0);
    w.write_u32(0);
    w.write_u32(0);
    w.write_u32(0);
    w.pad(8);
    EXPECT_EQ(w.end().size(), 16);
}

}
#endif

}
}
