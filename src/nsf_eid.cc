//
// DRNSF - An unofficial Crash Bandicoot level editor
// Copyright (C) 2017-2019  DRNSF contributors
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

}
}
