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
#include <chrono>
#include "util.hh"

#if _WIN32
#include <windows.h> // for wchar_t conversions
#endif

namespace drnsf {
namespace util {

// declared in util.hh
std::string to_string(char c)
{
    return std::string(1, c);
}

// declared in util.hh
std::string to_string(std::string s)
{
    return s;
}

#ifdef _WIN32
// declared in util.hh
std::wstring u8str_to_wstr(std::string u8str)
{
    if (u8str.empty())
        return L"";

    int len = MultiByteToWideChar(
        CP_UTF8,
        0,
        u8str.c_str(),
        u8str.size(),
        nullptr,
        0
    );

    if (len <= 0)
        throw std::runtime_error("util::u8str_to_wstr: failed to convert");

    std::wstring wstr(len, L' ');

    int err = MultiByteToWideChar(
        CP_UTF8,
        0,
        u8str.c_str(),
        u8str.size(),
        wstr.data(),
        len
    );

    if (err <= 0)
        throw std::runtime_error("util::u8str_to_wstr: failed to convert");

    return wstr;
}

// declared in util.hh
std::string wstr_to_u8str(std::wstring wstr)
{
    if (wstr.empty())
        return "";

    int len = WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr.c_str(),
        wstr.size(),
        nullptr,
        0,
        nullptr,
        nullptr
    );

    if (len <= 0)
        throw std::runtime_error("util::wstr_to_u8str: failed to convert");

    std::string u8str(len, ' ');

    int err = WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr.c_str(),
        wstr.size(),
        u8str.data(),
        u8str.size(),
        nullptr,
        nullptr
    );

    if (err <= 0)
        throw std::runtime_error("util::wstr_to_u8str: failed to convert");

    return u8str;
}
#endif

// declared in util.hh
long get_time()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
}

}
}
