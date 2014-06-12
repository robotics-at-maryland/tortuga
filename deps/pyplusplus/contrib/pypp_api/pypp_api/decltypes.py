# Copyright 2006 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# Initial author: Matthias Baas

"""This module contains some basic definitions.
"""

NAMESPACE       = 0x01
CLASS           = 0x02
MEMBER_FUNCTION = 0x04
METHOD          = MEMBER_FUNCTION   # this also includes constructors
FREE_FUNCTION   = 0x08
FUNCTION        = FREE_FUNCTION
CONSTRUCTOR     = 0x10              # limit to constructors
ENUM            = 0x20
VARIABLE        = 0x40

CALLABLE        = METHOD | FUNCTION | CONSTRUCTOR
CLASS_MEMBER    = METHOD | CONSTRUCTOR | VARIABLE | ENUM

NON_VIRTUAL  = 0x01
NON_PURE_VIRTUAL = 0x02    
PURE_VIRTUAL = 0x04
VIRTUAL      = NON_PURE_VIRTUAL | PURE_VIRTUAL

# cpp
class cpp:
    """This class wraps C++ source code for default values.

    This class is used together with the 'arg' class to provide
    C++ source code as default value. Example:

    method.cdef("foo", "&Foo::foo", arg("ptr", cpp("bp::object()")))

    The cpp class prevents the generation of apostrophes (as it would
    happen when only a string would be passed).
    """
    def __init__(self, src):
        self.src = src

    def __str__(self):
        return self.src

# arg
class arg:
    """Provide keyword arguments for methods/functions.

    This class is equivalent to the Boost.Python arg class
    and is used together with the Decl.cdef method.
    """
    
    def __init__(self, name, default=None):
        """Constructor.

        @param name: Argument name
        @type name: str
        @param default: Optional default value
        """
        self.name = name
        self.default = default

    def __str__(self):
        res = 'arg("%s")'%self.name
        if self.default!=None:
            res += "=%s"%self.py2cpp(self.default)
        return res

    def py2cpp(self, val):
        """Convert a Python value to a C++ value.
        """
        if type(val)==bool:
            return str(val).lower()
        elif type(val)==str:
            return '"%s"'%val
        else:
            return str(val)
