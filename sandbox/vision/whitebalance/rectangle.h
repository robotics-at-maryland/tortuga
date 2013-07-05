
#ifndef rectangle_h
#define rectangle_h

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <stdio.h>


// STD Includes
#include <set>
#include <vector>
#include <string>
#include <cmath>

// Library Includes
#include "cv.h"
#include "highgui.h"

// Project Includes
#include "vision/include/Common.h"
#include "vision/include/Color.h"
#include "vision/include/Detector.h"
#include "vision/include/BlobDetector.h"
#include "vision/include/TableColorFilter.h"
#include "core/include/ConfigNode.h"
#include "vision/include/WhiteBalance.h"
#include "vision/include/Camera.h"
#include "vision/include/Color.h"
#include "vision/include/Events.h"
#include "vision/include/Image.h"
//#include "vision/include/OpenCVImage.h"
//#include "vision/include/ImageFilter.h"
#include "vision/include/ColorFilter.h"
//#include "vision/include/RegionOfInterest.h"
#include "vision/include/Utility.h"
//#include "vision/include/VisionSystem.h"
//#include "vision/include/TableColorFilter.h"
#include "vision/include/WhiteBalance.h"
#include "vision/include/BuoyDetectorKate.h"


// Must be included last
#include "vision/include/Export.h"

using namespace std;
using namespace cv;

namespace ram {
namespace vision {

int rectangle(char *argv);


class foundLines {
public:
	struct left_hedge
	{
		int foundHedge;
		int cornerX;
		int cornerY;
		int foundHeight;
		int foundWidth;
		int estimatedWidth; // based on foundheight and aspect ratio
		int estimatedHeight; //based on found width and aspect ratio
		float foundAspectRatio;
		float foundAspectRatio_difference;
		int horizontalX1;
		int horizontalY1;
		int horizontalX2;
		int horizontalY2;
		int verticalX1;
		int verticalY1;
		int verticalX2;
		int verticalY2;
		float theta;
		float rho;

	}left_corner; 
	struct right_hedge
	{
		int foundHedge;
		int cornerX;
		int cornerY;
		int foundHeight;
		int foundWidth;
		int estimatedWidth; // based on foundheight and aspect ratio
		int estimatedHeight; //based on found width and aspect ratio
		float foundAspectRatio;
		float foundAspectRatio_difference;
		int horizontalX1;
		int horizontalY1;
		int horizontalX2;
		int horizontalY2;
		int verticalX1;
		int verticalY1;
		int verticalX2;
		int verticalY2;
		float theta;
		float rho;
	}right_corner;

	int foundFullHedge;
	
};//end class
 
}//end namespace
}//end namespace


#endif
