# Copyright 2006 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# Initial author: Matthias Baas

"""This module contains the 'Declaration wrapper' object.
"""

import sys, os.path, inspect, types
from filters import *
import decltypes
import pygccxml
import pyplusplus
import selection

# Create an alias for the builtin type() function
_type = type

allow_empty_queries = False
default_recursive = False
query_log = None
decoration_log = None

# If this is set to True an attempt to decorate a declaration will
# result in an error (this is set after the code creators have been created)
decl_lock = False

# IDecl
class IDecl:
    """Declaration interface.

    This class represents the interface to the declaration tree. Its
    main purpose is to "decorate" the nodes in the tree with
    information about how the binding is to be created. Instances of
    this class are never created by the user, instead they are
    returned by the API.

    You can think of this class as a container for declaration nodes
    that are selected by matching a set of filters.

    @group Selection interface: Decl, Namespace, Class, Constructor, Method, Function, Enum, Var, Decls, Namespaces, Classes, Constructors, Methods, Functions, Enums, Vars
    @group Decoration interface: expose, ignore, exclude, finalize, rename, setPolicy, setArgPolicy, disableKeywordArgs, setHeldType, addMethod, cdef, staticmethod
    """

    def __init__(self, decls, filter=None, modulebuilder=None):
        """Constructor.

        @param decls: One or more declarations that should be stored in this instance
        @type decls: declaration_t or list of declaration_t
        @param filter: Filter string (only for informational purposes)
        @type filter: str
        @param modulebuilder: The module builder object this object belongs to
        """
        global query_log

        self.modulebuilder = modulebuilder

        if type(decls)!=list:
            decls = [decls]

        # A sequence containing the underlying pygccxml declaration nodes.
        # (actually these are decl_wrapper nodes)
        self.decl_handles = decls

        # Determine where this instance was created...
        filename,funcname,linenr,sourceline = self._sourceInfo()
        self.filename = filename
        self.funcname = funcname
        self.linenr = linenr
        self.sourceline = sourceline

        # Dump info about this query...
        if query_log:
            print >>query_log, 70*"-"
            if funcname==None:
                print >>query_log, "%s, %d: %s"%(self.filename, self.linenr, self.sourceline)
            else:
                print >>query_log, "%s, %s(), %d: %s"%(self.filename, self.funcname, self.linenr, self.sourceline)
            if filter!=None:
                print >>query_log, "Filter: %s"%filter
            for decl in self.decl_handles:
                print >>query_log, " -> %s"%decl
            if len(self.decl_handles)==0:
                print >>query_log, " <no result>"
            elif len(self.decl_handles)>1:
                print >>query_log, " (%d declarations)"%len(self.decl_handles)
        

    def __str__(self):
        """Return a descriptive string."""
        if len(self.decl_handles)==0:
            return "Decl: <empty>"
        elif len(self.decl_handles)==1:
#            ds = getattr(self.decl_handles[0], "name", "?")
            ds = str(self.decl_handles[0])
            return 'Decl: "%s"'%(ds)
        else:
            return 'Decl: (%d declarations)'%(len(self.decl_handles))

    def __iter__(self):
        """Iterate over the matched declarations (non-recursively!).

        The iterator yields IDecl objects that each contain one declaration.
        The number of items is identical to self.decl_handles.
        """
        return iter(map(lambda decl: IDecl([decl], modulebuilder=self.modulebuilder), self.decl_handles))

    def __len__(self):
        return len(self.decl_handles)

    # iterContained
    def iterContained(self):
        """Iterate over all contained nodes.

        The iterator yields Decl objects.
        """
        for decl in self._iterContained():
            yield IDecl([decl], modulebuilder=self.modulebuilder)

    # expose
    def expose(self, flag=True):
        """Expose the declarations in the generated module.

        If flag is True all contained declarations are marked
        for being exposed, otherwise they are marked for being ignored.

        @param flag: Determines whether the declaration is actually exposed or ignored.
        @type flag: bool
        @returns: Returns self
        @see: L{ignore()}
        """
        if not flag:
            return self.ignore()
        self._checkLock()
        for d in self._iterContained():
            if decoration_log!=None:
                self._logDecoration("expose", d)
            d.include()
        return self

    # ignore
    def ignore(self, flag=True):
        """Ignore the declarations in the generated module.

        If flag is True all contained declarations are marked
        for being ignored, otherwise they are marked for being exposed.

        @param flag: Determines whether the declaration is actually ignored or exposed.
        @type flag: bool
        @return: Returns self
        @see: L{expose()}
        """
        if not flag:
            return self.expose()
        self._checkLock()
        for d in self._iterContained():
            if decoration_log!=None:
                self._logDecoration("ignore", d)
            d.exclude()
        return self

    # exclude
    def exclude(self, flag=True):
        """This is an alias for L{ignore()}."""
        return self.ignore(flag)
        
    # rename
    def rename(self, name):
        """Rename a declaration.

        The declaration will receive a new name under which it is exposed
        in the Python module.
        If there are currently several declarations referenced the new
        name is assigned to all of them. However, only the declarations
        that were directly matched will receive a new name, children are
        always ignored.
        
        @param name: New name for the declaration
        @type name: str
        """
        self._checkLock()
        for decl in self._iterContained(recursive=False):
            if decoration_log!=None:
                self._logDecoration("rename(%s)"%name, decl)
            decl.rename(name)
        return self

    # finalize
    def finalize(self):
        """Finalize virtual member functions or an entire class.

        This method can be called on classes or individual member functions
        to prevent the generation of wrapper classes. This is done by
        resetting the virtuality attribute of the members to "non-virtual"
        and by ignoring protected and private member functions.
        A ValueError exception is raised if the method is called on pure
        virtual functions because those cannot be wrapped without a wrapper
        class.

        Even when finalize() was successfully called it is still possible
        that Py++ generates a wrapper class nevertheless. The presence
        of virtual member functions is only one part of the test whether
        a wrapper class has to be generated (member variables can also
        trigger a wrapper class which is not prevented by this method).        
        """
        self._checkLock()
        VIRTUALITY_TYPES = pygccxml.declarations.VIRTUALITY_TYPES
        for decl in self._iterContained():
            # Mark the declaration as being finalized
            decl._pypp_api_finalized = True
            
            if not isinstance(decl, pygccxml.declarations.member_calldef_t):
                continue

            if (isinstance(decl, pyplusplus.decl_wrappers.constructor_t) or
                isinstance(decl, pyplusplus.decl_wrappers.destructor_t)):
                continue
            
            if decl.virtuality==VIRTUALITY_TYPES.PURE_VIRTUAL:
                raise ValueError, "%s\nMember is pure virtual and cannot be finalized."%decl

            # Pretend that this method is a non-virtual method
            decl.virtuality = VIRTUALITY_TYPES.NOT_VIRTUAL

            if decoration_log!=None:
                self._logDecoration("finalize", decl)
#            decl.finalize()

        # Ignore protected and private methods as these would trigger
        # the generation of a wrapper class
        ACCESS_TYPES = pygccxml.declarations.ACCESS_TYPES
        self.Methods(accesstype=ACCESS_TYPES.PROTECTED, allow_empty=True).ignore()
        self.Methods(accesstype=ACCESS_TYPES.PRIVATE, allow_empty=True).ignore()
        return self

    # setPolicy
    def setPolicy(self, policy):
        """Set policies for functions or methods.

        @param policy: Policy to apply to the contained functions/methods.
        @type policy: ...policy...
        """
        self._checkLock()
        for decl in self._iterContained():
            if decoration_log!=None:
                self._logDecoration("setPolicy(...)", decl)
            decl.call_policies = policy

        return self

    # setHeldType
    def setHeldType(self, heldtype):
        """Explicitly set the held type.
        
        Ex: C{setHeldType("boost::shared_ptr<Class>")}
        """
        self._checkLock()
        for decl in self._iterContained():
            if decoration_log!=None:
                self._logDecoration("setHeldType(%s)"%heldtype, decl)
            decl.held_type = heldType
        return self

    # disableKeywordArgs
    def disableKeywordArgs(self):
        """Disable generation of keyword arguments.
        """
       
        self._checkLock()
        for decl in self._iterContained():
            if decoration_log!=None:
                self._logDecoration("disableKeyWord", decl)
            decl.use_keywords = False
#            if (    isinstance(decl, calldef_t) and
#                not isinstance(decl, destructor_t) and
#                    getattr(decl, "access_type", None)!=PRIVATE):
#                decl._use_keywords = False
        return self

    # setArgPolicy
    def setArgPolicy(self, *policies):
        """Append argument policies.

        This method takes a variable number of arguments. Each argument
        must be an ArgPolicy object.
        """
        self._checkLock()
        for decl in self._iterContained():
            if decoration_log!=None:
                ps = ", ".join(map(lambda x: str(x), policies))
                self._logDecoration("setArgPolicy(%s)"%ps, decl)
            decl.function_transformers.extend(list(policies))
#            self.modulebuilder.mArgPolicyManager.setArgPolicy(decl, policies)

        return self

    # setAttr
    def setAttr(self, attr, value):
        """Set an arbitrary attribute.

        Sets an arbitrary attribute on the contained decl_wrappers.

        This method can be used as a backdoor to access Py++
        decl_wrapper properties that are not directly exposed in
        pypp_api (yet).

        @param attr: Attribute name
        @type attr: str
        @param value: The value that should be set
        """
        self._checkLock()
        for decl in self._iterContained():
            if decoration_log!=None:
                self._logDecoration('setAttr("%s", %s)'%(attr,value), decl)
            setattr(decl, attr, value)
        return self

    # addMethod
    def addMethod(self, name, impl):
        """Add a new method to a class.

        Adds a new method to a class. The implementation is given as a
        C/C++ function that is defined elsewhere.
        The return value is a Decl object that can be
        used to further customize the method.

        @Note: This method isn't implemented yet!

        @param name: The method name as it will appear in the Python module
        @type name: str
        @param impl: The name of the C/C++ function that implements the method.
        @type impl: str
        """
        return self.cdef(name, impl)


    # def
    def cdef(self, name, fn=None, *args):
        """Apply a raw def() statement.

        This method is equivalent to the Boost.Python def() method.
        Example::

          Class("Foo").cdef("spam", "cspam", return_internal_reference(), (arg("a"), arg("b", 0)), "The spam method")

        It is up to the user to ensure that the C/C++ function cspam
        is declared and implemented somewhere.

        If fn is None, the string name is not quoted. You can use this form
        to wrap constructors or operators. Example::
        
          Class("Foo").cdef("bp::init< const MFoo& >()")
        
        @param name: Name of the Python method or a valid Boost.Python construct
        @type name: str
        @param fn: Name of the C++ function that implements the method or None
        @type fn: str
        @param args: There can be up to three additional arguments in any order: A doc string, the call policies and the keywords.
        @see: L{staticmethod()}
        """

        self._checkLock()
        doc,policies,keywords = self._parseDefArgs(args)
        if fn==None:
            args = ['%s'%name]
        else:
            args = ['"%s"'%name, fn]
        if policies!=None:
            policystr = policies.create(None)
            args.append(policystr)
        if keywords!=None:
            args.append("(%s)"%", ".join(map(lambda x: "bp::"+str(x), keywords)))
        if doc!=None:
            a = map(lambda x: "%s\\n"%x, doc.split("\n"))
            while len(a)>0 and a[-1]=="\\n":
                a = a[:-1]
            if len(a)>0:
                # Remove the newline in the last line
                a[-1] = a[-1][:-2]
            args.append('%s'%"\n".join(map(lambda x: '"%s"'%x, a)))
        src = 'def( %s )'%(", ".join(args))
        for decl in self._iterContained(recursive=False):
            # Add the 'def' source code...
            decl.add_code(src)

        return self

    # staticmethod
    def staticmethod(self, name):
        """Apply a raw staticmethod() statement.

        @param name: Name of the method.
        @type name: str
        @see: L{cdef()}
        """

        self._checkLock()
        for decl in self._iterContained(recursive=False):
            src = 'staticmethod( "%s" )'%name
            decl.add_code(src)

        return self


    # Decl
    def Decls(self,
             name=None,
             fullname=None,
             type=None,
             retval=None,
             args=None,
             anyarg=None,
             signature=None,
             header=None,
             headerdir=None,
             accesstype=None,
             const=None,
             virtuality=None,
             filter=None,
             recursive=None,
             allow_empty=None,
             assert_count=None
             ):
        """Obtain a Decl object referencing one or more declarations.

        Filters all contained declarations and returns a new Decl
        object that only contains declarations matching the filtering
        rules as specified by the arguments. If an argument is None,
        that particular filtering operation is disabled. If several
        arguments are provided, all of them must be matched.

        For any filter that is based on strings (such as name) the
        following rules apply:

         - A string must match exactly the corresponding attribute of the
           declaration (C{name="wxFrame"} will only return the class
           "wxFrame").
         - A string that is bracketed by a leading and trailing slash '/' is
           interpreted as a regular expression (C{name="/wx.*/"} will return
           all classes that begin with "wx").

        Any argument can also be passed a list of values which duplicates
        the filter. These filter are concatenated with OR, so a declaration
        has to match only one of the filters. For example, you can select all
        classes starting with either "wx" or "WX" by setting
        C{name=["/wx.*/", "/WX.*/"}].

        The user defined filter function filter must accept a Decl
        object as argument and has to return True when the declaration
        is matched.

        @param name: Select declarations by name
        @type name: str
        @param fullname: Select declarations by name (which includes namespaces)
        @type fullname: str
        @param type: Select declarations by type. The type is given by a combination of flags (CLASS, MEMBER_FUNCTION/METHOD, FREE_FUNCTION/FUNCTION, ENUM, ...)
        @type type: int
        @param retval: Select functions/methods based on their return value (this implies the type flags MEMBER_FUNCTION | FREE_FUNCTION)
        @type retval: str
        @param args: Select functions/methods bases on their arguments (this implies the type flags MEMBER_FUNCTION | FREE_FUNCTION)
        @type args: list of str
        @param anyarg: Select all functions/methods that have the specified argument somewhere in their argument list (this implies the type flags MEMBER_FUNCTION | FREE_FUNCTION)
        @type anyarg: str
        @param signature: Select declarations by their signature (this implies the type flags MEMBER_FUNCTION | FREE_FUNCTION)
        @type signature: str
        @param header: Select declarations by the header file in which they are defined
        @type header: str
        @param headerdir: Select declarations by the directory in which their header file is located
        @type headerdir: str
        @param accesstype: Access type (PUBLIC or PROTECTED). This implies the type flags CLASS_MEMBER.
        @param const: Select declarations by their constness.
        @type const: bool
        @param virtuality: Select declarations by their virtuality. This implies the type flags CLASS_MEMBER:
        @type virtuality: Virtuality flags
        @param filter: User defined filter function
        @type callable
        @param recursive: Extend the search to grandchildren? If not specified, a global (customizable) default value is used.
        @type recursive: bool
        @param allow_empty: Allow empty results. If not specified, a global (customizable) default value is used.
        @type allow_empty: bool
        @param assert_count: Check the number of matched declarations in the resulting Decl object
        @type assert_count: int
        @returns: Returns a Decl object that may reference an arbitrary number of declarations.
        @rtype: IDecl
        @see: Namespace(), Class(), Method(), Function(), Enum()
        """
        global allow_empty_queries, default_recursive

        itype = 0
        filters = []

        if recursive==None:
            recursive = default_recursive
        if allow_empty==None:
            allow_empty = allow_empty_queries
        
        def addFilter(arg, filtercls):
            if arg!=None:
                if _type(arg)==list:
                    filters.append(OrFilter(map(lambda x: filtercls(x), arg)))
                else:
                    filters.append(filtercls(arg))

        # name filter
        addFilter(name, NameFilter)
        # fullname filter
        addFilter(fullname, FullNameFilter)
        # retval filter
        if retval!=None:
            addFilter(retval, RetValFilter)
            itype |= CALLABLE
        # args filter
        if args!=None:
            filters.append(ArgsFilter(args))
            itype |= CALLABLE
        # anyarg filter
        if anyarg!=None:
            raise NotImplementedError, "anyarg filter is not yet implemented"
        # signature filter
        if signature!=None:
            raise NotImplementedError, "signature filter is not yet implemented"
        # header filter
        addFilter(header, HeaderFilter)
        # headerdir filter
        addFilter(headerdir, HeaderDirFilter)
        # accesstype filter
        if accesstype!=None:
            addFilter(accesstype, AccessTypeFilter)
            itype |= CLASS_MEMBER
        # const filter
        if const!=None:
            addFilter(const, ConstFilter)
        # virtuality filter
        if virtuality!=None:
            addFilter(virtuality, VirtualityFilter)
            itype |= CLASS_MEMBER
        # custom filters
        if filter!=None:
            if _type(filter)==list:
                filters.extend(map(lambda x: CustomFilter(x), filter))
            else:
                filters.append(CustomFilter(filter))

        # XXX
        if itype!=0:
            if type==None:
                type = 0
            if (type & CALLABLE)==0:
                type |= itype
        addFilter(type, TypeFilter)

        # Add parent filters...
        pfs = []
        for decl in self.decl_handles:
            pfs.append(ParentFilter(decl, recursive))
        if len(pfs)>0:
            if len(pfs)==1:
                filters.append(pfs[0])
            else:
                filters.append(OrFilter(pfs))

        # Create the final filter by combining the individual filters
        # with AND...
        if len(filters)==0:
            filter = TrueFilter()
        elif len(filters)==1:
            filter = filters[0]
        else:
            filter = AndFilter(filters)

#        print "Filter:",filter
        if len(self.decl_handles)==0:
            decls = []
        else:
            decls = selection.select(self.decl_handles, filter)

        res = IDecl(decls, str(filter), modulebuilder=self.modulebuilder)
        count = res.count
        if allow_empty and count==0:
            return res
        if count==0:
            raise RuntimeError, "Query produced no results (filter: %s)"%filter
        res_count = res.count
        # If all contained declarations are from one single set of overloads
        # then treat that as one single declaration
        if res._checkOverloads():
            res_count = 1
        if assert_count!=None:
            if res_count!=assert_count:
                raise RuntimeError, "Query produced the wrong number of results (%d instead of %d)"%(res_count, assert_count)
        return res
            

    # Namespace
    def Namespaces(self, name=None, type=0, **args):
        """Obtain a Decl object referencing one or more namespaces.

        This method is equivalent to calling Decl() with the flag NAMESPACE
        set in its type filter.

        See Decl() for a full description of this method.

        @returns: Returns a Decl object that may reference an arbitrary number of declarations.
        @see: L{Decl()}
        """
        return self.Decls(name=name, type=type|NAMESPACE, **args)

    # Classes
    def Classes(self, name=None, type=0, **args):
        return self.Decls(name=name, type=type|CLASS, **args)

    # Methods
    def Methods(self, name=None, type=0, **args):
        return self.Decls(name=name, type=type|METHOD|CONSTRUCTOR, **args)

    # Constructors
    def Constructors(self, name=None, type=0, **args):
        return self.Decls(name=name, type=type|CONSTRUCTOR, **args)

    # Functions
    def Functions(self, name=None, type=0, **args):
        return self.Decls(name=name, type=type|FUNCTION, **args)

    # Enums
    def Enums(self, name=None, type=0, **args):
        return self.Decls(name=name, type=type|ENUM, **args)

    # Vars
    def Vars(self, name=None, type=0, **args):
        return self.Decls(name=name, type=type|VARIABLE, **args)

    # Decl
    def Decl(self, name=None, **args):
        return self.Decls(name, assert_count=1, **args)

    # Namespace
    def Namespace(self, name=None, **args):
        return self.Namespaces(name, assert_count=1, **args)

    # Class
    def Class(self, name=None, **args):
        return self.Classes(name, assert_count=1, **args)

    # Method
    def Method(self, name=None, **args):
        return self.Methods(name, assert_count=1, **args)

    # Constructor
    def Constructor(self, name=None, **args):
        return self.Constructors(name, assert_count=1, **args)

    # Function
    def Function(self, name=None, **args):
        return self.Functions(name, assert_count=1, **args)

    # Enum
    def Enum(self, name=None, **args):
        return self.Enums(name, assert_count=1, **args)

    # Var
    def Var(self, name=None, **args):
        return self.Vars(name, assert_count=1, **args)

    
    # Private methods:

    def _getCount(self):
        """Return the number of matched declarations.
        """
        return len(self.decl_handles)

    count = property(_getCount, None, None, "The number of matched declarations.")

    def _getTotalCount(self):
        """Return the total number of contained declarations (including the children).
        """
        return len(list(self))

    totalcount = property(_getTotalCount, None, None, "The total number of contained declarations.")

    def _checkLock(self):
        """Check that the decoration is not locked.

        If it is locked, an exception is thrown.
        """
        global decl_lock
        if decl_lock:
            raise RuntimeError, "You have to decorate the declarations before the code creators have been built."

    def _checkOverloads(self):
        """Check if all contained declarations are from the same set of overloads.

        @returns: True if all contained declarations are from the same set of overloads.
        @rtype: bool
        """
        if len(self.decl_handles)==0:
            return False
        # Get a list with the overloaded functions
        overloads = getattr(self.decl_handles[0], "overloads", None)
        if overloads==None:
            return False

        # Check if the decls are all contained in overloads...
        for decl in self.decl_handles[1:]:
            if decl not in overloads:
                return False

        return True

    def _parseDefArgs(self, args):
        """Determine which of the args is the doc string, call policies and keywords argument.

        @returns: Returns a tuple (doc, policies, keywords).
        """
        if len(args)>3:
            raise ValueError, "Too many arguments (%d)"%len(args)
        
        doc = None
        policies = None
        keywords = None
#        call_policy_t = pyplusplus.code_creators.call_policies.call_policy_t
        call_policy_t = pyplusplus.decl_wrappers.call_policies.call_policy_t
        for a in args:
            if isinstance(a, types.StringTypes):
                doc = a
            elif isinstance(a, call_policy_t):
                policies = a
            elif type(a)==tuple:
                keywords = a
            elif isinstance(a, decltypes.arg):
                keywords = (a,)
            else:
                raise ValueError, "Invalid argument: %s"%a
        return doc,policies,keywords

    def _iterContained(self, recursive=True):
        """Iterate over all contained declarations.

        The generator yields pygccxml declaration objects.

        @param recursive: Determines whether the method also yields children nodes 
        @type recursive: bool
        """
        global _timestamp
        _timestamp += 1
        for rootdecl in self.decl_handles:
            for decl in self._iterdecls(rootdecl, recursive=recursive):
                # Get the time stamp and check if this declaration hasn't 
                # been visited yet
                ts = getattr(decl, "_timestamp", -1)
                if ts!=_timestamp:
                    decl._timestamp = _timestamp
                    yield decl

    # _iterdecls
    def _iterdecls(self, rootdecl, recursive=True):
        """Depth first iteration over one or more declaration trees.
        
        rootdecl can be either a single declaration, a list of
        declarations or None. A declaration must be an object derived
        from the declaration_t class. If recursive is False, only the
        root is returned.
        
        @param recursive: Determines whether the method also yields children nodes 
        @type recursive: bool
        """
        if rootdecl==None:
            return

        if type(rootdecl) is not list:
            rootdecl = [rootdecl]

        for root in rootdecl:
            yield root
            if recursive:
                children = getattr(root, "declarations", None)
                for node in self._iterdecls(children):
                    yield node

    # _logDecoration
    def _logDecoration(self, operation, decl):
        """Write a line into the decoration log.

        @param operation: Name of the decoration operation
        @type operation: str
        @param decl: Declaration that was decorated
        @type decl: declaration_t
        """
        global decoration_log
        if self.funcname==None:
            print >>decoration_log, "%s,%d;%s; %s "%(self.filename, self.linenr, operation, decl)
        else:
            print >>decoration_log, "%s,%s(),%d;%s; %s "%(self.filename, self.funcname, self.linenr, operation, decl)

    # _sourceInfo
    def _sourceInfo(self):
        """Determine where in the user's source code this instance was created.

        Returns a tuple (filename, funcname, linenr, sourceline) that
        describes the source code line that created this instance.
        funcname is None if the source code is not located inside a function.

        @returns: (filename, funcname, linenr, sourceline)
        @rtype: 4-tuple
        """
        frame = inspect.currentframe()
        try:
            records = inspect.getouterframes(frame)
            # Determine the directory where this source file is located
            apidir = os.path.dirname(records[0][1])
            for rec in records[1:]:
                file = rec[1]
                dir = os.path.dirname(file)
                # Return the first record that is not within the API directory
                # (this must then be code from the user)
                if dir!=apidir:
                    linenr = rec[2]
                    funcname = rec[3]
                    srcindex = rec[5]
                    sourceline = rec[4][srcindex]
                    sourceline = sourceline.strip()
                    if funcname=="?":
                        funcname = None
                    return file,funcname,linenr,sourceline
        finally:
            del frame

        # We should never get here...
        return "?", None, 0, "?"

                
# This value is used to determine if a declaration was already visited or not
_timestamp = 0
