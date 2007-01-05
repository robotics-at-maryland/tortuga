# Copyright 2006 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# Initial author: Matthias Baas

"""This module contains the interface for manipulating trees.

Just importing the module will attach new methods to the pyplusplus
code creator base class.
The extended interface simplifies iterating over the tree and
manipulating it.
"""

import pyplusplus.code_creators

# iter
def iter(self, includeself=True, recursive=True):
    """Iterate over the code creator tree.

    @param includeself: Should root also be yielded?
    @type includeself: bool
    @param recursive: Iterate over grandchildren?
    @type recursive: bool
    """
    if includeself:
        yield self
        
    children = getattr(self, "creators", [])
    for child in children:
        yield child
        if recursive:
            for node in iter(child, includeself=False):
                yield node

# iterSiblings
def iterSiblings():
    pass

# insertBefore
def insertBefore(self, node):
    """Insert self before node.

    self will be a sibling of node.
    """
    if not isinstance(self, pyplusplus.code_creators.code_creator_t):
        raise TypeError, "self must be a code creator node."
    if self.parent!=None:
        raise ValueError, "self is already inserted into the tree. Remove it first."

    idx = node.parent.creators.index(node)
    node.parent.adopt_creator(self, idx)

# insertAfter
def insertAfter(self, node):
    """Insert self after node.

    self will be a sibling of node.
    """
    if not isinstance(self, pyplusplus.code_creators.code_creator_t):
        raise TypeError, "self must be a code creator node."
    if self.parent!=None:
        raise ValueError, "self is already inserted into the tree. Remove it first."

    idx = node.parent.creators.index(node)
    node.parent.adopt_creator(self, idx+1)

# insertAsChildOf
def insertAsChildOf(self, parent, index=None):
    """Insert self as a children of another node.

    Insert self as a children of parent. By default, self is appended to
    the list of children of parent.

    @param parent: The parent node.
    @type parent: compound_t
    @param index: The desired position of self (or None)
    @type index: int
    """
    if not isinstance(parent, pyplusplus.code_creators.compound_t):
        raise ValueError, "Leaf nodes cannot have children nodes."
    if not isinstance(self, pyplusplus.code_creators.code_creator_t):
        raise TypeError, "self must be a code creator node."
    if self.parent!=None:
        raise ValueError, "self is already inserted into the tree. Remove it first."
    
    parent.adopt_creator(self, index)

# remove
def remove(self):
    """Remove this node.

    @return: self
    """
    self.parent.remove_creator(self)
    return self

######################################################################

# Update the code_create_t class
code_creator_t = pyplusplus.code_creators.code_creator_t
code_creator_t.iter = iter
code_creator_t.insertBefore = insertBefore
code_creator_t.insertAfter = insertAfter
code_creator_t.insertAsChildOf = insertAsChildOf
code_creator_t.remove = remove
