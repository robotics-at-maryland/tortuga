/* 
* Daniel Hakim
* VisionSystem is a wrapper for the vision system allowing easy Python wrapping
*/

#ifndef RAM_VISION_VISIONSYSTEM_H_01_29_2008
#define RAM_VISION_VISIONSYSTEM_H_01_29_2008

#include "core/include/Subsystem.h"
#include "vision/include/VisionRunner.h"
#include "vision/include/DetectorTest.h"

namespace ram {
namespace vision {

class VisionSystem : public core::Subsystem
{
    private:
        VisionRunner vr;

	protected:
		VisionSystem(std::string name, core::EventHubPtr eventHub = core::EventHubPtr());
        VisionSystem(std::string name, core::SubsystemList list);
    
    public:
        void redLightDetectorOn();
        void redLightDetectorOff();

};



}
}
#endif 
