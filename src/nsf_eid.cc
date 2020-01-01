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
#include "nsf.hh"

namespace drnsf {
namespace nsf {

// declared in res.hh
std::string eid::str() const
{
    char result[8];
    char *result_p = result;

    if (m_value & 0x80000000) {
        // If the high bit is set, this EID is not legitimate and cannot be
        // converted to a 5-char string. Instead, we will prefix it with an
        // equal sign. This symbol will be used when converting back from
        // string to EID to indicate that the high bit is flipped.
        *result_p++ = '=';
    }

    *result_p++ = charset[(m_value >> 25) & 0x3F];
    *result_p++ = charset[(m_value >> 19) & 0x3F];
    *result_p++ = charset[(m_value >> 13) & 0x3F];
    *result_p++ = charset[(m_value >> 7) & 0x3F];
    *result_p++ = charset[(m_value >> 1) & 0x3F];

    if (~m_value & 1) {
        // If the low bit is NOT set, this EID is not legitimate and cannot
        // be converted to a 5-char string. Instead, we will append an equal
        // sign to it. This symbol will be used when converting back from
        // string to EID to indicate that the low bit is flipped.
        *result_p++ = '=';
    }

    *result_p++ = '\0';

    return std::string(result);
}

// declared in nsf.hh
bool eid::try_parse(const std::string &str)
{
    static int8_t reverse_charset[CHAR_MAX + 1];

    // Build the charset reverse lookup table the first time this function is
    // called.
    static std::once_flag once;
    std::call_once(once, []{
        for (int i = 0; i < CHAR_MAX; i++) {
            reverse_charset[i] = -1;
        }
        for (int i = 0; i < 64; i++) {
            reverse_charset[int(charset[i])] = i;
        }
    });

    const char *input_p = str.c_str();
    uint32_t value = 0;

    // Parse the equal-sign prefix, if any. This indicates that the high bit is
    // set.
    if (*input_p == '=') {
        value = 1;
        input_p++;
    }

    // Parse the five content characters.
    for (int i = 0; i < 5; i++) {
        char c = *input_p++;
        if (c <= 0) {
            return false;
        }

        auto cv = reverse_charset[int(c)];
        if (cv == -1) {
            return false;
        }

        value <<= 6;
        value |= (cv & 0x3F);
    }

    // Parse the equal-sign suffix, if any. This indicates that the low bit is
    // clear.
    if (*input_p == '=') {
        value <<= 1;
        input_p++;
    } else {
        value <<= 1;
        value |= 1;
    }

    // Fail if the string is any longer than this.
    if (*input_p) {
        return false;
    }

    m_value = value;
    return true;
}

}
}
