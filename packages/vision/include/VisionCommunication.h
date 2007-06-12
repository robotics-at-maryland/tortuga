#ifndef RAM_VISION_VISIONCOMMUNICATION_H_06_11_2007
#define RAM_VISION_VISIONCOMMUNICATION_H_06_11_2007

#include "VisionData.h"

namespace ram
{
namespace vision
{
class VisionCommunication
{
 public:
  VisionData unsafe;
  VisionData** safe;
  VisionData guaranteed;
  VisionData dummyCheck;
  
  VisionData* getDummy();
  VisionData* getUnsafe();
  VisionData** getSafe();
  VisionData* getData();
 private:
  VisionData* captureSnapshot();
};

}

}
#endif // RAM_VISION_VISIONCOMMUNICATION_H_06_11_2007
