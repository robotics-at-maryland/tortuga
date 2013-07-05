
#ifndef blobfinder_h
#define blobfinder_h


// STD Includes
#include <set>
#include <vector>
#include <string>
#include <cmath>

// Library Includes
#include <cv.h>
#include <highgui.h>

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


class blobfinder {
private:
		

		vector<KeyPoint> _keypoints_red_previous;
		vector<KeyPoint> _keypoints_green_previous;
		vector<KeyPoint> _keypoints_yellow_previous;

		vector<KeyPoint> _keypoints_red_previous2;
		vector<KeyPoint> _keypoints_green_previous2;
		vector<KeyPoint> _keypoints_yellow_previous2;

		vector<KeyPoint> _keypoints_red;
		vector<KeyPoint> _keypoints_green;
		vector<KeyPoint> _keypoints_yellow;     
public:
		struct foundblob
		{
			double centerx;
			double centery;
			double range;
			int minX, minY, maxX,maxY;
			int color;
			int position;
			int frames;
		};
	foundblob yellow;

        blobfinder();
	Mat DetectBuoys(Mat img_whitebalance,ColorFilter *m_redFilter, ColorFilter *m_greenFilter, ColorFilter *m_yellowFilter);
	void FindWindow(Image* input, Mat output,ColorFilter *m_redFilter, ColorFilter *m_greenFilter, ColorFilter *m_yellowFilter);
	foundblob getKeyPoints(KeyPoint blob);
	unsigned int getYellowSize();
	unsigned int getRedSize();
	unsigned int getGreenSize();
	vector<KeyPoint> getYellow();
	vector<KeyPoint> getRed();
	vector<KeyPoint> getGreen();
	Mat RedFilter(vector<Mat> hsv_planes,int red_minH, int red_maxH);
	Mat OtherColorFilter(vector<Mat> hsv_planes,int minH, int maxH);
	Mat SaturationFilter(vector<Mat> hsv_planes,double minH, double maxH);





	
};//end blobfinder class
 }//end vision namespace
} //end ram namespace

#endif
