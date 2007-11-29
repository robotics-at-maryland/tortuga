/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/src/TestApplication.cxx
 */

// STD Includes
#include <sstream>

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/assign/list_of.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>

// Project Includes
#include "core/test/include/MockSubsystem.h"
#include "core/include/Application.h"

namespace ba = boost::assign;
namespace bf = boost::filesystem;


bf::path getConfigRoot()
{
    bf::path root(getenv("RAM_SVN_DIR"));
    return root / "packages" / "core" / "test" / "data";
}

TEST(getSubsystem)
{
    bf::path path(getConfigRoot() / "simpleSubsystem.yml");
    ram::core::Application app(path.string());

    MockSubsystem* subsystem =
        dynamic_cast<MockSubsystem*>(app.getSubsystem("SystemA").get());
    CHECK(subsystem);
    CHECK_EQUAL("SystemA", subsystem->getName());
    CHECK_EQUAL(0u, subsystem->dependents.size());
    CHECK_EQUAL("John", subsystem->config["test"].asString());

    subsystem =
        dynamic_cast<MockSubsystem*>(app.getSubsystem("BSystem").get());
    CHECK(subsystem);
    CHECK_EQUAL("BSystem", subsystem->getName());
    CHECK_EQUAL(0u, subsystem->dependents.size());
    CHECK_EQUAL("Bob", subsystem->config["test"].asString());
}

TEST(getSubsystemNames)
{
    bf::path path(getConfigRoot() / "simpleSubsystem.yml");
    ram::core::Application app(path.string());

    std::set<std::string> expected = ba::list_of("SystemA")("BSystem");
    std::set<std::string> given;
    BOOST_FOREACH(std::string name, app.getSubsystemNames())
    {
        given.insert(name);
    }
    CHECK(expected == given);
}
    
TEST(Dependencies)
{
    bf::path path(getConfigRoot() / "subsystems.yml");
    ram::core::Application app(path.string());
    
    MockSubsystem* manager =
        dynamic_cast<MockSubsystem*>(app.getSubsystem("Manager").get());

    CHECK(manager);
    CHECK_EQUAL(10, manager->config["test"].asInt());
    CHECK_EQUAL(0u, manager->dependents.size());

    MockSubsystem* servant1 =
        dynamic_cast<MockSubsystem*>(app.getSubsystem("Servant1").get());
    ram::core::SubsystemList expected =
        ba::list_of(app.getSubsystem("Manager"));
    
    CHECK(servant1);
    CHECK_EQUAL(5, servant1->config["test"].asInt());
    CHECK(expected == servant1->dependents);

    MockSubsystem* servant2 =
        dynamic_cast<MockSubsystem*>(app.getSubsystem("Servant2").get());
    
    CHECK(servant2);
    CHECK_EQUAL(3, servant2->config["test"].asInt());
    CHECK(expected == servant2->dependents);


    MockSubsystem* subServant =
        dynamic_cast<MockSubsystem*>(app.getSubsystem("SubServant").get());
    expected =
        ba::list_of(app.getSubsystem("Servant1"))(app.getSubsystem("Servant2"));
    
    CHECK(subServant);
    CHECK_EQUAL(11, subServant->config["test"].asInt());
    CHECK(expected == subServant->dependents);
}

TEST(writeDependencyGraph)
{
    bf::path path(getConfigRoot() / "subsystems.yml");
    ram::core::Application app(path.string());

    std::stringstream ss;
    app.writeDependencyGraph(ss);

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
