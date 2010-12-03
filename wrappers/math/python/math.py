# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  wrappers/control/python/control.py

# STD Imports
import os
import sys
import StringIO
from distutils import sysconfig

# Capture stderr, to suppress unwanted warnings
stderr = sys.stderr
sys.stderr = StringIO.StringIO()

try:
    try:
        import ext.core
    except ImportError:
        # Only import the core module if it exists,
        # it's a conditional dependency
        pass

    from ext._math import *

    def quatStr(self):
        return 'Quaternion(%f, %f, %f, %f)' % (self.x, self.y, self.z, self.w)
    Quaternion.__str__ = quatStr
    Quaternion.__repr__ = quatStr
    del quatStr

    def vector3Str(self):
        return 'Vector3(%f, %f, %f)' % (self.x, self.y, self.z)
    Vector3.__str__ = vector3Str
    Vector3.__repr__ = vector3Str
    del vector3Str

    def vector2Str(self):
        return 'Vector2(%f, %f)' % (self.x, self.y)
    Vector2.__str__ = vector2Str
    Vector2.__repr__ = vector2Str
    del vector2Str

finally:
    sys.stderr = stderr

