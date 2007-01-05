# Copyright 2006 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# Initial author: Matthias Baas

"""This module contains the L{substitution_manager_t} class.
"""

import os
from pygccxml import declarations
from code_manager import code_manager_t, wrapper_code_manager_t
from transformer import transformer_t
from transformer import return_

# substitution_manager_t
class substitution_manager_t:
    """Helper class for creating C++ source code for wrapper functions.

    The class does not create the entire function source code itself
    but it maintains the individual parts that can be composed by the
    user of the class. Those individual parts are stored inside
    variables which can be used to perform text substitutions.

    Doing substitutions
    ===================

    Here is an example that demonstrates the usage of the class. The
    user creates a template string that contains the layout of the
    entire wrapper function. Such a template string may look like
    this::

      $RET_TYPE $CLASS_SPEC$FUNC_NAME($ARG_LIST)
      {
        $DECLARATIONS
        
        $PRE_CALL
        
        $RESULT_VAR_ASSIGNMENT$CALL_FUNC_NAME($INPUT_PARAMS);
        
        $POST_CALL
        
        $RETURN_STMT
      }

    Any part of the function that is not fixed, i.e. that can be
    modified by argument policies, is specified via a variable. The
    substitution manager can now be used to substitute the variables with
    their actual value. There are actually two sets of identical
    variables, one for the wrapper function and one for the virtual
    function. You choose a set either by using the L{subst_wrapper()} or
    L{subst_virtual()} method for doing the substitution. For example,
    performing a "wrapper" substitution on the above template string
    might result in the following code::

      boost::python::object Spam_wrapper::foo_wrapper(Spam& self, int mode)
      {
        int result;
        int w;
        int h;
        
        result = self.foo(w, &h, mode);
        
        return boost::python::make_tuple(result, w, h);
      }

    In this example, the individual variables have the following values: 

     - RET_TYPE = C{boost::python::object}
     - CLASS_SPEC = C{Spam_wrapper::}
     - FUNC_NAME = C{foo_wrapper}
     - ARG_LIST = C{Spam& self, int mode}
     - DECLARATIONS = C{int result;\\nint w;\\nint h;}
     - PRE_CALL = <empty> 
     - RESULT_VAR_ASSIGNMENT = C{result =}
     - CALL_FUNC_NAME = C{self.foo}
     - INPUT_PARAMS = C{w, &h, mode}
     - POST_CALL = <empty> 
     - RETURN_STMT = C{return boost::python::make_tuple(result, w, h);}


    Modifying the variables
    =======================

    In addition to the actual user of the class (who wants to do text
    substitutions), the class is also used by the arg policies (code blocks)
    to modify the variables.
    There are two attributes L{wrapper_func} and L{virtual_func} that are
    used to modify either the wrapper or the virtual function. If the
    signature of the wrapper needs modification this should be done via
    the methods L{remove_arg()} and L{insert_arg()} and not via the
    wrapper_func or virtual_func attributes because this affects the
    virtual function as well (because the virtual function makes a call
    to the Python function).

    Variables
    =========


     - RET_TYPE: The return type (e.g. "void", "int", "boost::python::object")
 
     - CLASS_SPEC: "<classname>::" or empty

     - FUNC_NAME: The name of the wrapper or virtual function.

     - ARG_LIST: The parameters for $FUNC_NAME (including self if required)

     - ARG_LIST_DEF: Like ARG_LIST, but including default values (if there are any)

     - ARG_LIST_TYPES: Like ARG_LIST but the variable names are left out and only the types are listed (this can identify a particular signature).

     - DECLARATIONS: The declaration block

     - PRE_CALL::

         +--------------------------+
         | [try {]                  | 
         +--------------------------+
         |  Pre-call code block 1   |
         +--------------------------+
         |  Pre-call code block 2   |
         +--------------------------+
         |  ...                     |
         +--------------------------+
         |  Pre-call code block n   |
         +--------------------------+

     - RESULT_VAR_ASSIGNMENT:  "<varname> = " or empty

     - RESULT_TYPE: The type of the result variable 

     - CALL_FUNC_NAME: The name of the function that should be invoked (self?).

     - INPUT_PARAMS:  The values or variables that will be passed to $FUNC_NAME,
                     e.g. "a, b" or "0.5, 1.5" etc

     - POST_CALL::

         +--------------------------+
         |  Post-call code block n  |
         +--------------------------+
         |  ...                     |
         +--------------------------+
         |  Post-call code block 2  |
         +--------------------------+
         |  Post-call code block 1  |
         +--------------------------+
         | [} catch(...) {...}]     |
         +--------------------------+

     - CLEANUP:  The cleanup code blocks of all function transformers.

     - RETURN_STMT:  "return <varname>" or "return boost::python::make_tuple(...)"

     - EXCEPTION_HANDLER_EXIT: The C++ code that is executed at the end of the main exception handler (default: "throw;")
    

    @ivar wrapper_func: The L{code manager<code_manager_t>} object that manages the wrapper function. This is used by the arg policies to modify the wrapper function.
    @type wrapper_func: L{wrapper_code_manager_t}
    @ivar virtual_func: The L{code manager<code_manager_t>} object that manages the virtual function. This is used by the arg policies to modify the virtual function.
    @type virtual_func: L{code_manager_t}
    
    @group Methods called by the user of the class: append_code_block, subst_wrapper, subst_virtual
    @group Methods called by the function transformers: remove_arg, insert_arg, py_result_expr

    @author: Matthias Baas
    """

    def __init__(self, decl, wrapper_class=None, transformers=None):
        """Constructor.

        @param decl: calldef declaration
        @type decl: calldef_t
        @param wrapper_class: The name of the class the generated function should belong to (or None if the generated function should be a free function)
        @type wrapper_class: str
        @param transformers: Function transformer objects
        @type transformers: list of transformer_t
        """
        #prevent recursive import
        from pyplusplus import decl_wrappers
        # Code manager for the virtual function
        self.virtual_func = code_manager_t()
        # Code manager for the wrapper function
        self.wrapper_func = wrapper_code_manager_t()

        # The declaration that represents the original C++ function
        self.decl = decl

        # The function transformers
        if transformers==None:
            transformers = []
        self.transformers = transformers
        
        self.wrapper_class = wrapper_class

        # Initialize the code managers...

        self.virtual_func.arg_list = decl.arguments[:]
        self.virtual_func.class_name = wrapper_class
        self.virtual_func.FUNC_NAME = decl.name
        self.virtual_func.CALL_FUNC_NAME = decl.name
        self.virtual_func.input_params \
            = map( lambda arg: decl_wrappers.python_traits.call_traits( arg.type ) % arg.name
                   , decl.arguments )

        self.wrapper_func.arg_list = decl.arguments[:]
        self.wrapper_func.class_name = wrapper_class
        self.wrapper_func.FUNC_NAME = "%s_wrapper"%decl.alias
        self.wrapper_func.CALL_FUNC_NAME = decl.name
        self.wrapper_func.input_params = map(lambda a: a.name, decl.arguments)

        if str(decl.return_type)=="void":
            ret_type = None
        else:
            ret_type = decl.return_type
            self.wrapper_func.result_type = str(declarations.type_traits.remove_reference(ret_type))
            self.wrapper_func.result_var = self.wrapper_func.declare_variable("result", self.wrapper_func.result_type)
            self.wrapper_func.result_exprs = [self.wrapper_func.result_var]

        self.virtual_func.ret_type = ret_type

        self.wrapper_func.ret_type = ret_type

        # The offset that is added to the index in insert_arg()
        # This index is either 0 for free functions or 1 for member functions
        # because the "self" argument is not a regular argument.
        self._insert_arg_idx_offset = 0

        # Check if we're dealing with a member function...
        if isinstance( decl.parent, declarations.class_t ):
            if decl.has_static:
                self.wrapper_func.CALL_FUNC_NAME \
                    = "%s::%s" % (declarations.full_name(decl.parent), self.wrapper_func.CALL_FUNC_NAME)                
            else:
                selfname = self.wrapper_func._make_name_unique("self")
                selfarg = declarations.argument_t(selfname, declarations.dummy_type_t("%s&"%declarations.full_name(decl.parent)))
                self.wrapper_func.arg_list.insert(0, selfarg)
                self.wrapper_func.CALL_FUNC_NAME = "%s.%s"%(selfname, self.wrapper_func.CALL_FUNC_NAME)
                self._insert_arg_idx_offset = 1

        # Argument index map
        # Original argument index ---> Input arg index  (indices are 0-based!)
        # Initial state is the identity:  f(x) = x
        # The argument index map represents a function that maps the argument
        # index of the original C++ function to the index of the corresponding
        # parameter in the input parameter list for the Python call.
        self.argidxmap = range(len(decl.arguments))


        # Flag that is set after the functions were initialized
        self._funcs_initialized = False
    
    # init_funcs
    def init_funcs(self):
        """Initialize the virtual function and the wrapper function.

        After this method has been called, the substitution variables
        are ready for usage.

        It is not necessary to call this method manually, it is
        automatically called at the time a substitution is requested.
        """

        # Append the default return_virtual_result_t code modifier
        transformers = self.transformers+[return_virtual_result_t(self.decl)]

        for cb in transformers:
            if hasattr(cb, "init_funcs"):
                cb.init_funcs(self)

        # Create a variable that will hold the result of the Python call
        # inside the virtual function.
        if len(self.wrapper_func.result_exprs)>0:
            self.virtual_func.result_type = "boost::python::object"
            self.virtual_func.result_var = self.virtual_func.declare_variable("pyresult", self.virtual_func.result_type)

        self.wrapper_func.init_variables()
        self.virtual_func.init_variables()

        self._funcs_initialized = True

        block_sep = os.linesep * 2
        # Create the wrapper function pre-call block...
        tmp = filter(None, map(lambda cb: cb.wrapper_pre_call(self), transformers) )  
        self.wrapper_func.PRE_CALL = block_sep.join( tmp )

        # Create the wrapper function post-call block...
        tmp = filter(None, map(lambda cb: cb.wrapper_post_call(self), transformers) )        
        self.wrapper_func.POST_CALL = block_sep.join( tmp )

        # Create the wrapper function cleanup block...
        tmp = filter(None, map(lambda cb: cb.wrapper_cleanup(self), transformers) )        
        self.wrapper_func.CLEANUP = block_sep.join( tmp )

        # Create the virtual function pre-call block...
        tmp = filter(None, map(lambda cb: cb.virtual_pre_call(self), transformers) )        
        self.virtual_func.PRE_CALL = block_sep.join( tmp )

        # Create the virtual function post-call block...
        tmp = filter(None, map(lambda cb: cb.virtual_post_call(self), transformers) )        
        self.virtual_func.POST_CALL = block_sep.join( tmp )
        
        # Create the virtual function cleanup block...
        tmp = filter(None, map(lambda cb: cb.virtual_cleanup(self), transformers) )        
        self.virtual_func.CLEANUP = block_sep.join( tmp )

    # remove_arg
    def remove_arg(self, idx):
        """Remove an argument from the wrapper function.

        This function can also be used to remove the original return value
        (idx=0).

        The function is supposed to be called by function transformer
        objects.

        @param idx: Argument index of the original function (may be negative)
        @type idx: int
        @returns: Returns the argument_t object that was removed (or None
          if idx is 0 and the function has no return type). You must not
          modify this object as it may still be in use on the virtual
          function.
        @rtype: L{argument_t<pygccxml.declarations.calldef.argument_t>}
        """
        if self._funcs_initialized:
            raise ValueError, "remove_arg() may only be called before function initialization."
        if idx<0:
            idx += len(self.virtual_func.arg_list)+1
        if idx>=len(self.virtual_func.arg_list)+1:
            raise IndexError, "Index (%d) out of range."%idx

        # Remove original return type?
        if idx==0:
            if id(self.wrapper_func.ret_type)==id(self.wrapper_func.ret_type):
                self.wrapper_func.ret_type = None
                self.wrapper_func.result_exprs.remove(self.wrapper_func.result_var)
            else:
                raise ValueError, 'Argument %d not found on the wrapper function'%(idx)
        # Remove argument...
        else:
            # Get the original argument...
            arg = self.virtual_func.arg_list[idx-1]
            # ...and remove it from the wrapper
            try:
                self.wrapper_func.arg_list.remove(arg)
            except ValueError:
                msg = str(self.decl)+"\n"
                msg += 'Argument %d ("%s") not found on the wrapper function'%(idx, arg.name)
                raise ValueError, msg

            # Remove the input parameter on the Python call in the
            # virtual function.
            paramidx = self.argidxmap[idx-1]
            if paramidx==None:
                raise ValueError, "Argument was already removed"
            del self.virtual_func.input_params[paramidx]
            self.argidxmap[idx-1] = None
            for i in range(idx,len(self.argidxmap)):
                if self.argidxmap[i]!=None:
                    self.argidxmap[i] -= 1
            
            return arg


    # insert_arg
    def insert_arg(self, idx, arg, inputexpr):
        """Insert a new argument into the argument list of the wrapper function.

        This function is supposed to be called by function transformer
        objects.
        
        @param idx: New argument index (may be negative)
        @type idx: int
        @param arg: New argument object
        @type arg: L{argument_t<pygccxml.declarations.calldef.argument_t>}
        @param inputexpr: The expression that is used as input parameter for the Python function call inside the virtual function
        @type inputexpr: str
        """
        if self._funcs_initialized:
            raise ValueError, "insert_arg() may only be called before function initialization."
        if idx==0:
            pass
        else:
            if idx<0:
                idx += len(self.wrapper_func.arg_list)+2

            idx += self._insert_arg_idx_offset
            self.wrapper_func.arg_list.insert(idx-1, arg)

            # What to insert?
            self.virtual_func.input_params.insert(idx-1, inputexpr)
            # Adjust the argument index map
            for i in range(idx-1,len(self.argidxmap)):
                if self.argidxmap[i]!=None:
                    self.argidxmap[i] += 1

    # py_result_expr
    def py_result_expr(self, local_wrapper_var):
        """Return a C++ expression that references one particular item of the result tuple in the virtual function.

        This method is supposed to be used in the virtual_post_call() method
        of function transformers when the result object of the Python call
        is required. It relieves the transformer from having to check whether
        its value is the only output value or not (in the former case the
        value is an arbitrary Python object, in the latter case it is always
        a tuple).

        For example, if a function transformer allocates a variable and
        adds it to the result expressions of the wrapper function (so that
        the wrapper function returns the value of this variable) then
        the corresponding virtual function has to receive this value
        and convert it to a C++ value. Suppose the result of invoking
        the Python function inside the virtual function is stored in
        the variable "pyresult" (which is of type "object"). The transformer
        object that has to access its output value has to distinguish
        two cases:

         - Its output value was the only output, then it references the
           output value simply with "pyresult".
         - There are also other output values, then it references its
           output value with "pyresult[n]" where n is the position of
           the output variable in the result tuple.

        To relieve the transformer from the burden of having to perform
        this test, py_result_expr() can be used which generates the
        appropriate expression.        

        @param local_wrapper_var: The name of the local variable inside the wrapper that is part of the result tuple
        @type local_wrapper_var: str
        @return: The C++ expression that references the result item
        @rtype: str
        """
        # Get the name of the result variable
        pyresult = self.virtual_func.result_var
        # Determine the index of the requested local wrapper variable
        try:
            idx = self.wrapper_func.result_exprs.index(local_wrapper_var)
        except ValueError:
            raise ValueError, "The local variable '%s' is not returned by the wrapper function."

        # Return the C++ expression
        if len(self.wrapper_func.result_exprs)==1:
            return pyresult
        else:
            return "%s[%d]"%(pyresult, idx)

    # subst_virtual
    def subst_virtual(self, template):
        """Perform a text substitution using the "virtual" variable set.

        @return: Returns the input string that has all variables substituted.
        @rtype: str
        """
        if not self._funcs_initialized:
            self.init_funcs()
        return self.virtual_func.substitute(template)

    # subst_wrapper
    def subst_wrapper(self, template):
        """Perform a text substitution using the "wrapper" variable set.

        @return: Returns the input string that has all variables substituted.
        @rtype: str        
        """
        if not self._funcs_initialized:
            self.init_funcs()
        return self.wrapper_func.substitute(template)



# return_virtual_result_t
class return_virtual_result_t(transformer_t):
    """Extract and return the result value of the virtual function.

    This is an internal code block object that is automatically appended
    to the list of code blocks inside the substitution_manager_t class.
    """

    def __init__(self, function):
        transformer_t.__init__(self, function)
        self.result_var = "<not initialized>"

    def __str__(self):
        return "ReturnVirtualResult()"%(self.idx)

    def init_funcs(self, sm):
        if sm.virtual_func.ret_type==None:
            return
        
        # Declare the local variable that will hold the return value
        # for the virtual function
        self.result_var = sm.virtual_func.declare_variable("result", sm.virtual_func.ret_type)
        # Replace the result expression if there is still the default
        # result expression (which will not work anyway)
        if sm.virtual_func.result_expr==sm.virtual_func.result_var:
            sm.virtual_func.result_expr = self.result_var

    def virtual_post_call(self, sm):
        # Search the result tuple of the wrapper function for the return
        # value of the C++ function call. If the value exists it is extracted
        # from the Python result tuple, converted to C++ and returned from
        # the virtual function. If it does not exist, do nothing.
        try:
            resexpr = sm.py_result_expr(self.result_var)
        except ValueError:
            return
        
        res = "// Extract the C++ return value\n"
        res += "%s = boost::python::extract<%s>(%s);"%(self.result_var, sm.virtual_func.ret_type, resexpr)
        return res        
