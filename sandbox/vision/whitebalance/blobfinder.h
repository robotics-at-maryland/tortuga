
#ifndef blobfinder_h
#define blobfinder_h

#include <cv.h>
#include <highgui.h>
#include <stdio.h>
using namespace std;
using namespace cv;
#include <cmath>


class blobfinder {
private:

		Mat whitebalance(Mat Input);
		vector<KeyPoint> _keypoints_red;
		vector<KeyPoint> _keypoints_green;
		vector<KeyPoint> _keypoints_yellow;

		vector<KeyPoint> _keypoints_red_previous;
		vector<KeyPoint> _keypoints_green_previous;
		vector<KeyPoint> _keypoints_yellow_previous;

		vector<KeyPoint> _keypoints_red_previous2;
		vector<KeyPoint> _keypoints_green_previous2;
		vector<KeyPoint> _keypoints_yellow_previous2;

		
          
public:
	int ImageFilter(char *argv);
	
};//end blobfinder class
 

#endif