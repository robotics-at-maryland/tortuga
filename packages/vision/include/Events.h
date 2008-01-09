//Daniel Hakim

#ifndef RAM_VISION_EVENTS_01_08_2008
#define RAM_VISION_EVENTS_01_08_2008

// Project Includes
#include "core/include/Event.h"

namespace ram {
namespace vision {

class ImageEvent : public core::Event
{
	public:
	ImageEvent(double x, double y){this->x=x; this->y=y;}
    double x;
    double y;
};
    
} // namespace vision
} // namespace ram

#endif // RAM_VISION_EVENTS_01_08_2008
