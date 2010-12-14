
#include "vision/slice/VisionSystemProxy.h"
#include "vision/include/VisionSystem.h"

namespace proxy {
namespace vision {

VisionSystemProxy::VisionSystemProxy(ram::vision::VisionSystem *impl)
    : m_impl(impl)
{
}

VisionSystemProxy::~VisionSystemProxy()
{
}

void VisionSystemProxy::redLightDetectorOn(const Ice::Current &)
{
    m_impl->redLightDetectorOn();
}

void VisionSystemProxy::redLightDetectorOff(const Ice::Current &)
{
    m_impl->redLightDetectorOff();
}

void VisionSystemProxy::buoyDetectorOn(const Ice::Current &)
{
    m_impl->buoyDetectorOn();
}

void VisionSystemProxy::buoyDetectorOff(const Ice::Current &)
{
    m_impl->buoyDetectorOff();
}

void VisionSystemProxy::binDetectorOn(const Ice::Current &)
{
    m_impl->binDetectorOn();
}

void VisionSystemProxy::binDetectorOff(const Ice::Current &)
{
    m_impl->binDetectorOff();
}

void VisionSystemProxy::pipeLineDetectorOn(const Ice::Current &)
{
    m_impl->pipeLineDetectorOn();
}

void VisionSystemProxy::pipeLineDetectorOff(const Ice::Current &)
{
    m_impl->pipeLineDetectorOff();
}

void VisionSystemProxy::ductDetectorOn(const Ice::Current &)
{
    m_impl->ductDetectorOn();
}

void VisionSystemProxy::ductDetectorOff(const Ice::Current &)
{
    m_impl->ductDetectorOff();
}

void VisionSystemProxy::downwardSafeDetectorOn(const Ice::Current &)
{
    m_impl->downwardSafeDetectorOn();
}

void VisionSystemProxy::downwardSafeDetectorOff(const Ice::Current &)
{
    m_impl->downwardSafeDetectorOff();
}

void VisionSystemProxy::gateDetectorOn(const Ice::Current &)
{
    m_impl->gateDetectorOn();
}

void VisionSystemProxy::gateDetectorOff(const Ice::Current &)
{
    m_impl->gateDetectorOff();
}

void VisionSystemProxy::targetDetectorOn(const Ice::Current &)
{
    m_impl->targetDetectorOn();
}

void VisionSystemProxy::targetDetectorOff(const Ice::Current &)
{
    m_impl->targetDetectorOff();
}

void VisionSystemProxy::windowDetectorOn(const Ice::Current &)
{
    m_impl->windowDetectorOn();
}

void VisionSystemProxy::windowDetectorOff(const Ice::Current &)
{
    m_impl->windowDetectorOff();
}

void VisionSystemProxy::barbedWireDetectorOn(const Ice::Current &)
{
    m_impl->barbedWireDetectorOn();
}

void VisionSystemProxy::barbedWireDetectorOff(const Ice::Current &)
{
    m_impl->barbedWireDetectorOff();
}

void VisionSystemProxy::hedgeDetectorOn(const Ice::Current &)
{
    m_impl->hedgeDetectorOn();
}

void VisionSystemProxy::hedgeDetectorOff(const Ice::Current &)
{
    m_impl->hedgeDetectorOff();
}

void VisionSystemProxy::velocityDetectorOn(const Ice::Current &)
{
    m_impl->velocityDetectorOn();
}

void VisionSystemProxy::velocityDetectorOff(const Ice::Current &)
{
    m_impl->velocityDetectorOff();
}

} // namespace vision
} // namespace proxy
