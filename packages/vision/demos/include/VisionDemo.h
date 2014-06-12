#ifndef RAM_VISION_DEMO_H_09_06_2007
#define RAM_VISION_DEMO_H_09_06_2007

#include "vision/include/Export.h"

#define CAMERA 127
#define MOVIE 159

namespace ram{
namespace vision{
void dataCopy(unsigned char *myData, int w, int h);

class VisionDemo
{
	public:
		void startup(int CAMERAorMOVIE);
		void setOperation(int operation);
	private:
		ram::vision::DetectorTest* forward;	

};	

}
}	
#endif
