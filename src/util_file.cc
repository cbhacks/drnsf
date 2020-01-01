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
#include "util.hh"

namespace drnsf {
namespace util {

// declared in util.hh
file::~file()
{
    if (m_handle) {
        close();
    }
}

// declared in util.hh
void file::open(const std::string &path, const char *mode)
{
    if (m_handle) {
        throw std::logic_error("file::open: file already open");
    }
#ifdef _WIN32
    auto wpath = u8str_to_wstr(path);
    auto wmode = u8str_to_wstr(mode);
    m_handle = _wfopen(wpath.c_str(), wmode.c_str());
#else
    m_handle = fopen(path.c_str(), mode);
#endif
    if (!m_handle) {
        throw std::system_error(errno, std::generic_category());
    }
}

// declared in util.hh
void file::close()
{
    if (!m_handle) {
        throw std::logic_error("file::close: no file open");
    }
    fclose(m_handle);
    m_handle = nullptr;
}

// declared in util.hh
bool file::read(void *buffer, size_t len)
{
    if (!m_handle) {
        throw std::logic_error("file::read: no file open");
    }
    size_t result = fread(buffer, 1, len, m_handle);
    if (result == len) {
        return true;
    } else if (result == 0 && feof(m_handle) && !ferror(m_handle)) {
        return false;
    } else if (feof(m_handle)) {
        throw std::runtime_error("file::read: EOF");
    } else {
        throw std::system_error(errno, std::generic_category());
    }
}

// declared in util.hh
void file::write(const void *buffer, size_t len)
{
    if (!m_handle) {
        throw std::logic_error("file::write: no file open");
    }
    size_t result = fwrite(buffer, len, 1, m_handle);
    if (result != 1) {
        throw std::system_error(errno, std::generic_category());
    }
}

// declared in util.hh
void file::seek(long offset, int whence)
{
    if (!m_handle) {
        throw std::logic_error("file::seek: no file open");
    }
    if (fseek(m_handle, offset, whence)) {
        throw std::system_error(errno, std::generic_category());
    }
}

// declared in util.hh
long file::tell() const
{
    if (!m_handle) {
        throw std::logic_error("file::tell: no file open");
    }
    long result = ftell(m_handle);
    if (result == -1) {
        throw std::system_error(errno, std::generic_category());
    }
    return result;
}

}
}
