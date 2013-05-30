
#ifndef rectangle_h
#define rectangle_h

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

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
 



#endif
