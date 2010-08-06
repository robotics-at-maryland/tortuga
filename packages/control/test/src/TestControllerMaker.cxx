/*
 * Copyright (C) 2008 Robotics at Maryland
 * Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/control/test/src/TestControllerMaker.cxx
 */

// STD Includes
//#include <utility>

// Library Includes
#include <UnitTest++/UnitTest++.h>

// Test Includes
#include "control/include/ControllerMaker.h"
#include "control/test/include/MockDepthController.h"
#include "control/test/include/MockTranslationalController.h"
#include "control/test/include/MockRotationalController.h"

static ram::control::DepthControllerImpMakerTemplate<MockDepthController>
registerDepthController("MockDepthController");

static ram::control::TranslationalControllerImpMakerTemplate<
    MockTranslationalController>
registerTranslationalController("MockTranslationalController");

static ram::control::RotationalControllerImpMakerTemplate<
    MockRotationalController>
registerRotationalController("MockRotationalController");

using namespace ram::control;

SUITE(ControllerMaker)
{

TEST(Depth)
{
    ram::core::ConfigNode config(ram::core::ConfigNode::fromString(
            "{ 'val' : 5, 'type' : 'MockDepthController' }"));
    IDepthControllerImpPtr controller =
        DepthControllerImpMaker::newObject(config);

    MockDepthController* mockDepthController =
        dynamic_cast<MockDepthController*>(controller.get());
    CHECK(mockDepthController);
}

TEST(Translational)
{
    ram::core::ConfigNode config(ram::core::ConfigNode::fromString(
            "{ 'val' : 5, 'type' : 'MockTranslationalController' }"));
    ITranslationalControllerImpPtr controller =
        TranslationalControllerImpMaker::newObject(config);

    MockTranslationalController* mockTranslationalController =
        dynamic_cast<MockTranslationalController*>(controller.get());
    CHECK(mockTranslationalController);
}

TEST(Rotational)
{
    ram::core::ConfigNode config(ram::core::ConfigNode::fromString(
            "{ 'val' : 5, 'type' : 'MockRotationalController' }"));
    IRotationalControllerImpPtr controller =
        RotationalControllerImpMaker::newObject(config);

    MockRotationalController* mockRotationalController =
        dynamic_cast<MockRotationalController*>(controller.get());
    CHECK(mockRotationalController);
}

} // SUITE(ControllerMaker)
