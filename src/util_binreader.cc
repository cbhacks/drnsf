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
binreader::binreader(const read_dir read_dir) :
    m_data(nullptr),
    m_size(0),
    m_bitbuf(0),
    m_bitbuf_len(0),
    m_read_dir(read_dir)
{
}

// declared in util.hh
void binreader::begin(const unsigned char *data, size_t size)
{
    if (m_data)
        throw std::logic_error("util::binreader::begin: already started");
    if (!data)
        throw std::logic_error("util::binreader::begin: null data arg");

    m_data = data;
    m_size = size;

    m_data_base = m_data;
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

        m_data_base = m_data;
    }
}

void binreader::begin(const util::binwriter &writer, int offset)
{
    if (m_data)
        throw std::logic_error("util::binreader::begin: already started");
    if (!writer.m_data.size())
        throw std::logic_error("util::binreader::begin: no writer data");

    if (offset < 0)
        offset += writer.m_data.size();
    if (offset < 0 || offset > writer.m_data.size())
        throw std::logic_error("util::binreader::begin: bad offset arg");

    m_data = &writer.m_data[offset];
    m_size = writer.m_data.size() - offset;

    m_data_base = m_data;
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
uint8_t binreader::read_u8()
{
    if (m_size == 0)
        throw std::logic_error("util::binreader::read_u8: out of data");

    if (m_bitbuf_len > 0)
        throw std::logic_error("util::binreader::read_u8: bit data remaining");

    uint8_t value = *m_data;
    m_data++;
    m_size--;
    return value;
}

// declared in util.hh
uint16_t binreader::read_u16()
{
    uint16_t value;
    value  = read_u8();
    value |= read_u8() << 8;
    return value;
}

// declared in util.hh
uint32_t binreader::read_u32()
{
    uint32_t value;
    value  = read_u16();
    value |= read_u16() << 16;
    return value;
}

// declared in util.hh
int64_t binreader::read_ubits(int bits)
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
    if (m_read_dir == read_dir::rtl) {
        if (bits <= m_bitbuf_len) {
            value = m_bitbuf & ((1 << bits) - 1);
            m_bitbuf >>= bits;
            m_bitbuf_len -= bits;
        }
        else {
            value = m_bitbuf;
            int bits_consumed = m_bitbuf_len;
            m_bitbuf_len = 0;
            value |= read_ubits(bits - bits_consumed) << bits_consumed;
        }
    }
    else if (m_read_dir == read_dir::ltr) {
        if (bits <= m_bitbuf_len) {
            value = (m_bitbuf & (256 - (1 << (8 - bits)))) >> (8 - bits);
            m_bitbuf <<= bits;
            m_bitbuf_len -= bits;
        }
        else {
            int bits_consumed = m_bitbuf_len;
            if (bits < 8)
                value = m_bitbuf >> (8 - bits);
            else
                value = m_bitbuf << (bits - 8);
            m_bitbuf_len = 0;
            value |= read_ubits(bits - bits_consumed);
        }
    }
    return value;
}

// declared in util.hh
int8_t binreader::read_s8()
{
    return read_u8();
}

// declared in util.hh
int16_t binreader::read_s16()
{
    return read_u16();
}

// declared in util.hh
int32_t binreader::read_s32()
{
    return read_u32();
}

// declared in util.hh
int64_t binreader::read_sbits(int bits)
{
    int64_t value = read_ubits(bits);

    value <<= 64 - bits;
    value >>= 64 - bits;

    return value;
}

// declared in util.hh
blob binreader::read_bytes(int bytes)
{
    if (bytes < 0)
        throw std::logic_error("util::binreader::read_bytes: bad byte count");

    if (!m_data)
        throw std::logic_error("util::binreader::read_bytes: not started");

    if (m_size < static_cast<unsigned int>(bytes))
        throw std::logic_error("util::binreader::read_bytes: not enough data");

    blob result(m_data, m_data + bytes);
    m_data += bytes;
    m_size -= bytes;
    return result;
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

#if FEATURE_INTERNAL_TEST
namespace {

TEST(util_binreader, U8Read)
{
    blob data = { 0, 1, 0x7F, 0x80, 0xFF };
    binreader r;
    r.begin(data);
    EXPECT_EQ(r.read_u8(), 0);
    EXPECT_EQ(r.read_u8(), 1);
    EXPECT_EQ(r.read_u8(), 0x7F);
    EXPECT_EQ(r.read_u8(), 0x80);
    EXPECT_EQ(r.read_u8(), 0xFF);
    r.end();
}

TEST(util_binreader, U16Read)
{
    blob data = {
        0, 0,
        1, 0,
        0xFF, 0x7F,
        0x00, 0x80,
        0xFF, 0xFF
    };
    binreader r;
    r.begin(data);
    EXPECT_EQ(r.read_u16(), 0);
    EXPECT_EQ(r.read_u16(), 1);
    EXPECT_EQ(r.read_u16(), 0x7FFF);
    EXPECT_EQ(r.read_u16(), 0x8000);
    EXPECT_EQ(r.read_u16(), 0xFFFF);
    r.end();
}

TEST(util_binreader, U32Read)
{
    blob data = {
        0, 0, 0, 0,
        1, 0, 0, 0,
        0xFF, 0xFF, 0xFF, 0x7F,
        0x00, 0x00, 0x00, 0x80,
        0xFF, 0xFF, 0xFF, 0xFF
    };
    binreader r;
    r.begin(data);
    EXPECT_EQ(r.read_u32(), 0);
    EXPECT_EQ(r.read_u32(), 1);
    EXPECT_EQ(r.read_u32(), 0x7FFFFFFFLL);
    EXPECT_EQ(r.read_u32(), 0x80000000LL);
    EXPECT_EQ(r.read_u32(), 0xFFFFFFFFLL);
    r.end();
}

TEST(util_binreader, UBitsRead)
{
    blob data = { 0b01000010, 0b10111010, 0b10101010, 0b10101010, 0b11101010 };
    binreader r;
    r.begin(data);
    EXPECT_EQ(r.read_ubits( 0), 0);
    EXPECT_EQ(r.read_ubits( 1), 0);
    EXPECT_EQ(r.read_ubits( 1), 1);
    EXPECT_EQ(r.read_ubits( 4), 0);
    EXPECT_EQ(r.read_ubits( 4), 0b1001); // crosses a byte boundary
    EXPECT_EQ(r.read_ubits( 5), 0b01110);
    EXPECT_EQ(r.read_ubits(23), 0b10101010101010101010101);
    EXPECT_EQ(r.read_ubits( 2), 0b11);
    r.end();
}

TEST(util_binreader, S8Read)
{
    blob data = { 0, 1, 0x7F, 0x80, 0xFF };
    binreader r;
    r.begin(data);
    EXPECT_EQ(r.read_s8(), 0);
    EXPECT_EQ(r.read_s8(), 1);
    EXPECT_EQ(r.read_s8(), 0x7F);
    EXPECT_EQ(r.read_s8(), -0x80);
    EXPECT_EQ(r.read_s8(), -1);
    r.end();
}

TEST(util_binreader, S16Read)
{
    blob data = {
        0, 0,
        1, 0,
        0xFF, 0x7F,
        0x00, 0x80,
        0xFF, 0xFF
    };
    binreader r;
    r.begin(data);
    EXPECT_EQ(r.read_s16(), 0);
    EXPECT_EQ(r.read_s16(), 1);
    EXPECT_EQ(r.read_s16(), 0x7FFF);
    EXPECT_EQ(r.read_s16(), -0x8000);
    EXPECT_EQ(r.read_s16(), -1);
    r.end();
}

TEST(util_binreader, S32Read)
{
    blob data = {
        0, 0, 0, 0,
        1, 0, 0, 0,
        0xFF, 0xFF, 0xFF, 0x7F,
        0x00, 0x00, 0x00, 0x80,
        0xFF, 0xFF, 0xFF, 0xFF
    };
    binreader r;
    r.begin(data);
    EXPECT_EQ(r.read_s32(), 0);
    EXPECT_EQ(r.read_s32(), 1);
    EXPECT_EQ(r.read_s32(), 0x7FFFFFFFLL);
    EXPECT_EQ(r.read_s32(), -0x80000000LL);
    EXPECT_EQ(r.read_s32(), -1LL);
    r.end();
}

TEST(util_binreader, SBitsRead)
{
    blob data = { 0b01000010, 0b10111010, 0b10101010, 0b10101010, 0b11101010 };
    binreader r;
    r.begin(data);
    EXPECT_EQ(r.read_sbits( 0), 0);
    EXPECT_EQ(r.read_sbits( 1), 0);
    EXPECT_EQ(r.read_sbits( 1), -1);
    EXPECT_EQ(r.read_sbits( 4), 0);
    EXPECT_EQ(r.read_sbits( 4), -0b0111); // crosses a byte boundary
    EXPECT_EQ(r.read_sbits( 5), 0b01110);
    EXPECT_EQ(r.read_sbits(23), -0b01010101010101010101011);
    EXPECT_EQ(r.read_sbits( 2), -0b01);
    r.end();
}

TEST(util_binreader, PartialBitError)
{
    blob data = { 0, 0 };
    binreader r;
    r.begin(data);
    r.read_ubits(1);
    EXPECT_THROW(r.read_u8(), std::logic_error);
}

TEST(util_binreader, DoubleBeginError)
{
    blob data = { 0 };
    binreader r;
    r.begin(data);
    EXPECT_THROW(r.begin(data), std::logic_error);
}

TEST(util_binreader, EarlyEndError)
{
    blob data = { 0 };
    binreader r;
    r.begin(data);
    EXPECT_THROW(r.end(), std::logic_error);
}

TEST(util_binreader, DoubleEndError)
{
    blob data = { 0 };
    binreader r;
    r.begin(data);
    r.read_u8();
    r.end();
    EXPECT_THROW(r.end(), std::logic_error);
}

TEST(util_binreader, OverrunError)
{
    blob data_32(7);
    binreader r_32;
    r_32.begin(data_32);
    r_32.read_u32();
    EXPECT_THROW(r_32.read_u32(), std::logic_error);

    blob data_16(3);
    binreader r_16;
    r_16.begin(data_16);
    r_16.read_u16();
    EXPECT_THROW(r_16.read_u16(), std::logic_error);

    blob data_8(1);
    binreader r_8;
    r_8.begin(data_8);
    r_8.read_u8();
    EXPECT_THROW(r_8.read_u8(), std::logic_error);
}

}
#endif

}
}
