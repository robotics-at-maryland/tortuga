//Kate McBryan= May 30 2013
//white balance is an auto-white balancing function
//inputs an Mat and outputs a Mat

#ifndef whitebalance_H
#define whitebalance_H
 
// Library Includes
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <cmath>

// Must be included last
#include "vision/include/Export.h"

using namespace std;
using namespace cv;

namespace ram {
namespace vision {

Mat WhiteBalance (Mat Input);

} //end ram
} //end vision

#endif
