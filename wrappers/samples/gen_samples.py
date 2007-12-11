# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# Library Imports
from pygccxml import declarations

# Project Imports
import buildfiles.wrap as wrap

def generate(module_builder, local_ns, global_ns):
    """
    global_ns: is the module builder for the entire library
    local_ns: is the namespace that coresponds to the given namespace
    """
    local_ns.include()

    # Free function which deals with a std::vector<boost::shared_ptr<T> >
    f = local_ns.free_fun( 'funcVectorShared' )
    wrap.fix_shared_ptr_vector(declarations.remove_declarated(f.return_type))

    # CodeInject Class
    CodeInject = local_ns.class_('CodeInject')

    # Return type only function
    CodeInject.add_declaration_code("""
  int injectedFunc( samples::CodeInject& code ){
      return code.getValue();
  }
    """);
    CodeInject.add_registration_code(
        'def( "injectedFunc", &::injectedFunc )', works_on_instance = True)
                                     
    # Return and set type function
    CodeInject.add_declaration_code("""
    int otherInjectedFunc(samples::CodeInject& code, int value) {
        return code.setValue(value);
    }
    """);
    CodeInject.add_registration_code(
        'def("otherInjectedFunc", (int (*)(samples::CodeInject&, int))(&::otherInjectedFunc))',
        works_on_instance = True)


    # This does not work, because its append with a ".bp..."
    # if it wasn't this would work perfectly
    #CodeInject.add_registration_code(
    #    'bp::implicitly_convertible< int, samples::CodeInject >()')

    wrap.add_needed_includes([CodeInject])
    wrap.set_implicit_conversions([CodeInject], False)

    return ['include/SharedPtrVector.h']

