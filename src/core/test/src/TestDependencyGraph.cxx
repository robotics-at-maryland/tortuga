/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/src/TestDependencyGraph.cxx
 */

#ifdef RAM_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#endif
 
// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/filesystem.hpp>
#include <boost/assign/list_of.hpp>

// Project Includes
#include "core/include/DependencyGraph.h"

namespace ba = boost::assign;
namespace bf = boost::filesystem;

static bf::path getConfigRoot()
{
    bf::path root(getenv("RAM_SVN_DIR"));
    return root / "packages" / "core" / "test" / "data";
}

TEST(getOrder)
{
    bf::path path(getConfigRoot() / "subsystems.yml");
    ram::core::ConfigNode config(
        ram::core::ConfigNode::fromFile(path.string()));
    ram::core::ConfigNode sysCfg(config["Subsystems"]);
    ram::core::DependencyGraph depGraph(sysCfg);
        
    std::vector<std::string> expectedOrder = ba::list_of("Manager")
        ("Servant2")("Servant1")("SubServant");
    
    std::vector<std::string> startUpOrder = depGraph.getOrder();

    CHECK_ARRAY_EQUAL(expectedOrder, startUpOrder, 4);    
}

TEST(getDependencies)
{
    bf::path path(getConfigRoot() / "subsystems.yml");
    ram::core::ConfigNode config(
        ram::core::ConfigNode::fromFile(path.string()));
    ram::core::ConfigNode sysCfg(config["Subsystems"]);
    ram::core::DependencyGraph depGraph(sysCfg);

    std::vector<std::string> expectedDeps;
    CHECK(expectedDeps == depGraph.getDependencies("Manager"));

    expectedDeps = ba::list_of("Manager");
    CHECK(expectedDeps == depGraph.getDependencies("Servant1"));
    CHECK(expectedDeps == depGraph.getDependencies("Servant2"));

    expectedDeps = ba::list_of("Servant1")("Servant2");
    CHECK(expectedDeps == depGraph.getDependencies("SubServant"));
}

TEST(writeDependencyGraph)
{
    bf::path path(getConfigRoot() / "subsystems.yml");
    ram::core::ConfigNode config(
        ram::core::ConfigNode::fromFile(path.string()));
    ram::core::ConfigNode sysCfg(config["Subsystems"]);
    ram::core::DependencyGraph depGraph(sysCfg);

    std::stringstream ss;
    depGraph.writeDependencyGraph(ss);

    std::string expected =
        "digraph G {\n"
        "0[label=\"Manager\"];\n"
        "1[label=\"Servant2\"];\n"
        "2[label=\"Servant1\"];\n"
        "3[label=\"SubServant\"];\n"
        "0->1 ;\n"
        "0->2 ;\n"
        "2->3 ;\n"
        "1->3 ;\n"
        "}\n";

    CHECK_EQUAL(expected, ss.str());
}
