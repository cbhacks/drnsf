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

namespace util {

std::string to_string(std::string s)
{
	return s;
}

std::string to_string(long long ll)
{
	std::ostringstream ss;
	ss << ll;
	return ss.str();
}

std::string format(std::string fmt)
{
	auto delim_pos = fmt.find('$');
	if (delim_pos != std::string::npos)
		throw 0; // FIXME

	return fmt;
}

}
