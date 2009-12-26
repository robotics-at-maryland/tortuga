/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/src/TestApplication.cxx
 */

#ifdef RAM_WINDOWS
#define _CRT_SECURE_NO_WARNINGS
#endif
 
// STD Includes
#include <sstream>
#include <cstdlib>

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/assign/list_of.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

// Project Includes
#include "core/test/include/MockSubsystem.h"
#include "core/test/include/LoopSubsystem.h"
#include "core/include/Application.h"
#include "core/include/SubsystemMaker.h"

namespace ba = boost::assign;
namespace bf = boost::filesystem;

const std::string LoopSubsystem::STOP("CORE_TEST_LOOPSUBSYSTEM");
RAM_CORE_REGISTER_SUBSYSTEM_MAKER(LoopSubsystem, LoopSubsystem);

static bf::path getConfigRoot()
{
    bf::path root(getenv("RAM_SVN_DIR"));
    return root / "packages" / "core" / "test" / "data";
}

SUITE(Application)
{

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

TEST(BadDependencies)
{
    std::cout << "Testing Bad Dependencies" << std::endl;
    bf::path path(getConfigRoot() / "bad_subsystems.yml");
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

    // Check to make sure the sub servant is not there
    std::vector<std::string> subsystemList = app.getSubsystemNames();
    BOOST_FOREACH(std::string name, subsystemList)
    {
	CHECK(name != "SubServant");
    }
}

void stopLoop(ram::core::Application* app, ram::core::EventPtr)
{
    app->stopMainLoop();
}

TEST(mainLoop)
{
    bf::path path(getConfigRoot() / "loopSubsystems.yml");
    ram::core::Application app(path.string());

    // Bind to the stop event so we don't have an event loop
    app.getSubsystem("SubsystemA")->subscribe(LoopSubsystem::STOP,
                                              boost::bind(stopLoop, &app, _1));

    // Run the main loop
    app.mainLoop();

    // Check results
    BOOST_FOREACH(std::string name, app.getSubsystemNames())
    {
        ram::core::SubsystemPtr subsystem = app.getSubsystem(name);
        LoopSubsystem* loopSubsystem =
            dynamic_cast<LoopSubsystem*>(subsystem.get());

        CHECK_EQUAL(loopSubsystem->actualUpdates.size(),
                    loopSubsystem->expectedUpdates.size());

        for (size_t i = 0; i < loopSubsystem->actualUpdates.size(); ++i)
        {
            CHECK_CLOSE(loopSubsystem->actualUpdates[i],
                        loopSubsystem->expectedUpdates[i], 0.05);
        }
    }
}

TEST(setPriority)
{
    bf::path path(getConfigRoot() / "prioritySubsystems.yml");
    ram::core::Application app(path.string());

    MockSubsystem* subsystem =
        dynamic_cast<MockSubsystem*>(app.getSubsystem("SystemA").get());
    CHECK(subsystem);
    CHECK_EQUAL(ram::core::IUpdatable::LOW_PRIORITY, subsystem->getPriority());

    subsystem =
        dynamic_cast<MockSubsystem*>(app.getSubsystem("BSystem").get());
    CHECK_EQUAL(ram::core::IUpdatable::NORMAL_PRIORITY,
                subsystem->getPriority());
}

TEST(setAffinity)
{
    bf::path path(getConfigRoot() / "affinitySubsystems.yml");
    ram::core::Application app(path.string());

    MockSubsystem* subsystem =
        dynamic_cast<MockSubsystem*>(app.getSubsystem("SystemA").get());
    CHECK(subsystem);
    CHECK_EQUAL(-1, subsystem->getAffinity());

    subsystem =
        dynamic_cast<MockSubsystem*>(app.getSubsystem("BSystem").get());
    CHECK_EQUAL(1, subsystem->getAffinity());
}

} // SUITE(Application)
