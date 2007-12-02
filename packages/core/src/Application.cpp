/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/Application.cpp
 */

#ifdef RAM_WINDOWS
#pragma warning( disable : 4510 ) // Not default constuctor generated (BGL)
#pragma warning( disable : 4610 ) // Another caused by BGL
#endif
 
// STD Includes
#include <utility>

// Library Includes
#include <boost/foreach.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>

// Project Includes
#include "core/include/Application.h"
#include "core/include/ConfigNode.h"
#include "core/include/SubsystemMaker.h"

namespace ram {
namespace core {

Application::Application(std::string configPath)
{
    ConfigNode rootCfg = core::ConfigNode::fromFile(configPath);

    
    if (rootCfg.exists("Subsystems"))
    {
        ConfigNode sysConfig(rootCfg["Subsystems"]);
        NodeNameList subnodes = sysConfig.subNodes();

        // Properly fills m_order, and m_subsystemDeps
        determineStartupOrder(subnodes, sysConfig);
        
        // Create all the subsystems
        BOOST_FOREACH(std::string subsystemName, m_order)
        {
            // Set 'name' properly in the config
            ConfigNode config(sysConfig[subsystemName]);
            config.set("name", subsystemName);

            // Build list of dependencies
            SubsystemList deps;
            BOOST_FOREACH(std::string depName, m_subsystemDeps[subsystemName])
                deps.insert(getSubsystem(depName));

            // Create out new subsystem and store it
            SubsystemPtr subsystem(SubsystemMaker::newObject(
                std::make_pair(config, deps) ));
            
            m_subsystems[subsystemName] = subsystem;            
        }


        // Not sure if this is the right place for this or not
        // maybe another function, maybe a scheduler?
        BOOST_FOREACH(std::string name, m_order)
        {
            ConfigNode cfg(sysConfig[name]);
            if (cfg.exists("update_interval"))
                m_subsystems[name]->background(cfg["update_interval"].asInt());
        }
    }
}

Application::~Application()
{
    // Go through subsystems in the reverse order of contrustuction and shut
    // them down
    for (int i = (((int)m_order.size()) - 1); i >= 0; --i)
    {
        std::string name(m_order[i]);
        SubsystemPtr subsystem = m_subsystems[name];
        subsystem->unbackground(true);

        m_subsystems.erase(name);
    }
}

SubsystemPtr Application::getSubsystem(std::string name)
{
    NameSubsystemMapIter iter = m_subsystems.find(name);
    assert(iter != m_subsystems.end() && "Error subsystem not found");
    return (*iter).second;
}

std::vector<std::string> Application::getSubsystemNames()
{
    return m_order;
}

void Application::writeDependencyGraph(std::ostream& file)
{
    // Graphviz header
    file << "digraph G {" << std::endl;
    
    // Write labels to file and build a translation table
    std::map<std::string, int> nameToVertex;
    for (size_t i = 0; i < m_order.size(); ++i)
    {
        std::string name(m_order[i]);
        nameToVertex[name] = i;
        file << i << "[label=\"" << name << "\"];" << std::endl;
    }

    // The following hoops are to make sure the order is always printed in a
    // consistent manner
    for (size_t i = m_order.size() - m_subsystemDeps.size();
         i < m_order.size(); ++i)
    {
        std::map<std::string, NameList>::iterator iter =
            m_subsystemDeps.find(m_order[i]);
    
        int vertex = nameToVertex[iter->first];
        
        BOOST_FOREACH(std::string name, iter->second)
        {
            file << nameToVertex[name] << "->" << vertex << " ;" << std::endl;
        }
    }

    // Graphiz footer
    file << "}" << std::endl;
}
    
void Application::determineStartupOrder(NodeNameList& subnodes,
                                        ConfigNode sysConfig)
{
    // The vertex_name_t is to allow us to attach subsystem names to vertices
    typedef boost::property<boost::vertex_name_t, std::string,
        // The vertex_color_t is for use by the topological sort
        boost::property<boost::vertex_color_t, boost::default_color_type> >
        VertexProperties;

    typedef boost::adjacency_list<boost::vecS, boost::vecS,
        boost::bidirectionalS, VertexProperties
        > Graph;
    
    typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
    typedef boost::property_map<Graph,boost::vertex_name_t>::type VertexNameMap;
    typedef std::map<std::string, Vertex> NameVertexMap;
    
    // All powerful graph
    Graph graph;
    
    // Translation tables from Subsystem name to vertex, and back again
    NameVertexMap nameToVertex;
    VertexNameMap vertexToName = boost::get(boost::vertex_name, graph);
  
    // Go through all subsystems configs and build the graph
    
    // Records which Subsystems are in the dependency graph
    std::map<std::string, bool> inGraph;
    BOOST_FOREACH(std::string subsystemName, subnodes)
    {
        ConfigNode config(sysConfig[subsystemName]);
        
        // Set new Subsystems to false
        if (inGraph.end() == inGraph.find(subsystemName))
            inGraph[subsystemName] = false;
      
        if (config.exists("depends_on"))
        {
            NameVertexMap::iterator vertexMapPos; 
            bool inserted;
            Vertex subsystemVertex;
            
            // If it depends on something its part of the graph
            inGraph[subsystemName] = true;
            
            // Get the vertex which represents the subsystem, adding it to the
            // graph if necessary
            boost::tie(vertexMapPos, inserted) =
                nameToVertex.insert(std::make_pair(subsystemName, Vertex()));
            if (inserted)
            {
                // Create new vertex for the Subsystem
                subsystemVertex = add_vertex(graph);
                
                // Update the nameToVertex map
                vertexToName[subsystemVertex] = subsystemName;
                vertexMapPos->second = subsystemVertex;
            }
            // We have already added this vertex, just look it up
            else
            {
                subsystemVertex = vertexMapPos->second;
            }
            
            ram::core::ConfigNode depsCfg(config["depends_on"]);
            NameList depNames;
            size_t length =  depsCfg.size();
            for(size_t i = 0; i < length; ++i)
            {
                std::string dependencyName(depsCfg[i].asString());
                depNames.push_back(dependencyName);
                
                // If it has dependants its part of the graph
                inGraph[dependencyName] = true;
              
                Vertex dependencyVertex;
                
                // Preform the same Vertex adding processing as above, but with
                // the dependency subsystems instead
                boost::tie(vertexMapPos, inserted) =
                    nameToVertex.insert(std::make_pair(dependencyName,
                                                       Vertex()));
                if (inserted)
                {
                    dependencyVertex = add_vertex(graph);
                    vertexToName[dependencyVertex] = dependencyName;
                    vertexMapPos->second = dependencyVertex;
                }
                else
                {
                    dependencyVertex = vertexMapPos->second;
                }
                
                // Create the edge
                boost::add_edge(dependencyVertex, subsystemVertex, graph);
            }
            
            // Record which subsystems are needed
            m_subsystemDeps[subsystemName] = depNames;
        }
  }
  
    // All those subsystems not in the graph are created first
    std::pair<std::string, bool> pair;
    BOOST_FOREACH(pair, inGraph)
    {
        if (!pair.second)
            m_order.push_back(pair.first);
    }

    // Use a topological sort to determine the ordering
    typedef std::list<Vertex> CreationOrder;
    CreationOrder creationOrder;
    boost::topological_sort(graph, std::front_inserter(creationOrder));

    BOOST_FOREACH(Vertex vertex, creationOrder)
        m_order.push_back(vertexToName[vertex]);
}
    
} // namespace core
} // namespace ram
