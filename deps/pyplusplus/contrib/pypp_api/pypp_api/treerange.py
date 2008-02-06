# Copyright 2006 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# Initial author: Matthias Baas

"""This module contains the TreeRange class.
"""

from pygccxml.declarations import scopedef_t

# TreeRange
class TreeRange:
    """This class represents a part of a tree.

    A B{simple tree range} is defined by a I{parent} node and a boolean
    flag I{recursive}. The immediate children of the parent node are
    always contained in the range (the parent itself is excluded). If
    the boolean flag is True then all grand-children of the parent
    node are also contained in the range.

    A B{compound range} is one that contains more than one simple ranges.

    The purpose of this class is to provide an iterator that yields
    parts of a declaration tree. This iteration is used when particular
    declarations are queried.
    By calling the L{union()} and L{intersect()} methods, a range
    can be created that restricts the search to particular areas of the
    tree. This can be used to exploit a priori knowledge about the filter
    function when it is known that a filter will never match a node outside
    a particular range.

    A tree range can itself be a tree of ranges. For example, consider the
    case when only the direct children of a node are contained in a range
    and you make a union() operation with a range that lies beneath that
    node. In this case, the sub range will be a children of the first range.
    A range object can only have sub ranges when its recursive flag is set
    to False, otherwise all possible sub ranges are already contained
    in that range anyway.
    """
    
    def __init__(self, parent, recursive, subranges=None):
        """Constructor.
        """

        if subranges==None:
            subranges = []

        # Parent node
        # (None is only allowed in the root)
        self.parent = parent
        # Recursive flag 
        self.recursive = recursive
        # A list of sub ranges. The list items are TreeRange objects.
        # The sub ranges must be disjoint!
        # The list must be empty when recursive is True!
        self.subranges = subranges

    def __str__(self):
        return "<TreeRange: parent:%s recursive:%s #subranges:%d>"%(self.parent, self.recursive, len(self.subranges))

    def __iter__(self):
        """The same as iterNodes()."""
        return self.iterNodes()

    # iterNodes
    def iterNodes(self):
        """Iterate over all nodes within this range.

        yields the individual nodes (declaration_t objects).
        """
        if self.parent!=None:
            for node in self._iterdecls(self.parent, self.recursive):
                yield node

        for sr in self.subranges:
            for node in sr.iterNodes():
                yield node

    # _iterdecls
    def _iterdecls(self, parent, recursive):
        """Depth first iteration over a declaration (sub) tree.
        
        Iterates over the children of parent. If recursive is set to True
        the iteration also includes the grand-children.
        The parent itself is not returned.

        @param parent: Starting node
        @param recursive: Determines whether the method also yields children nodes 
        @type recursive: bool
        """

        children = getattr(parent, "declarations", [])
        for ch in children:
            yield ch
            if recursive and isinstance(ch, scopedef_t):
                for node in self._iterdecls(ch, True):
                    yield node

    # iterRanges
    def iterRanges(self):
        """Iterate over the simple ranges within this range.

        The ranges are iterated from bottom to top.
        All ranges are disjoint!
        """

        for sr in self.subranges:
            for r in sr.iterRanges():
                yield r

        if self.parent!=None:
            yield self.parent, self.recursive
            

    # intersect
    def intersect(self, other):
        """Return the intersection of self and other.

        Update the current tree and return the new root.

        @param other: A simple range (parent, recursive).
        @type other: 2-tuple
        @returns: The new root node.
        @rtype: TreeRange        
        """

        res = TreeRange(None, False)

        # Intersect self with each disjoint component of other and
        # "add" the result to res...
        for rng in other.iterRanges():
            intersection = self._simpleIntersect(rng)
            res = res.union(intersection)

        # Check if the result only contains one sub range...
        if res.parent==None and len(res.subranges)==1:
            res = res.subranges[0]

        return res

    def _simpleIntersect(self, simplerange):
        """Return the intersection of self and other.

        Update the current tree and return the new root.

        @param simplerange: A simple range (parent, recursive).
        @type simplerange: 2-tuple
        @returns: The new root node.
        @rtype: TreeRange        
        """
        otherparent, otherrecursive = simplerange
        
        if self.parent!=None:
            rs = self._rangeRelationship(self.parent, otherparent)
            # disjoint?
            if rs==0:
                self.parent = None
                self.recursive = False
                self.subranges = []
                return self
            # otherparent==parent?
            elif rs==3:
                if not otherrecursive:
                    self.recursive = False
                    self.subranges = []
                return self
            # parent below otherparent?
            elif rs==2:
                if not otherrecursive:
                    self.parent = None
                    self.recursive = False
                    self.subranges = []
                return self
            # otherparent below parent
            else:
                if self.recursive:
                    self.parent = otherparent
                    self.recursive = otherrecursive
                    self.subranges = []
                    return self

                self.parent = None

                # go on as if parent was None in the first place
            
        # newsub receives all sub ranges that are below otherparent
        # (these will survive)
        newsub = []
        for sr in self.subranges:
            rs = self._rangeRelationship(sr.parent, otherparent)
            # sr parent == otherparent?
            if rs==3:
                if not otherrecursive:
                    sr.subranges = []
                    sr.recursive = False
                return sr
            # otherparent below sr.parent?
            elif rs==1:
                return sr._simpleIntersect(simplerange)
            # sr.parent below otherparent?
            elif rs==2:
                if otherrecursive:
                    newsub.append(sr)

        self.subranges = newsub
        return self

    # union
    def union(self, other):
        """Return the union of self and other.

        Update the current tree and return the new root.
        
        @param other: Another TreeRange object
        @type other: TreeRange
        @returns: The new root node.
        @rtype: TreeRange
        """
        res = self
        for rng in other.iterRanges():
            res = res._simpleUnion(rng)

        return res
    
    def _simpleUnion(self, simplerange):
        """Return the union of self and a simple range.

        Update the current tree and return the new root.
        
        @param simplerange: A simple range (parent, recursive).
        @type simplerange: 2-tuple
        @returns: The new root node.
        @rtype: TreeRange
        """
        otherparent, otherrecursive = simplerange

        if self.parent!=None:
            rs = self._rangeRelationship(self.parent, otherparent)
            # disjoint?
            if rs==0:
                otherrange = TreeRange(otherparent, otherrecursive, [])
                root = TreeRange(None, False, [self, otherrange])
                return root
            # otherparent==parent?
            elif rs==3:
                if otherrecursive:
                    self.recursive = True
                    self.subranges = []
                return self
            # parent below otherparent?
            elif rs==2:
                if otherrecursive:
                    self.parent = otherparent
                    self.recursive = True
                    self.subranges = []
                    return self
                else:
                    rng = TreeRange(otherparent, False, [self])
                    return rng
            # otherparent below parent
            else:
                # Are we already recursive? Then self already contains other...
                if self.recursive:
                    return self
                
            # if we get here, otherparent is below parent and self is not
            # recursive. This is treated as if parent was None

        # newsub receives all sub ranges that are below otherparent
        # (these will become the new sub range of other)
        newsub = []
        for i,sr in enumerate(self.subranges):
            rs = self._rangeRelationship(sr.parent, otherparent)
            # sr parent == otherparent?
            if rs==3:
                if otherrecursive:
                    sr.recursive = True
                    sr.subranges = []
                return self
            # otherparent below sr.parent?
            elif rs==1:
                del self.subranges[i]
                newsr = sr._simpleUnion(simplerange)
                # if a new TreeRange with empty parent was created, then
                # pull the subranges to this level
                if newsr.parent==None:
                    self.subranges.extend(newsr.subranges)
                else:
                    self.subranges.append(newsr)
                return self
            # sr.parent below otherparent?
            elif rs==2:
                newsub.append(sr)

        # other will become a new sub range
        rng = TreeRange(otherparent, otherrecursive, [])

        # Was other above some previous sub ranges? Then move those sub ranges
        # below other...
        if len(newsub)>0:
            for sr in newsub:
                self.subranges.remove(sr)
            rng.subranges = newsub

        self.subranges.append(rng)
        return self


    def _rangeRelationship(self, parent1, parent2):
        """Determines the relationship between two nodes.
        
        @returns: Returns the relationship as an integer value:
           - 0: The sub trees rooted at parent1 and parent2 are disjoint
           - 1: parent2 is below parent1
           - 2: parent1 is below parent2
           - 3: parent1 and parent2 are the same
        @rtype: int
        """
        path1 = self._rootPath(parent1)
        path2 = self._rootPath(parent2)
        res = 0
        if parent1 in path2:
            res |= 0x01
        if parent2 in path1:
            res |= 0x02
        return res

    def _rootPath(self, node):
        """Returns a list with all nodes from node to the root (including node).

        @precondition: node must have a parent attribute
        @returns: Returns a list of nodes
        @rtype: list
        """
        res = []
        while node!=None:
            res.append(node)
            node = node.parent
        return res

    def _dump(self, level=0):
        """Dump the range tree for debugging purposes.
        """
        s = "Rng: (%s:%s, %s)"%(getattr(self.parent, "name", "None"), self.parent.__class__.__name__, self.recursive)
        print "%s%s"%((2*level)*" ", s)
        for sr in self.subranges:
            sr._dump(level+1)

######################################################################

if __name__=="__main__":

    class DummyNode:
        def __init__(self, id, parent=None):
            self.id = id
            self.parent = parent
            self.declarations = []
            if parent!=None:
                self.parent.declarations.append(self)

        def __str__(self):
            return "<Node %d>"%self.id

    root = DummyNode(0)
    n1 = DummyNode(1, root)
    n2 = DummyNode(2, n1)
    n3 = DummyNode(3, n1)
    n4 = DummyNode(4, root)
    n5 = DummyNode(5, n4)
    n6 = DummyNode(6, n2)

    tr = TreeRange(root, False)
    tr = tr.union((n4,True))
    print tr
    for r in tr.iterRanges():
        print r
    for n in tr:
        print n