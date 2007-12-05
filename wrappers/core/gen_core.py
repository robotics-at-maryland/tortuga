# Copyright 2004 Roman Yakovenko.
# Distributed under the Boost Software License, Version 1.0. (See
# accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

# STD Imports
import pprint

# Library Imports
from pyplusplus import module_builder

# Project Imports
import buildfiles.wrap as wrap



def export_typedef(ns, typedef):
    cls = ns.typedef(typedef).type.declaration
    cls.rename(typedef)
    cls.include()
    return cls

def expose_publisher(local_ns, cls_name):
    ePublisher = local_ns.class_(cls_name)
    ePublisher.include()

    # Replace 'subscribe' method
    ePublisher.member_function('subscribe').exclude()
    ePublisher.add_declaration_code("""
    void pysubscribe(ram::core::%s & epub, std::string type,
                     boost::python::object pyFunction)
    {
        epub.subscribe(type, EventFunctor(pyFunction));
    }
    """ % (cls_name))
    ePublisher.add_registration_code(
        'def("subscribe", &::pysubscribe)', works_on_instance = True )
    ePublisher.include_files.append('include/EventFunctor.h')
    return ePublisher

def generate(local_ns, global_ns):
    """
    local_ns: is the namespace that coresponds to the given namespace
    global_ns: is the module builder for the entire library
    """

    classes = []

    # ConfigNode
    ConfigNode = local_ns.class_('ConfigNode')
    ConfigNode.include()
    ConfigNode.constructors().exclude()
    classes.append(ConfigNode)

    # Event Subsystem
    EventPublisher = expose_publisher(local_ns, 'EventPublisher')
    classes.append(EventPublisher)

    QueuedEventPublisher = expose_publisher(local_ns, 'QueuedEventPublisher')
    classes.append(QueuedEventPublisher)
    
    Event = local_ns.class_('Event')
    Event.include()
    classes.append(Event)

    export_typedef(local_ns, 'EventPtr')

    # Subsystemx
#    SubsystemPtr = local_ns.typedef('SubsystemPtr').type.declaration
#    SubsystemPtr.rename('SubsystemPtr')
#    SubsystemPtr.include()

    SubsystemList = local_ns.typedef('SubsystemList').type.declaration
    SubsystemList.alias = 'SubsystemList'
    wrap.fix_shared_ptr_vector(SubsystemList)
    SubsystemList.include()
    SubsystemList.include_files.append('core/include/Subsystem.h')
    SubsystemList.include_files.append('boost/python/indexing/value_traits.hpp')
#    classes.append(SubsystemList)
    

    Subsystem = local_ns.class_('Subsystem')
    Subsystem.include()
    classes.append(Subsystem)

    # SubsystemMaker
    SubsystemMaker = local_ns.typedef('SubsystemMaker').type.declaration
    SubsystemMaker.rename('SubsystemMaker')
    SubsystemMaker.include()
    SubsystemMaker.include_files.append('core/include/SubsystemMaker.h')
    SubsystemMaker.include_files.append('iostream')

    SubsystemMakerParamType = \
        local_ns.typedef('SubsystemMakerParamType').type.declaration
    SubsystemMakerParamType.alias = 'SubsystemMakerArgs'
    SubsystemMakerParamType.include()
    SubsystemMakerParamType.include_files.append(
        'core/include/SubsystemMaker.h')

    # Application
    Application = local_ns.class_('Application')
    Application.include()
    # Don't have ofstream wrapped properly
    Application.member_function('writeDependencyGraph').exclude()
    classes.append(Application)
    wrap.set_implicit_conversions([Application, QueuedEventPublisher], False)


    wrap.add_needed_includes(classes)

    #local_ns.class_('Updatable').include()
    #local_ns.class_('IUpdatable').include()
    
#    local_ns.class_('ConfigNode').include()
#    local_ns.class_('ConfigNodeImp').include()
#    local_ns.class_('PythonConfigNodeImp').include()

    #local_ns.typedef('ConfigNodeImpPtr').already_exposed = True
#    global_ns.class_('boost::shared_ptr<ram::core::ConfigNodeImp>').already_exposed = True

#def insert_code(mb):
#    mb.add_registration_code("""
#    bp::register_ptr_to_python<boost::shared_ptr<ram::core::ConfigNodeImpPtr> >();
#    """)

#def generate_code(module_name, files, output_dir, include_files,
#                  extra_includes = []):
#    wrap.generate_code(module_name, files, output_dir, include_files,
#                       extra_includes,
#                       {'core' : generate_core})#,
#                       {'vehicle' : insert_code})
#
