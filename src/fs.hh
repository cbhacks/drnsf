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

#pragma once

/*
 * fs.hh
 *
 * Provides C++17 <filesystem> in the drnsf::fs namespace. These are normally
 * provided in the std::filesystem namespace from the header <filesystem>,
 * however some implementations (libstdc++ < 8.0) only provide an older
 * experimental version.
 */

#if __has_include(<experimental/filesystem>)

#include <experimental/filesystem>
namespace drnsf {
    namespace fs = std::experimental::filesystem;
}

#elif __has_include(<filesystem>)

#include <filesystem>
namespace drnsf {
    namespace fs = std::filesystem;
}

#else
#error "No C++17 filesystem support!"
#endif
