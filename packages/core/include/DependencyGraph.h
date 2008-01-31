/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/DependencyGraph.h
 */

#ifndef RAM_CORE_DEPENDENCYGRAPH_H_01_18_2008
#define RAM_CORE_DEPENDENCYGRAPH_H_01_18_2008

// STD Includes
#include <ostream>
#include <string>
#include <vector>
#include <map>

// Project Includes
#include "core/include/ConfigNode.h"

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {
    
/** Determine the start up order from the sections of a config file
 *
 *  This uses a Topological sort to dervie the startup order a given set of
 *  sections.  The config must be the of the following format:
 * 
 *    SectionA:
 *        depends_on: ["SectionB", "SectionC"]
 *    SectionB:
 *        depends_on: ["SectionC"]
 *    SectionC:
 *        data: 5
 *
 *  The depends_on entry lists the names that the given section depends on.
 * 
 */
class RAM_EXPORT DependencyGraph
{
public:
    /** Generate a dependency graph from given config */
    DependencyGraph(ram::core::ConfigNode config);

    /** The order to start the sections from the config file */
    std::vector<std::string> getOrder();

    /** Get the sections which the current section depends on */
    std::vector<std::string> getDependencies(std::string section);

    /** Writes the dependency graph in Graphviz dot format */
    void writeDependencyGraph(std::ostream& file);
    
private:
    /** Stores which sections on dependent on the current section */
    std::map<std::string, std::vector<std::string> > m_sectionDeps;

    /** The order to start the sections in */
    std::vector<std::string> m_order;
};
    
} // namespace core
} // namespace ram

#endif // RAM_CORE_DEPENDENCYGRAPH_H_01_18_2008
