
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


// Must be included last
#include "vision/include/Export.h"

using namespace std;
using namespace cv;

namespace ram {
namespace vision {


class foundLines {
public:
Mat gateblob(Mat bw, Mat img_whitebalance);
Mat hedgeblob(Mat img_whitebalance);

Mat rectangle(Mat bw,Mat src);
Mat verticalParallelLines(Mat bw, Mat src);

bool m_found;

	struct checklines
	{
		//vertical lines
		CvPoint upper1;
		CvPoint lower1;
		CvPoint upper2;
		CvPoint lower2;
		CvPoint left;
		CvPoint right;
		int diffVertical;
		int diffHorizontal;
		bool foundHorizontal;
		bool foundVertical;
	};

	struct foundCorner
	{
		int cornerX;
		int cornerY;
		int horizontal;
		int vertical;
		int bottom;
		int left;
		int width;
		int height;
		int maxY;
	};

	struct hedge
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

	}left_top_corner, right_top_corner, left_bottom_corner, right_bottom_corner; 
	struct foundRectangle
	{
		int numberofcorners;
		CvPoint corner1;
		CvPoint corner2;
		CvPoint corner3;
		CvPoint corner4;
		CvPoint corneropposite;
		float foundAspectRatio;
		float foundAspectRatio_difference;
		CvPoint center;
		int c1,c2,c3,c4; //just used for keeping track of which corners were used
		int previous;
		int previousdiff;
		int previous2diff;
		int height;
		int width;
	};

	struct parallelLinesPairs
	{
		CvPoint line1_lower;
		CvPoint line1_upper;
		CvPoint line2_lower;
		CvPoint line2_upper;
		int line1_height;
		int line2_height;
		int height; //max height - lowest point ot the heightest point
		int width;
		float foundAspectRatio;
		float foundAspectRatio_diff;
		int foundtwoside;
		int foundleft;
		int foundright;
		int foundhedge;
		int foundHorizontal;
		int horizontalAtTop;
		CvPoint center;
	};

	//int foundFullHedge;
	//int foundFullSquare;
	parallelLinesPairs finalPair;
private:
	foundRectangle final[5];
	foundRectangle previous[5];
	foundRectangle previous2[5];
	
};//end class
 
}//end namespace
}//end namespace


#endif
