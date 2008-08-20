/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/src/DependencyGraph.cpp
 */

// STD Includes
#include <iostream>

// Project Includes
#include "core/include/DependencyGraph.h"

// Library Includes
#include <boost/foreach.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>

namespace ram {
namespace core {

DependencyGraph::DependencyGraph(ram::core::ConfigNode sectionConfig)
{
    NodeNameList subnodes = sectionConfig.subNodes();
    
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
        ConfigNode config(sectionConfig[subsystemName]);
        
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
            
            // The config list of subsystems this depends on
            ram::core::ConfigNode depsCfg(config["depends_on"]);
            std::vector<std::string> depNames;
            size_t length =  depsCfg.size();
            
            for(size_t i = 0; i < length; ++i)
            {
                std::string dependencyName(depsCfg[i].asString());
                
                // I know this is O(n^2), but the n should be quite small
                if (depNames.end() != std::find(depNames.begin(),
                                                depNames.end(), 
                                                dependencyName)) 
                {
                    std::cout << "Duplicate '" << dependencyName 
                              << "' found in dependency list for Subsystem: '" 
                              << subsystemName << "'" << std::endl;
                }
                else
                {                
                    depNames.push_back(dependencyName);
                }
                
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
            m_sectionDeps[subsystemName] = depNames;
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

void DependencyGraph::writeDependencyGraph(std::ostream& file)
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
    for (size_t i = m_order.size() - m_sectionDeps.size();
         i < m_order.size(); ++i)
    {
        std::map<std::string, std::vector<std::string> >::iterator iter =
            m_sectionDeps.find(m_order[i]);
    
        int vertex = nameToVertex[iter->first];
        
        BOOST_FOREACH(std::string name, iter->second)
        {
            file << nameToVertex[name] << "->" << vertex << " ;" << std::endl;
        }
    }

    // Graphiz footer
    file << "}" << std::endl;
}
    
std::vector<std::string> DependencyGraph::getOrder()
{
    return m_order;
}


std::vector<std::string> DependencyGraph::getDependencies(std::string section)
{
    return m_sectionDeps[section];
}


} // namespace core
} // namespace ram

