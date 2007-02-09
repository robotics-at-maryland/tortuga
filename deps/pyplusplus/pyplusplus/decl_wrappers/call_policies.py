# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

"""
This modules contains definition of call policies classes. Call policies names
are same, that used in boost.python library.

For every class that implements code creation of call policies, there is a
convinience function.
"""

from pygccxml import declarations
import algorithm

class CREATION_POLICY:
    """Implementation details"""
    AS_INSTANCE = 'as instance'
    AS_TEMPLATE_ARGUMENT = 'as template argument'

class call_policy_t(object):
    """Base class for all call polices classes"""
    def __init__(self):
        object.__init__(self)

    def create(self, function_creator, creation_policy=CREATION_POLICY.AS_INSTANCE):
        """Creates code from the call policies class instance.
        @param function_creator: parent code creator
        @type function_creator: L{code_creators.function_t} or L{code_creators.constructor_t}

        @param creation_policy: indicates whether we this call policy used as template
                                argument or as an instance
        @type creation_policy: L{CREATION_POLICY}
        """
        code = self._create_impl( function_creator )
        if code and creation_policy == CREATION_POLICY.AS_INSTANCE:
            code = code + '()'
        return code

    def create_template_arg( self, function_creator ):
        return self.create( function_creator, CREATION_POLICY.AS_TEMPLATE_ARGUMENT )

    def is_default( self ):
        """Returns True is self is instance of L{default_call_policies_t} class"""
        #Small hack that allows to write nicer code
        return False

    def _create_impl( self, function_creator ):
        raise NotImplementedError()

class default_call_policies_t(call_policy_t):
    """implementation for ::boost::python::default_call_policies"""
    def __init__( self ):
        call_policy_t.__init__( self )

    def _create_impl(self, function_creator ):
        return algorithm.create_identifier( function_creator, '::boost::python::default_call_policies' )

    def is_default( self ):
        return True

    def __str__(self):
        return 'default_call_policies'

def default_call_policies():
    """create ::boost::python::default_call_policies"""
    return default_call_policies_t()

class compound_policy_t( call_policy_t ):
    """base class for all call policies, except default one"""
    def __init__( self, base=None ):
        call_policy_t.__init__( self )
        self._base = base
        if not base:
            self._base = default_call_policies_t()

    def _get_base_policy( self ):
        return self._base
    def _set_base_policy( self, new_policy ):
        self._base = new_policy
    base_policy = property( _get_base_policy, _set_base_policy
                            , doc="base call policy, by default is reference to L{default_call_policies_t} call policy")

    def _get_args(self, function_creator):
        return []

    def _get_name(self, function_creator):
        raise NotImplementedError()

    def _create_impl( self, function_creator ):
        args = self._get_args(function_creator)
        if not self._base.is_default():
            args.append( self._base.create( function_creator, CREATION_POLICY.AS_TEMPLATE_ARGUMENT ) )
        name = algorithm.create_identifier( function_creator, self._get_name(function_creator) )
        return declarations.templates.join( name, args )

    def __str__(self):
        name = self._get_name(None).replace('::boost::python::', '' )
        args = map( lambda text: text.replace( '::boost::python::', '' )
                    , self._get_args( None ) )
        return declarations.templates.join( name, args )

class return_argument_t( compound_policy_t ):
    """implementation for ::boost::python::return_argument call policies"""
    def __init__( self, position=1, base=None):
        compound_policy_t.__init__( self, base )
        self._position = position

    def _get_position( self ):
        return self._position
    def _set_position( self, new_position):
        self._position = new_position
    position = property( _get_position, _set_position )

    def _get_name(self, function_creator):
        if self.position == 1:
            return '::boost::python::return_self'
        else:
            return '::boost::python::return_arg'

    def _get_args(self, function_creator):
        if self.position == 1:
            return []
        else:
            return [ str( self.position ) ]

def return_arg( arg_pos, base=None ):
    return return_argument_t( arg_pos, base )

def return_self(base=None):
    return return_argument_t( 1, base )

class return_internal_reference_t( compound_policy_t ):
    def __init__( self, position=1, base=None):
        compound_policy_t.__init__( self, base )
        self._position = position

    def _get_position( self ):
        return self._position
    def _set_position( self, new_position):
        self._position = new_position
    position = property( _get_position, _set_position )

    def _get_name(self, function_creator):
        return '::boost::python::return_internal_reference'

    def _get_args(self, function_creator):
        return [ str( self.position ) ]

def return_internal_reference( arg_pos=1, base=None):
    return return_internal_reference_t( arg_pos, base )

class with_custodian_and_ward_t( compound_policy_t ):
    def __init__( self, custodian, ward, base=None):
        compound_policy_t.__init__( self, base )
        self._custodian = custodian
        self._ward = ward

    def _get_custodian( self ):
        return self._custodian
    def _set_custodian( self, new_custodian):
        self._custodian = new_custodian
    custodian = property( _get_custodian, _set_custodian )

    def _get_ward( self ):
        return self._ward
    def _set_ward( self, new_ward):
        self._ward = new_ward
    ward = property( _get_ward, _set_ward )

    def _get_name(self, function_creator):
        return '::boost::python::with_custodian_and_ward'

    def _get_args(self, function_creator):
        return [ str( self.custodian ), str( self.ward ) ]

def with_custodian_and_ward( custodian, ward, base=None):
    return with_custodian_and_ward_t( custodian, ward, base )

class with_custodian_and_ward_postcall_t( with_custodian_and_ward_t ):
    def __init__( self, custodian, ward, base=None):
        with_custodian_and_ward_t.__init__( self, custodian, ward, base )

    def _get_name(self, function_creator):
        return '::boost::python::with_custodian_and_ward_postcall'

def with_custodian_and_ward_postcall( custodian, ward, base=None):
    return with_custodian_and_ward_postcall_t( custodian, ward, base )

class return_value_policy_t( compound_policy_t ):
    def __init__( self, result_converter_generator, base=None):
        compound_policy_t.__init__( self, base )
        self._result_converter_generator = result_converter_generator

    def _get_result_converter_generator( self ):
        return self._result_converter_generator
    def _set_result_converter_generator( self, new_result_converter_generator):
        self._result_converter_generator = new_result_converter_generator
    result_converter_generator = property( _get_result_converter_generator
                                           , _set_result_converter_generator )

    def _get_name(self, function_creator):
        return '::boost::python::return_value_policy'

    def _get_args(self, function_creator):
        if function_creator:
            rcg = algorithm.create_identifier( function_creator, self.result_converter_generator )
            return [ rcg ]
        else:
            return [self.result_converter_generator]

copy_const_reference = '::boost::python::copy_const_reference'
copy_non_const_reference = '::boost::python::copy_non_const_reference'
manage_new_object = '::boost::python::manage_new_object'
reference_existing_object = '::boost::python::reference_existing_object'
return_by_value = '::boost::python::return_by_value'
return_opaque_pointer = '::boost::python::return_opaque_pointer'
return_pointee_value = '::pyplusplus::call_policies::return_pointee_value'

def return_value_policy( result_converter_generator, base=None):
    return return_value_policy_t( result_converter_generator, base )

def is_return_opaque_pointer_policy( policy ):
    """returns True is policy represents return_value_policy<return_opaque_pointer>, False otherwise"""
    return isinstance( policy, return_value_policy_t ) \
            and policy.result_converter_generator == return_opaque_pointer

def is_return_pointee_value_policy( policy ):
    """returns True is policy represents return_value_policy<return_pointee_value>, False otherwise"""
    return isinstance( policy, return_value_policy_t ) \
            and policy.result_converter_generator == return_pointee_value
    

class custom_call_policies_t(call_policy_t):
    """implementation for user defined call policies"""
    def __init__( self, call_policies ):
        call_policy_t.__init__( self )
        self.__call_policies = call_policies

    def _create_impl(self, function_creator ):
        return str( self.__call_policies )

    def __str__(self):
        return 'custom call policies'

def custom_call_policies(call_policies):
    """create custom\\user defined call policies"""
    return custom_call_policies_t(call_policies)

class memory_managers:
    none = 'none'
    delete_ = 'delete_'
    all = [ none, delete_ ]
    
    @staticmethod
    def create( manager, function_creator=None):
        mem_manager = 'pyplusplus::call_policies::memory_managers::' + manager
        if function_creator:
            mem_manager = algorithm.create_identifier( function_creator, mem_manager )
        return mem_manager

class convert_array_to_tuple_t( compound_policy_t ):
    def __init__( self, array_size, memory_manager, make_object_call_policies=None, base=None):
        compound_policy_t.__init__( self, base )
        self._array_size = array_size
        self._memory_manager = memory_manager
        self._make_objec_call_policies = make_object_call_policies
    
    def _get_array_size( self ):
        return self._array_size
    def _set_array_size( self, new_array_size):
        self._array_size = new_array_size
    array_size = property( _get_array_size, _set_array_size )

    def _get_memory_manager( self ):
        return self._memory_manager
    def _set_memory_manager( self, new_memory_manager):
        self._memory_manager = new_memory_manager
    memory_manager = property( _get_memory_manager, _set_memory_manager )

    def _get_make_objec_call_policies( self ):
        if None is self._make_objec_call_policies:
            self._make_objec_call_policies = default_call_policies()
        return self._make_objec_call_policies
    def _set_make_objec_call_policies( self, new_make_objec_call_policies):
        self._make_objec_call_policies = new_make_objec_call_policies
    make_objec_call_policies = property( _get_make_objec_call_policies, _set_make_objec_call_policies )


    def _get_name(self, function_creator):
        return '::boost::python::return_value_policy'

    def _get_args(self, function_creator):
        as_tuple_args = [ str( self.array_size ) ]
        as_tuple_args.append( memory_managers.create( self.memory_manager, function_creator ) )
        if not self.make_objec_call_policies.is_default():
            as_tuple_args.append( self.make_objec_call_policies.create_template_arg( function_creator ) )
        as_tuple = '::pyplusplus::call_policies::arrays::as_tuple'
        if function_creator:
            as_tuple = algorithm.create_identifier( function_creator, as_tuple )
        return [ declarations.templates.join( as_tuple, as_tuple_args ) ]
    
def convert_array_to_tuple( array_size, memory_manager, make_object_call_policies=None, base=None ):
    return convert_array_to_tuple_t( array_size, memory_manager, make_object_call_policies, base )