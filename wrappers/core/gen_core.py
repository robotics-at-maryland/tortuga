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
    EventPublisher = local_ns.class_('EventPublisher')
    EventPublisher.include()

    # Replace 'subscribe' method
    EventPublisher.member_function('subscribe').exclude()
    EventPublisher.add_declaration_code("""
    struct EventFunctor {
        EventFunctor(boost::python::object pyObject) : pyFunction(pyObject) {}
    
        void operator()(ram::core::EventPtr event)
        {
            pyFunction(event);
        }

        boost::python::object pyFunction;
    };
    
    void pysubscribe(ram::core::EventPublisher& epub, std::string type,
                     boost::python::object pyFunction)
    {
        epub.subscribe(type, EventFunctor(pyFunction));
    }
    """)
    EventPublisher.add_registration_code(
        'def("subscribe", &::pysubscribe)', works_on_instance = True )
    classes.append(EventPublisher)
    
    Event = local_ns.class_('Event')
    Event.include()
    classes.append(Event)

    export_typedef(local_ns, 'EventPtr')

    # Subsystem
#    SubsystemPtr = local_ns.typedef('SubsystemPtr').type.declaration
#    SubsystemPtr.rename('SubsystemPtr')
#    SubsystemPtr.include()

#    SubsystemList = local_ns.typedef('SubsystemList').type.declaration
#    SubsystemList.alias = 'SubsystemList'
#    SubsystemList.include()

    Subsystem = local_ns.class_('Subsystem')
    Subsystem.include()
    classes.append(Subsystem)

    # SubsystemMaker
    SubsystemMaker = local_ns.typedef('SubsystemMaker').type.declaration
    SubsystemMaker.rename('SubsystemMaker')
    SubsystemMaker.include()
    SubsystemMaker.include_files.append('core/include/SubsystemMaker.h')
    SubsystemMaker.include_files.append('iostream')
    
    # Handle newObject
    SubsystemMaker.member_function('newObject').exclude()
    SubsystemMaker.add_declaration_code("""
    ram::core::SubsystemPtr pynewObject(boost::python::object cfg,
                                        boost::python::object subsystems)
    {
        // Build the list of subsystems
        ram::core::SubsystemList deps;

        int size = boost::python::len(subsystems);
        for (int i = 0; i < size; ++i)
        {
            ram::core::SubsystemPtr subsystem =
                boost::python::extract<ram::core::SubsystemPtr>(subsystems[i]);
            deps.insert(subsystem);
        }

        // Create the ConfigNode
        std::string cfg_str = boost::python::extract<std::string>(
            boost::python::str(cfg));

        std::pair<ram::core::ConfigNode, ram::core::SubsystemList>
            params(ram::core::ConfigNode::fromString(cfg_str), deps);

        return ram::core::SubsystemMaker::newObject(params);
    }
    """)
    SubsystemMaker.add_registration_code(
        'def("newObject", &::pynewObject).staticmethod("newObject")',)
            
    # Can't seem to find this function for some reason
    #print SubsystemMaker.member_function('makeObject').exclude()

    # Application
    Application = local_ns.class_('Application')
    Application.include()
    # Don't have ofstream wrapped properly
    Application.member_function('writeDependencyGraph').exclude()
    classes.append(Application)
    wrap.set_implicit_conversions([Application], False)


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
