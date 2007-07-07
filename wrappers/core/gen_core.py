# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

import wrap

from pyplusplus import module_builder

def generate_core(name, global_ns, local_ns):
    """
    name: is the name of the module being wrapped (in name::space::form)
    global_ns: is the module builder for the entire library
    local_ns: is the namespace that coresponds to the given namespace
    """

    ConfigNode = local_ns.class_('ConfigNode')
    ConfigNode.include()
    ConfigNode.constructors().exclude()

#    local_ns.class_('IUpdatable').include()
    
#    local_ns.class_('ConfigNode').include()
#    local_ns.class_('ConfigNodeImp').include()
#    local_ns.class_('PythonConfigNodeImp').include()

    #local_ns.typedef('ConfigNodeImpPtr').already_exposed = True
#    global_ns.class_('boost::shared_ptr<ram::core::ConfigNodeImp>').already_exposed = True

def insert_code(mb):
    mb.add_registration_code("""
    bp::register_ptr_to_python<boost::shared_ptr<ram::core::ConfigNodeImpPtr> >();
    """)

def generate_code(module_name, files, output_dir, include_files,
                  extra_includes = []):
    wrap.generate_code(module_name, files, output_dir, include_files,
                       extra_includes,
                       {'core' : generate_core})#,
#                       {'vehicle' : insert_code})
#
