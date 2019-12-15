#
# DRNSF - An unofficial Crash Bandicoot level editor
# Copyright (C) 2017-2019  DRNSF contributors
#
# See the AUTHORS.md file for more details.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

import drnsf
from drnsf import *

def extend(target):
    def decorator(f):
        setattr(target, f.__name__, f)
        return f
    return decorator

@extend(Atom)
def eachchild(self):
    c = self.firstchild()
    while c is not None:
        yield c
        c = c.nextsibling()

@extend(Atom)
def eachdescendant(self):
    for c in self.eachchild():
        yield c
        yield from c.eachdescendant()

def startconsole():
    def threadproc():
        import code
        code.interact()
    import threading
    threading.Thread(target=threadproc).start()
