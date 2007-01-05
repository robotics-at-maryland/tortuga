# Copyright 2006 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# Initial author: Matthias Baas

"""This module contains the generic selection function.
"""

import sys
from treerange import TreeRange

# select
def select(root, filter):
    """Select declarations based on a filter function.

    filter must accept a declaration as argument and return a boolean
    indicating whether the declaration matches the filter or not.

    @param root: The node(s) under which to search
    @type root: declaration_t or list of declaration_t
    @param filter: A filter that either accepts or rejects a declaration
    @type filter: callable
    """

    if type(root)!=list:
        root = [root]

    rng = TreeRange(root[0],True)
    for node in root[1:]:
        rng = rng.union(TreeRange(node,True))

    parents = None
    # First check for the filterRange() method so that also
    # regular functions can be passed
    if hasattr(filter, "filterRange"):
        frng = filter.filterRange()
        if frng!=None:
            rng = rng.intersect(frng)

    # Match the declarations...
#    print "Selection range:",rng
#    print "Select among %d decls"%len(list(rng))
    res = []
    for decl in rng:
        if filter(decl):
            res.append(decl)

    return res


        
    