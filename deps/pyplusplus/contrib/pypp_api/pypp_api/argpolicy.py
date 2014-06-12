# Copyright 2006 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# Initial author: Matthias Baas

import sys, os.path, copy
from pygccxml import declarations
from pyplusplus import code_creators
from pyplusplus import decl_wrappers
from pyplusplus import file_writers
from declwrapper import IDecl

# ArgPolicyManager
class ArgPolicyManager:
    """Manages all argument policies for a project.

    Before the code creator tree is created, setArgPolicy() has to be
    called on any method that requires this decoration.
    After the decoration is done and the code creator tree has been
    built, the updateCreators() method has to be called to update the tree.
    Finally, the writeFiles() method can be called to write the additional
    wrapper files.    
    """
    
    def __init__(self, includePaths=[]):
        """Constructor.

        The includePaths argument should be the same list that was passed
        to the module builder constructor. It is used to remove the
        unnecessary parts from include file names.

        @param includePaths: List of include paths
        @type includePaths: list of str
        """

        self.includePaths = includePaths

        # Key: Class declaration / Value: list of WrapperManagers
        self.classes = {}

        # Key: Include file name / Value: Irrelevant
        self.includes = {}
               
        # The root of the code creator tree
        self.creator_root = None

    # setArgPolicy
    def setArgPolicy(self, decl, policies):
        """Assign argument policies to a declaration.

        @param decl: The calldef declaration that receives the policies
        @param policies: A sequence of policy objects.
        """
        # Get the class declaration that belongs to decl
        cls = self._classDecl(decl)
        if cls==None:
            raise NotImplemented, "Arg policies on free functions are not supported yet"
        # Create the wrapper "code creator"...
        wm = WrapperManager(decl, policies)
        wm.wrappername = self._wrapperName(cls, decl)
        if cls in self.classes:
            self.classes[cls].append(wm)
        else:
            self.classes[cls] = [wm]

        # Ignore the declaration and replace it with the wrapper...
        decl.exclude()
        arglist = wm.wrapperargs[1:]
        if len(arglist)>0:
            dummydecl = decl_wrappers.member_function_t(arguments=arglist)
#            c = code_creators.function_t(dummydecl)
            c = code_creators.mem_fun_t(dummydecl)
#            args = c._keywords_args()
            args = c.keywords_args()
            cls.add_code('def( "%s", %s, %s)'%(decl.alias, wm.wrappername, args))
        else:
            cls.add_code('def( "%s", %s)'%(decl.alias, wm.wrappername))
#        IDecl([cls]).cdef(decl.alias, wm.wrappername)
        # Add the header to the include files...
        incname = self._outputFilename(cls)+file_writers.multiple_files_t.HEADER_EXT
        if incname not in self.includes:
            self.includes[incname] = 1

    # updateCreators
    def updateCreators(self, root):
        """Modifies the code creator tree.

        This method has to be called after the code creator tree was built
        and before the files were written.

        @param root: The root of the code creator tree
        @type root: module_t
        """
        self.creator_root = root
        for inc in self.includes:
            root.adopt_include(code_creators.include_t(inc))

    # writeFiles
    def writeFiles(self, path):
        """Write additional source files.

        @param path: The output directory
        @type path: str
        """

        # Key: sigid / Value: list of (decl, proto)
        sigids = {}
        # This dict contains the ambiguous wrappers with the same sigid
        # Key: sigid / Value: Reference to list in sigids
        ambiguous = {}

        for cls in self.classes:
            basename = self._outputFilename(cls)
            hname = basename+file_writers.multiple_files_t.HEADER_EXT
            cppname = basename+file_writers.multiple_files_t.SOURCE_EXT

            guard = "__%s_pyplusplus_wrapper__"%cls.alias
            hsrc = "#ifndef %s\n"%guard
            hsrc += "#define %s\n\n"%guard
            hsrc += "#include <boost/python.hpp>\n"
            # Add the include file where the original class is defined...
            incd = code_creators.include_directories_t()
            incd.user_defined = self.includePaths
            header = incd.normalize_header(cls.location.file_name)
            hsrc += "#include <%s>\n\n"%header

            # Include all 'global' include files...
            cppsrc = ""
            include_creators = filter(lambda creator: isinstance( creator, code_creators.include_t)
#                                      and not isinstance(creator, code_creators.precompiled_header_t)
                                      , self.creator_root.creators)
            includes = map(lambda include_creator: include_creator.create()
                           , include_creators )
            cppsrc += os.linesep.join(includes)
            cppsrc += "\n\n"

#            cppsrc += '#include "%s"\n\n'%hname
            wms = self.classes[cls]
            for wm in wms:
                sigid,proto,src = wm.create()
                if sigid in sigids:
                    sigids[sigid].append((wm.decl, proto))
                    ambiguous[sigid] = sigids[sigid]
                else:
                    sigids[sigid] = [(wm.decl, proto)]
                hsrc += "%s;\n"%proto
                cppsrc += (60*"/")+"\n"
                cppsrc += "// Original:\n// %s\n"%wm.decl
                cppsrc += (60*"/")+"\n"
                cppsrc += "%s\n"%src

            hsrc += "\n#endif\n"
           
            file_writers.writer_t.write_file(os.path.join(path, hname), hsrc)
            file_writers.writer_t.write_file(os.path.join(path, cppname), cppsrc)

        # Have there been any ambiguous wrappers?
        if ambiguous!={}:
            print 70*"="
            print "ERROR: There are ambiguous wrapper functions:"
            for sigid in ambiguous:
                print 70*"-"
                for i, (decl,proto) in enumerate(ambiguous[sigid]):
                    print "(%d) %s"%(i+1, decl)
                print "->"
                for i, (decl,proto) in enumerate(ambiguous[sigid]):
                    print "(%d) %s"%(i+1, proto)
                print 70*"-"
            print "***ABORTING because of %d ambiguous wrapper functions***"%(len(ambiguous))
            sys.exit(1)
            
    
    def _wrapperName(self, clsdecl, memberdecl):
        """Return the name of the wrapper function.
        """
        # The id of the memberdecl is added to make the name unique if there
        # are several (overloaded) functions with the same name
#        declid = abs(id(memberdecl))
        # Use a hash of the declaration string (i.e. the signature) because
        # this doesn't change the source file between runs
        declid = hex(abs(hash(str(memberdecl))))
        return "%s_%s_wrapper_%s"%(clsdecl.alias, memberdecl.alias, declid)

    def _outputFilename(self, cls):
        """Return the output file name of the wrapper file (without extension).
        """
        return "%s_wrappers"%cls.alias

    # _classDecl
    def _classDecl(self, decl):
        """Return the class declaration that belongs to a member declaration.
        """
        while decl.parent!=None:
            parent = decl.parent
            if isinstance(parent, declarations.class_t):
                return parent
            decl = parent
        return None


class ArgTransformerBase:
    """Base class for an argument policy class.
    """
    def __init__(self):
        pass

#    def requireInclude(self, include):
#        pass

    def prepareWrapper(self, wm):
        """Wrapper initialization.

        This method is called before the actual wrapper source code is
        generated. This is the place where you can modify the signature
        of the C++ wrapper function or allocate local variables.

        @param wm: Wrapper manager instance
        @type wm: WrapperManager
        """
        pass

    def preCall(self, wm):
        """Generate the C++ code that should be executed before the actual function call.
        
        @param wm: Wrapper manager instance
        @type wm: WrapperManager
        """
        pass

    def postCall(self, wm):
        """Generate the C++ code that should be executed after the actual function call.

        @param wm: Wrapper manager instance
        @type wm: WrapperManager
        """
        pass

    def cleanup(self, wm):
        """Generate code that should be executed in the case of an error.

        This method has to assume that the preCall code was executed but
        the postCall code won't be executed because something went wrong.
        
        <not used yet>
        
        @param wm: Wrapper manager instance
        @type wm: WrapperManager
        """
        pass

# Output
class Output(ArgTransformerBase):
    """Handles a single output variable.

    The specified variable is removed from the argument list and is turned
    into a return value.

    void getSize(int& width, int& height)
      ->
    (width,height) = getSize()
    """
    
    def __init__(self, idx):
        """Constructor.

        @param idx: Index of the argument that is an output value (the first arg has index 1).
        @type idx: int
        """
        ArgTransformerBase.__init__(self)
        self.idx = idx

    def __str__(self):
        return "Output(%d)"%(self.idx)

    def prepareWrapper(self, wm):
        arg = wm.removeArg(self.idx)

        reftype = arg.type
        if not (isinstance(reftype, declarations.reference_t) or
            isinstance(reftype, declarations.pointer_t)):
            raise ValueError, 'Output variable %d ("%s") must be a reference or a pointer (got %s)'%(self.idx, arg.name, arg.type)

        wm.declareLocal(arg.name, str(reftype.base))
        wm.appendResult(arg.name)

        if isinstance(reftype, declarations.pointer_t):
            wm.replaceCallArg(self.idx, "&%s"%arg.name)

# InputArray
class InputArray(ArgTransformerBase):
    """Handle an input array with fixed size.

    void setVec3(double* v)
    ->
    setVec3(object v)   # v must be a sequence with 3 floats
    """
    
    def __init__(self, idx, size):
        ArgTransformerBase.__init__(self)
        self.idx = idx
        self.size = size
        self.argname = None
        self.basetype = None
        self.carray = None
        self.ivar = None

    def __str__(self):
        return "InputArray(%d,%d)"%(self.idx, self.size)

    def prepareWrapper(self, wm):
        # Remove the original argument...
        arg = wm.removeArg(self.idx)
        if not (isinstance(arg.type, declarations.pointer_t) or
                isinstance(arg.type, declarations.array_t)):
            raise ValueError, "Argument %d (%s) must be a pointer."%(self.idx, arg.name)

        # ...and replace it with a Python object.
        newarg = declarations.argument_t(arg.name, "boost::python::object")
        wm.insertArg(self.idx, newarg)

        self.argname = arg.name
        self.basetype = str(arg.type.base).replace("const", "").strip()

        # Declare a variable that will hold the C array...
        self.carray = wm.declareLocal("c_"+arg.name, self.basetype, size=self.size)
        # and a int which is used for the loop
        self.ivar = wm.declareLocal("i", "int")

        wm.replaceCallArg(self.idx, self.carray)

    def preCall(self, wm):
        res = ""
        res += "// Copy the sequence '%s' into '%s'...\n"%(self.argname, self.carray)
        res += 'if (%s.attr("__len__")()!=%d)\n'%(self.argname, self.size)
        res += '{\n'
        res += '  PyErr_SetString(PyExc_ValueError, "Invalid sequence size (expected %d)");\n'%self.size
        res += '  boost::python::throw_error_already_set();\n'
        res += '}\n'
        res += "for(%s=0; %s<%d; %s++)\n"%(self.ivar, self.ivar, self.size, self.ivar)
        res += "  %s[%s] = boost::python::extract< %s >(%s[%s]);"%(self.carray, self.ivar, self.basetype, self.argname , self.ivar)
        return res

# OutputArray
class OutputArray(ArgTransformerBase):
    """Handle an output array with a fixed size.

    void getVec3(double* v)
    ->
    v = getVec3()   # v will be a list with 3 floats    
    """
    
    def __init__(self, idx, size):
        ArgTransformerBase.__init__(self)
        self.idx = idx
        self.size = size
        self.argname = None
        self.basetype = None
        self.pyval = None
        self.cval = None
        self.ivar = None

    def __str__(self):
        return "OutputArray(%d,%d)"%(self.idx, self.size)

    def prepareWrapper(self, wm):
        # Remove the original argument...
        arg = wm.removeArg(self.idx)
        if not (isinstance(arg.type, declarations.pointer_t) or
                isinstance(arg.type, declarations.array_t)):
            raise ValueError, "Argument %d (%s) must be a pointer."%(self.idx, arg.name)

        self.argname = arg.name
        self.basetype = str(arg.type.base).replace("const", "").strip()

        # Declare a variable that will hold the C array...
        self.cval = wm.declareLocal("c_"+self.argname, self.basetype, size=self.size)
        # Declare a Python list which will receive the output...
        self.pyval = wm.declareLocal(self.argname, "boost::python::list")
        # ...and add it to the result
        wm.appendResult(arg.name)

        # Declare an int which is used for the loop
        self.ivar = wm.declareLocal("i", "int")

        wm.replaceCallArg(self.idx, self.cval)

    def postCall(self, wm):
        res = ""
        res += "// Copy the sequence '%s' into '%s'...\n"%(self.cval, self.pyval)
        res += "for(%s=0; %s<%d; %s++)\n"%(self.ivar, self.ivar, self.size, self.ivar)
        res += "  %s.append(%s[%s]);"%(self.pyval, self.cval , self.ivar)
        return res


# WrapperManager
class WrapperManager:
    """
    represents source code for a wrapper function/method
    """

    def __init__(self, decl, transformers):

        self.decl = decl
        self.transformers = transformers
        
        # The original function name
        self.funcname = decl.name

        # Create the arg list of the original function (including the
        # return type)
        argnames = map(lambda a: a.name, decl.arguments)
        if str(decl.return_type)=="void":
            ret = None
            argnames = [None]+argnames
        else:
            resname = self._makeNameUnique("res", argnames)
            ret = declarations.argument_t(resname, decl.return_type, None)
            argnames = [resname]+argnames
        self.funcargs = [ret]+decl.arguments
        # Initialize the argument strings used for invoking the original
        # function
        self.callargs = argnames

        # The name of the wrapper function
        self.wrappername = "%s_wrapper"%decl.name
        # The arguments of the wrapper function
        self.wrapperargs = copy.copy(self.funcargs)

        # Result tuple
        self.resulttuple = []

        self.declared_vars = {}
        self.local_var_list = []

        # Return the original return value by default
        if self.funcargs[0]!=None:
            self.appendResult(self.funcargs[0].name)

        for trans in self.transformers:
            trans.prepareWrapper(self)


    # removeArg
    def removeArg(self, idx):
        """Remove an argument from the wrapper function.

        @param idx: Argument index of the original function
        @type idx: int
        @returns: Returns the argument_t object that was removed (or None
          if idx is 0 and the function has no return type)
        """
        if idx==0 and self.funcargs[0]==None:
            raise ValueError, 'Function "%s" has no return value'%self.decl
        argname = self.funcargs[idx].name
        for i,arg in enumerate(self.wrapperargs):
            if arg!=None:
                if arg.name==argname:
                    res = self.wrapperargs[i]
                    if i>0:
                        del self.wrapperargs[i]
                    else:
                        self.wrapperargs[0] = None
                    return res

        raise ValueError, 'Argument %d ("%s") not found on the wrapper function'%(idx, argname)

    # insertArg
    def insertArg(self, idx, arg):
        """Insert a new argument into the argument list of the wrapper function.
        
        @param idx: New argument index 
        @type idx: int
        @param arg: New argument object
        @type arg: argument_t
        """
        if idx>=0:
            self.wrapperargs.insert(idx, arg)
        else:
            self.wrapperargs.insert(len(self.wrapperargs)+idx+1, arg)

    # appendResult
    def appendResult(self, varname):
        self.resulttuple.append(varname)

    # removeResult
    def removeResult(self, varname):
        self.resulttuple.remove(varname)

    # replaceCallArg
    def replaceCallArg(self, idx, callstr):
        self.callargs[idx] = callstr
        
    # declareLocal
    def declareLocal(self, name, type, size=None, default=None):
        """Declare a local variable and return its final name.
        """
        name = self._makeNameUnique(name, self.declared_vars)
        self.declared_vars[name] = (type,size,default)
        self.local_var_list.append((name, type, size, default))
        return name

    # create
    def create(self):
        """
        Returns the signature id, the prototype and the function definition.
        """

        # Declare the args that are "output" only and that have been
        # converted to local variables...
#        newargnames = map(lambda a: a.name, self.wrapperargs[1:])
#        for a in self.funcargs[1:]:
#            if a.name not in newargnames:
#                self.declareLocal(a.name, str(a.type))

        if self.funcargs[0]!=None:
            self.declareLocal(self.funcargs[0].name, self.funcargs[0].type)

        if len(self.resulttuple)==0:
            rettype = "void"
        elif len(self.resulttuple)==1:
            varname = self.resulttuple[0]
            rettype = self.declared_vars[varname][0]
        elif len(self.resulttuple)>1:
            rettype = "boost::python::tuple"
#            self.wrapperargs[0].type = boost_python_t("tuple")
            
#        if self.wrapperargs[0]==None:
#            rettype = "void"
#        else:
#            rettype = str(self.wrapperargs[0].type)
            
        # Function header...
        a = map(lambda a: str(a), self.wrapperargs[1:])
        asig = map(lambda a: str(a.type), self.wrapperargs[1:])
        cls = self._classDecl(self.decl)
        if cls!=None:
            a = ["%s& self"%cls.name]+a
            asig = ["%s&"%cls.name]+asig
        proto = "%s %s(%s)"%(rettype, self.wrappername, ", ".join(a))
        sigid = "%s(%s)"%(self.wrappername, ", ".join(asig))
        # Remove the default values for the function definition...
        a2 = map(lambda s: s.split("=")[0], a)
        proto2 = "%s %s(%s)"%(rettype, self.wrappername, ", ".join(a2))
        res = "%s\n{\n"%(proto2)

        # Add the declarations...
        for varname,type,size,default in self.local_var_list:
            if default==None:
                res += "  %s %s"%(type, varname)
            else:
                res += "  %s %s = %s"%(type, varname, default)
            if size!=None:
                res += "[%d]"%size
            res += ";\n"
        res += "\n"

        # Precall code...
        src = map(lambda trans: trans.preCall(self), self.transformers)
        src = filter(lambda x: x!=None, src)
        src = "\n\n".join(src)
        res += self._indent(2, src)
#        for trans in self.transformers:
#            code = trans.preCall(self)
#            if code!=None:
#                res += self._indent(2, code)
            
        # Add the function call...
        res += "\n  // Invoke the wrapped function\n"
        res += "  "
        if self.callargs[0]!=None:
            res += "%s = "%self.callargs[0]
        if cls!=None:
            res += "self."
        res += "%s(%s);\n\n"%(self.funcname, ", ".join(self.callargs[1:]))

        # Postcall code...
        src = map(lambda trans: trans.postCall(self), self.transformers)
        src = filter(lambda x: x!=None, src)
        src = "\n\n".join(src)
        res += self._indent(2, src)
#        for trans in self.transformers:
#            code = trans.postCall(self)
#            if code!=None:
#                res += self._indent(2, code)

        # Return the result
        if len(self.resulttuple)==1:
            res += "  return %s;\n"%self.resulttuple[0]
        elif len(self.resulttuple)>1:
            res += "  return boost::python::make_tuple(%s);\n"%(", ".join(self.resulttuple))
        
        res += "}\n"
        return sigid, proto, res.replace("\n\n\n", "\n\n")

    # _indent
    def _indent(self, n, code):
        """Indent source code.
        """
        if code=="":
            return ""
        return "\n".join(map(lambda s: (n*" ")+s, code.split("\n")))+"\n"

    # _classDecl
    def _classDecl(self, decl):
        """Return the class declaration that belongs to a member declaration.
        """
        while decl.parent!=None:
            parent = decl.parent
            if isinstance(parent, declarations.class_t):
                return parent
            decl = parent
        return None

    # _makeNameUnique
    def _makeNameUnique(self, name, names):
        """Make a variable name unique so that there's no clash with other names.

        names must be a dict or a list.
        """
        if name not in names:
            return name

        n = 2
        while 1:
            newname = "%s_%d"%(name, n)
            if newname not in names:
                return newname
            n += 1

        