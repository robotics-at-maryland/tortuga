# Copyright 2006 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# Initial author: Matthias Baas

"""This module contains the L{code_manager_t} and L{wrapper_code_manager_t} classes.
"""

import os
import types
import string

# code_manager_t
class code_manager_t:
    """This class manages pieces of source code for a C++ function.

    The class mainly provides the interface for the code blocks to
    manipulate a function and stores the actual substitution variables.
    Each function has its own code manager instance.

    A code block can declare a local variable using L{declare_variable()}
    and it can manipulate one of the attributes that are used to
    initialize the final variables (see the documentation of the
    instance variables below). The final variables (such as RET_TYPE,
    FUNC_NAME, ARG_LIST, etc.) are stored as regular attributes of the
    object.    

    The functionality to perform a text substitution (using the
    substitution() method) is inherited
    from the class L{subst_t}.

    @ivar class_name: The name of the class of which the generated function is a member. A value of None or an empty string is used for free functions. This attribute is used for creating the CLASS_SPEC variable.
    @type class_name: str
    @ivar ret_type: Return type. The value may be any object where str(obj) is valid C++ code. The value None corresponds to void. This will be the value of the variable RET_TYPE.
    @type ret_type: str
    @ivar arg_list: The argument list. The items are pygccxml argument_t objects. This list will appear in the variables ARG_LIST, ARG_LIST_DEF and ARG_LIST_TYPES.
    @type arg_list: list of L{argument_t<pygccxml.declarations.calldef.argument_t>}
    @ivar input_params: A list of strings that contain the input parameter for the function call. This list is used for the INPUT_PARAMS variable.
    @type input_params: list of str
    @ivar result_var: The name of the variable that will receive the result of the function call. If None, the return value is ignored. This attribute will be used for the variable RESULT_VAR_ASSIGNMENT.
    @type result_var: str
    @ivar result_type: The type of the variable 'result_var'
    @type result_type: str
    @ivar result_expr: A string containing the expression that will be put after the "return" statement. This expression is used for the variable RETURN_STMT.
    @type result_expr: str
    @ivar exception_handler_exit: The C++ code that is executed at the end of the main exception handler (default: "throw;").
    @type exception_handler_exit: str

    @author: Matthias Baas
    """

    def __init__(self):
        """Constructor."""      
        # The name of the class of which the generated function is a member
        # (pass None or an empty string if the function is a free function)
        self.class_name = None

        # Return type (the value may be any object where str(obj) is valid
        # C++ code. The value None corresponds to "void").
        self.ret_type = None
        # The argument list. The items are argument_t objects.
        self.arg_list = []

        # A list of strings that contain the input parameter for the
        # function call
        self.input_params = []

        # The name of the variable that will receive the result of the
        # function call. If None, the return value is ignored.
        self.result_var = None

        # The type of 'result_var'
        self.result_type = "void"

        # A string containing the expression that will be put after
        # the "return" statement.
        self.result_expr = None

        # The C++ code that is executed at the end of the main
        # exception handler.
        self.exception_handler_exit = "throw;"

        # Key:Variable name / Value:(type,size,default)
        self._declared_vars = {}

    def substitute( self, template_code ):
        tmpl = string.Template(template_code)
        return tmpl.safe_substitute(self.__dict__)

    # declare_variable
    def declare_variable(self, name, type, initialize_expr=''):
        """Declare a local variable and return its final name.

        @param name: The desired variable name
        @type name: str
        @param type: The type of the variable (must be valid C++ code)
        @type type: str
        @param size: The array length or None
        @type size: int
        @param default: The default value (must be valid C++ code) or None
        @type default: str
        @return: The assigned variable name (which is guaranteed to be unique)
        @rtype: str
        """
        name = self._make_name_unique(name)
        self._declared_vars[name] = (type, initialize_expr)
        return name

    def init_variables(self):
        """Initialize the substitution variables.

        Based on the (lowercase) attributes, the final (uppercase)
        variables are created. Those variables are regular string
        attributes.
        """

        # CLASS_SPEC
        if (self.class_name in [None, ""]):
            self.CLASS_SPEC = ""
        else:
            self.CLASS_SPEC = "%s::"%self.class_name

        # RET_TYPE
        if self.ret_type==None:
            self.RET_TYPE = "void"
        else:
            self.RET_TYPE = str(self.ret_type)

        # ARG_LIST_DEF
        args = map(lambda a: str(a), self.arg_list)
        self.ARG_LIST_DEF = ", ".join(args)

        # ARG_LIST
        args = map(lambda s: s.split("=")[0], args)
        self.ARG_LIST = ", ".join(args)

        # ARG_LIST_TYPES
        args = map(lambda a: str(a.type), self.arg_list)
        self.ARG_LIST_TYPES = ", ".join(args)

        # Create the declaration block -> DECLARATIONS
        vardecls = []
        for name, (type, initialize_expr) in self._declared_vars.iteritems():
            tmpl = "%(type)s %(name)s%(initialize_expr)s;"
            vardecls.append( tmpl % { 'type' : type
                                      , 'name' : name
                                      , 'initialize_expr' : initialize_expr } )
        self.DECLARATIONS = os.linesep.join(vardecls)

        # RESULT_VAR_ASSIGNMENT
        if self.result_var!=None:
            self.RESULT_VAR_ASSIGNMENT = "%s = "%self.result_var
        else:
            self.RESULT_VAR_ASSIGNMENT = ""

        # RESULT_TYPE
        if self.result_type!=None:
            self.RESULT_TYPE = str(self.result_type)
        else:
            self.RESULT_TYPE = ""

        # INPUT_PARAMS
        self.INPUT_PARAMS = ", ".join(self.input_params)

        # RETURN_STMT
        if self.result_expr!=None:
            self.RETURN_STMT = "return %s;"%self.result_expr
        else:
            self.RETURN_STMT = ""

        # EXCEPTION_HANDLER_EXIT
        if self.exception_handler_exit!=None:
            self.EXCEPTION_HANDLER_EXIT = self.exception_handler_exit
        else:
            self.EXCEPTION_HANDLER_EXIT = ""

    # _make_name_unique
    def _make_name_unique(self, name):
        """Make a variable name unique so that there's no clash with other names.

        @param name: The variable name that should be unique
        @type name: str
        @return: A unique name based on the input name
        @rtype: str
        """
        n = 2
        newname = name
        while 1:
            if not self._declared_vars.has_key( newname ):
                return newname
            newname = "%s_%d"%(name, n)
            n += 1

# wrapper_code_manager_t
class wrapper_code_manager_t(code_manager_t):
    """The CodeManager class for the wrapper function.

    In contrast to a regular C++ function a Python function can return
    several values, so this class provides the extra attribute "result_exprs"
    which is a list of individual expressions. Apart from that this
    class is identical to L{code_manager_t}.

    @ivar result_exprs: Similar to result_expr but this list variable can contain more than just one result. The items can be either strings containing the variable names (or expressions) that should be returned or it can be an L{argument_t<pygccxml.declarations.calldef.argument_t>} object (usually from the argument list of the virtual function) whose name attribute will be used. This attribute only exists on the code manager for the wrapper function (the virtual function cannot return several values, use result_expr instead).
    @type result_exprs: list of str or L{argument_t<pygccxml.declarations.calldef.argument_t>}
    """

    def __init__(self):
        """Constructor.
        """
        code_manager_t.__init__(self)

        # Similar to result_expr but now there can be more than just one result
        # The items can be either strings containing the variable names (or
        # expressions) that should be returned or it can be an argument_t
        # object (usually from the argument list of the virtual function)
        # whose name attribute will be used.
        self.result_exprs = []

    def init_variables(self):
        """Initialize the substitution variables.
        """

        # Prepare the variables for RET_TYPE and RETURN_STMT...

        # Convert all items into strings
        result_exprs = []
        for re in self.result_exprs:
            # String?
            if isinstance(re, types.StringTypes):
                result_exprs.append(re)
            # argument_t
            else:
                result_exprs.append(re.name)

        if self.result_expr==None:
            # No output values?
            if len(result_exprs)==0:
                self.ret_type = None
                self.result_expr = None
            # Exactly one output value?
            elif len(result_exprs)==1:
                self.ret_type = "boost::python::object"
                self.result_expr = "boost::python::object(%s)"%result_exprs[0]
            # More than one output value...
            else:
                self.ret_type = "boost::python::object"
                self.result_expr = "boost::python::make_tuple(%s)"%(", ".join(result_exprs))

        # Invoke the inherited method that sets the actual variables
        code_manager_t.init_variables(self)

        
