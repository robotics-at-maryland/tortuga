
#ifndef RAM_VISION_SLICE_VISIONSYSTEM_H
#define RAM_VISION_SLICE_VISIONSYSTEM_H

#include <Ice/Ice.h>
#include "vision/slice/vision.h"

namespace ram {
namespace vision {
class VisionSystem;
} // namespace vision
} // namespace ram

namespace proxy {
namespace vision {

class VisionSystemProxy : public VisionSystem
{
public:
    VisionSystemProxy(ram::vision::VisionSystem *impl);

    virtual ~VisionSystemProxy();

    virtual void redLightDetectorOn(const Ice::Current &);
    virtual void redLightDetectorOff(const Ice::Current &);
    virtual void buoyDetectorOn(const Ice::Current &);
    virtual void buoyDetectorOff(const Ice::Current &);
    virtual void binDetectorOn(const Ice::Current &);
    virtual void binDetectorOff(const Ice::Current &);
    virtual void pipeLineDetectorOn(const Ice::Current &);
    virtual void pipeLineDetectorOff(const Ice::Current &);
    virtual void ductDetectorOn(const Ice::Current &);
    virtual void ductDetectorOff(const Ice::Current &);
    virtual void downwardSafeDetectorOn(const Ice::Current &);
    virtual void downwardSafeDetectorOff(const Ice::Current &);
    virtual void gateDetectorOn(const Ice::Current &);
    virtual void gateDetectorOff(const Ice::Current &);
    virtual void targetDetectorOn(const Ice::Current &);
    virtual void targetDetectorOff(const Ice::Current &);
    virtual void windowDetectorOn(const Ice::Current &);
    virtual void windowDetectorOff(const Ice::Current &);
    virtual void barbedWireDetectorOn(const Ice::Current &);
    virtual void barbedWireDetectorOff(const Ice::Current &);
    virtual void hedgeDetectorOn(const Ice::Current &);
    virtual void hedgeDetectorOff(const Ice::Current &);
    virtual void velocityDetectorOn(const Ice::Current &);
    virtual void velocityDetectorOff(const Ice::Current &);

private:
    ram::vision::VisionSystem *m_impl;
};

} // namespace vision
} // namespace proxy

#endif // RAM_VISION_SLICE_VISIONSYSTEMPROXY_H
