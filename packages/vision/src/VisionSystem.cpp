#include "vision/include/VisionSystem.h"

using namespace ram::vision;
VisionSystem::VisionSystem(std::string name, core::EventHubPtr eventHub)
:Subsystem(name, eventHub),vr()
{

}

VisionSystem::VisionSystem(std::string name, core::SubsystemList list)
:Subsystem(name, list),vr()
{

}

void VisionSystem::redLightDetectorOn()
{
    vr.forward.lightDetectOn();
}

void VisionSystem::redLightDetectorOff()
{
    vr.forward.lightDetectOff();
}