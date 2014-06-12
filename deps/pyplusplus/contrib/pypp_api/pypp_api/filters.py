# Copyright 2006 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# Initial author: Matthias Baas

"""This module contains the filters that are used to select declarations.
"""

import re, os.path
from pygccxml.declarations import *
from decltypes import *
from treerange import TreeRange

# _StringMatcher
class _StringMatcher:
    """Helper class to match strings.

    This class can be used to match a string with a pattern that may
    either be an ordinary string or contain a regular expression
    enclosed in '/'.
    """

    def __init__(self, pattern):
        """Constructor.

        pattern may contain a regular expression enclosed between two '/'.

        @param pattern: The pattern used for matching
        @type pattern: str
        """

        self.regexp = None
        self.pattern = pattern
        
        # Is pattern a regular expression?
        if len(pattern)>=2 and pattern[0]=="/" and pattern[-1]=="/":
            self.regexp = re.compile(pattern[1:-1])
                    
    # match
    def match(self, txt):
        """Check if a string matches the pattern.

        @param txt: The string to match
        @type txt: str
        @returns: True if txt matches the pattern 
        """
        if self.regexp==None:
            return txt==self.pattern
        else:
            m = self.regexp.match(txt)
            if m==None:
                return False
            # It was only a successful match when the entire string was matched
            return m.end()==len(txt)
        

# FilterBase
class FilterBase:
    """Base class for all filters.
    """
    def __init__(self):
        pass

    def __call__(self, decl):
        raise NotImplementedError, "filters must always implement the __call__() method."

    def __invert__(self):
        """NOT-operator (~)"""
        return NotFilter(self)

    def __and__(self, other):
        """AND-operator (&)"""
        return AndFilter([self, other])

    def __or__(self, other):
        """OR-Operator (|)"""
        return OrFilter([self, other])

    def filterRange(self):
        """Return the range of the filter.

        A return value of None means the filter's range is unlimited.

        @returns: Filter range or None
        @rtype: TreeRange
        """
        return None
        
    def parentConstraints(self):
        """Return the parent constraints.

        *** obsolete ***

        A filter can use this method to indicate that it will always
        return False if the parent is not a particular node.
        
        The following return values are possible:

          - C{None}: There are no constraints. The filter may return True on any node.
          - C{[]}: The filter will always return False.
          - C{[(parent, recursive),...]}: The parent constraints.

        A single parent constraint (I{parent}, I{recursive}) means that the
        filter may only return True on children of I{parent}. If I{recursive}
        is set to True these can also be grand-children, otherwise they
        are only the direct children. On all other nodes, the filter will
        always return False so the search algorithm may decide not to visit
        them at all to speed up the search.
        
        @returns: None, an empty list or a list of tuples (parent, recursive).
        """
        return None

# TrueFilter
class TrueFilter(FilterBase):
    """Returns always True.
    """
    def __call__(self, decl):
        return True

    def __str__(self):
        return "True"

# FalseFilter
class FalseFilter(FilterBase):
    """Returns always False.
    """
    def __call__(self, decl):
        return False

    def __str__(self):
        return "False"

# AndFilter
class AndFilter(FilterBase):
    """Combine several other filters with AND.
    """
    def __init__(self, filters):
        FilterBase.__init__(self)
        self.filters = filters

    def __str__(self):
        return " & ".join(map(lambda x: "(%s)"%str(x), self.filters))

    def __call__(self, decl):
        for f in self.filters:
            if not f(decl):
                return False
        return True

    def filterRange(self):
        res = None
        for f in self.filters:
            rng = f.filterRange()
            if rng!=None:
                if res==None:
                    res = rng
                else:
                    res = res.intersect(rng)
        return res


# OrFilter
class OrFilter(FilterBase):
    """Combine several other filters with OR.
    """
    def __init__(self, filters):
        FilterBase.__init__(self)
        self.filters = filters

    def __str__(self):
        return " | ".join(map(lambda x: "(%s)"%str(x), self.filters))

    def __call__(self, decl):
        for f in self.filters:
            if f(decl):
                return True
        return False

    def filterRange(self):
        res = None
        for f in self.filters:
            rng = f.filterRange()
            if rng==None:
                return None
            if res==None:
                res = rng
            else:
                res = res.union(rng)
        return res
           

# NotFilter
class NotFilter(FilterBase):
    """Return the inverse result of a filter.
    """
    def __init__(self, filter):
        FilterBase.__init__(self)
        self.filter = filter

    def __str__(self):
        return "~(%s)"%str(self.filter)

    def __call__(self, decl):
        return not self.filter(decl)

    def filterRange(self):
        # TreeRange does not support a NOT operation, so extend the
        # range to full range
        return None


# NameFilter
class NameFilter(FilterBase):
    """Filter by declaration name.
    """

    def __init__(self, namepattern):
        FilterBase.__init__(self)
        self.matcher = _StringMatcher(namepattern)

    def __str__(self):
        return "name=='%s'"%self.matcher.pattern

    def __call__(self, decl):
        return self.matcher.match(decl.name)

# FullNameFilter
class FullNameFilter(FilterBase):
    """Filter by full declaration name.
    """

    def __init__(self, namepattern):
        FilterBase.__init__(self)
        self.matcher = _StringMatcher(namepattern)

    def __str__(self):
        return "fullname=='%s'"%self.matcher.pattern

    def __call__(self, decl):
        return self.matcher.match(full_name(decl))

# ParentFilter
class ParentFilter(FilterBase):
    """Filter by parent node.
    """
    def __init__(self, parent, grandparents=False):
        """Constructor.

        @param parent: Parent node
        @type parent: declaration_t
        @param grandparents: Determine whether the parent may also be a grandparent
        @type grandparents: bool
        """
        FilterBase.__init__(self)
        self.parent = parent
        self.recursive = grandparents

    def __str__(self):
        if self.recursive:
            return "anyparent=='%s'"%self.parent.name
        else:
            return "parent=='%s'"%self.parent.name

    def __call__(self, decl):
        # We can always return True as the filter range was already adjusted
        # so that all visited declarations meet the requirements for this
        # filter
        return True
        # The following line only works with recursive=False
#        return id(decl.parent)==id(self.parent)

    def filterRange(self):
        return TreeRange(self.parent, self.recursive)

# RetValFilter
class RetValFilter(FilterBase):
    """Filter by return type.
    """

    def __init__(self, retvalpattern):
        FilterBase.__init__(self)
        self.matcher = _StringMatcher(retvalpattern)

    def __str__(self):
        return "retval==%s"%self.matcher.pattern

    def __call__(self, decl):
        rettype = getattr(decl, "return_type", None)
        if rettype==None:
            return False
        return self.matcher.match(str(rettype))

# ArgsFilter
class ArgsFilter(FilterBase):
    """Filter by argument types.
    """

    def __init__(self, args):
        FilterBase.__init__(self)
        self.args = args

    def __str__(self):
        return "args==%s"%self.args

    def __call__(self, decl):
        args = self.args
        declargs = getattr(decl, "arguments", None)
        if declargs==None:
            return False
        if len(args)!=len(declargs):
            return False
        for arg,argument in zip(args, declargs):
            if arg!=str(argument.type):
                return False
        return True

# TypeFilter
class TypeFilter(FilterBase):
    """Filter by declaration type.
    """

    def __init__(self, typeflags):
        FilterBase.__init__(self)        
        self.flags = typeflags        

    def __str__(self):
        f = self.flags
        t = []
        if f & NAMESPACE:
            t.append("NAMESPACE")
        if f & CLASS:
            t.append("CLASS")
        if f & METHOD:
            t.append("METHOD")
        if f & FUNCTION:
            t.append("FUNCTION")
        if f & CONSTRUCTOR:
            t.append("CONSTRUCTOR")
        if f & ENUM:
            t.append("ENUM")
        if f & VARIABLE:
            t.append("VARIABLE")
        return "type==%s"%("|".join(t))

    def __call__(self, decl):
        return (self.declFlags(decl) & self.flags)!=0

    # declFlags
    def declFlags(self, decl):
        """Return the type flags for decl.

        @param decl: Declaration
        @type decl: declaration_t
        """
        res = 0
        if isinstance(decl, class_t):
            res |= CLASS
        elif isinstance(decl, member_calldef_t):
            res |= METHOD
        elif isinstance(decl, free_calldef_t):
            res |= FUNCTION
        elif isinstance(decl, enumeration.enumeration_t):
            res |= ENUM
        elif isinstance(decl, namespace_t):
            res |= NAMESPACE
        elif isinstance(decl, variable_t):
            res |= VARIABLE

        if isinstance(decl, constructor_t):
            res |= CONSTRUCTOR

        return res

# HeaderFilter
class HeaderFilter(FilterBase):
    """Filter by header file.
    """

    def __init__(self, header):
        FilterBase.__init__(self)
        self.header = os.path.abspath(header)

    def __str__(self):
        return "header==%s"%self.header

    def __call__(self, decl):
        loc = getattr(decl, "location", None)
        if loc==None:
            return False
        header = os.path.abspath(loc.file_name)
        return header==self.header

# HeaderDirFilter
class HeaderDirFilter(FilterBase):
    """Filter by header location.
    """

    def __init__(self, headerdir):
        FilterBase.__init__(self)
        self.headerdir = os.path.abspath(headerdir)

    def __str__(self):
        return "headerdir==%s"%self.headerdir

    def __call__(self, decl):
        loc = getattr(decl, "location", None)
        if loc==None:
            return False
        dir = os.path.abspath(os.path.dirname(loc.file_name))
        return dir[:len(self.headerdir)]==self.headerdir

# AccessTypeFilter
class AccessTypeFilter(FilterBase):
    """Filter by access type.
    """

    def __init__(self, accesstype):
        """Constructor.

        accesstype can bei either PUBLIC or PROTECTED.
        """
        FilterBase.__init__(self)
        self.accesstype = accesstype

    def __str__(self):
        return "accesstype==%s"%self.accesstype

    def __call__(self, decl):
        try:
            at = getattr(decl, "access_type", None)
        except RuntimeError, e:
            # Accessing access_type on non-member variables
            # raises an error
            at = None

        if at==None:
            return False
        return at==self.accesstype

# ConstFilter
class ConstFilter(FilterBase):
    """Filter by constness.
    """

    def __init__(self, constness):
        """Constructor.

        constness is a boolean that specifies whether a declaration
        has to be declared const or not to match the filter.
        """
        FilterBase.__init__(self)
        self.constness = constness

    def __str__(self):
        return "const==%s"%self.constness

    def __call__(self, decl):
        const = bool(getattr(decl, "has_const", False))
        return const==self.constness

# VirtualityFilter
class VirtualityFilter(FilterBase):
    """Filter by virtuality.
    """

    def __init__(self, virtuality_flags):
        """Constructor.

        virtuality_flags is a combination of the flags NON_VIRTUAL,
        NON_PURE_VIRTUAL, PURE_VIRTUAL and VIRTUAL.
        """
        FilterBase.__init__(self)
        self.virtuality_flags = virtuality_flags

    def __str__(self):
        f = []
        if self.virtuality_flags & NON_VIRTUAL:
            f.append("non-virtual")
        if self.virtuality_flags & NON_PURE_VIRTUAL:
            f.append("non-pure virtual")        
        if self.virtuality_flags & PURE_VIRTUAL:
            f.append("pure virtual")

        v = "|".join(f)
        return "virtuality==%s"%v

    def __call__(self, decl):
        v = getattr(decl, "virtuality", None)
        if v==None:
            return False
        vf = self.virtuality_flags
        if vf&NON_VIRTUAL and v==VIRTUALITY_TYPES.NOT_VIRTUAL:
            return True
        elif vf&NON_PURE_VIRTUAL and v==VIRTUALITY_TYPES.VIRTUAL:
            return True
        elif vf&PURE_VIRTUAL and v==VIRTUALITY_TYPES.PURE_VIRTUAL:
            return True
        return False

# CustomFilter
class CustomFilter(FilterBase):
    """Filter by user defined function.
    """

    def __init__(self, func):
        FilterBase.__init__(self)
        self.func = func

    def __str__(self):
        return "custom"

    def __call__(self, decl):
        return self.func(decl)
