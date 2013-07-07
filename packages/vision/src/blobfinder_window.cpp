/*
//#include <cv.h>
//#include <highgui.h>
//#include <stdio.h>
#include "vision/include/WhiteBalance.h"
#include "vision/include/BuoyDetectorKate.h"

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/EventHub.h"
#include "core/include/PropertySet.h"

namespace ram {
namespace vision {

void blobfinder::FindWindow(Image* input, Mat output,ColorFilter *m_redFilter, ColorFilter *m_greenFilter, ColorFilter *m_yellowFilter)
{


   //convert from Image* to Mat
    Mat img = input->asIplImage();
  //output = input;
   //White balance image
	
   Mat img_whitebalance = cv::Mat::zeros(img.rows,img.cols,CV_8UC(3));
    img_whitebalance = WhiteBalance(img); 

    imshow("White Balance",img_whitebalance);

	Mat img_hsv;
	Mat img_temp;
	Mat img_red, img_green, img_yellow; //thresholded for these colors

	CvPoint point;
	unsigned int i;
	

		//Color Thresholding
		//Step 1: Convert to HSV space, HSV space is less susceptible to lighting changes
		//HSV stands for Hue, saturation and value. In theory, we can do 'color thresholding' just based on Hue
		cvtColor(img_whitebalance,img_hsv,CV_BGR2HSV);
		
		//Step 2:Threshold with pre-defined levels
		//Hue range from 0 to 180 in OpenCV
		//int red_minH, green_minH, green_maxH, yellow_minH, yellow_maxH;
		//yellow_minH = 0; //10
		//yellow_maxH =60; //40
		//red_minH =60; //15 works well 
		//green_minH = 120;//30
		//green_maxH = 180; //60
		//hsv - from 120-180 seems to be a good red filter
		//these levels should be defined in an XML file and not just loaded here
		int red_minH= m_redFilter->getChannel3Low();
		int red_maxH= m_redFilter->getChannel3High();

		int yellow_minH= m_yellowFilter->getChannel3Low();
		int yellow_maxH= m_yellowFilter->getChannel3High();

		int green_minH= m_greenFilter->getChannel3Low();
		int green_maxH= m_greenFilter->getChannel3High();
	
		vector<Mat> hsv_planes;
		split(img_hsv,hsv_planes);

		//first take any value higher than max and converts it to 0
		//red is a special case because the hue value for red are 0-10 and 170-1980
		//same filter as the other cases followed by an invert
		threshold(hsv_planes[0],img_red,red_minH,255,THRESH_TOZERO);
		threshold(img_red,img_red,red_maxH,0,THRESH_TOZERO_INV);
		threshold(img_red,img_red,1,255,THRESH_BINARY_INV);

		threshold(hsv_planes[0],img_yellow,yellow_maxH,0,THRESH_TOZERO_INV);
		threshold(img_yellow,img_yellow,yellow_minH,255,THRESH_TOZERO);

		threshold(hsv_planes[0],img_green,green_maxH,0,THRESH_TOZERO_INV);
		threshold(img_green,img_green,green_minH,255,THRESH_TOZERO);

		threshold(img_red,img_red,1,255,THRESH_BINARY);
		threshold(img_green,img_green,1,255,THRESH_BINARY);
		threshold(img_yellow,img_yellow,1,255,THRESH_BINARY);

		//imshow("Red Single Plane",img_red);
		//imshow("Yellow Plane",img_yellow);
		//imshow("Green Plane",img_green);
		
		hsv_planes[2] = img_red;
		hsv_planes[0] = img_yellow;
		hsv_planes[1] = img_green;
		merge(hsv_planes,img_whitebalance);

		//Step 3: Blob detection! On the thresheld (thresholded?) image
		//Attempt 2:
		//creation 
		//SimpleBlobDetector* blob_detector;
		//blob_detector = new SimpleBlobDetector();
		//blob_detector->creat("SimpleBlobDetector");
		//change params, first move it tp public
	

		SimpleBlobDetector::Params params;
		params.minDistBetweenBlobs =0;
		//params.minThreshold =100;
		//params.maxThreshold =256;
		//params.thresholdStep = 200;
		//params.filterByArea=false;
		params.filterByInertia = true;
		params.filterByConvexity = false;
		params.filterByColor=false;
		params.filterByArea = true;
		params.filterByCircularity = true;
		params.minArea =100.0f;
		params.maxArea = 20000.0f;
		params.minCircularity = 0.3;
		params.maxCircularity = 1.5;
		params.filterByColor = false;
		params.minInertiaRatio =.2;
		params.maxInertiaRatio = 1.3;

		//create
		SimpleBlobDetector blob_red(params);
		SimpleBlobDetector blob_yellow(params);
		SimpleBlobDetector blob_green(params);
		blob_red.create("SimpleBlob");
		blob_green.create("SimpleBlob");
		blob_yellow.create("SimpleBlob");
		//detect
		//vector<KeyPoint> keypoints_red;
		//vector<KeyPoint> keypoints_green;
		//vector<KeyPoint> keypoints_yellow;
		blob_red.detect(img_red,_keypoints_red);
		blob_yellow.detect(img_yellow,_keypoints_yellow);
		blob_green.detect(img_green,_keypoints_green);
		//extract x y coordinates of the keypoints
		
		printf("\n Red");
		for (i=0;i<_keypoints_red.size();i++)
		{		
			point.x = _keypoints_red[i].pt.x;
			point.y=  _keypoints_red[i].pt.y;
			printf("\n Size: %f,angle %f, response = %f",_keypoints_red[i].size,_keypoints_red[i].angle, _keypoints_red[i].response);
			circle(img_whitebalance,point,5,Scalar(0,0,150),1,8,0);
			printf("\n Keypoint = %d, (%d, %d)",i,point.x,point.y);
		}	
		printf("\n Yellow");

		for (i=0;i<_keypoints_yellow.size();i++)
		{		
			point.x = _keypoints_yellow[i].pt.x;
			point.y=  _keypoints_yellow[i].pt.y;
			printf("\n Size: %f,angle %f, response = %f",_keypoints_yellow[i].size,_keypoints_yellow[i].angle, _keypoints_yellow[i].response);
			circle(img_whitebalance,point,5,Scalar(0,255,255),1,8,0);
			printf("\n Keypoint = %d, (%d, %d)",i,point.x,point.y);
		}
		printf("\n Green");
		for (i=0;i<_keypoints_green.size();i++)
		{		
			point.x = _keypoints_green[i].pt.x;
			point.y=  _keypoints_green[i].pt.y;
			printf("\n Size: %f,angle %f, response = %f",_keypoints_green[i].size,_keypoints_green[i].angle, _keypoints_green[i].response);
			circle(img_whitebalance,point,5,Scalar(0,150,0),1,8,0);
			printf("\n Keypoint = %d, (%d, %d)",i,point.x,point.y);
		}

		imshow("Found Blobs",img_whitebalance);
		//imshow("img_red",img_red);
		//cvWaitKey(0);
		

	//convert back to Imatg*
	
	//output->setData(img_whitebalance.data,false);
//output = img_whitebalance;

};

}//end namspace vision
}//end namespace RAM
*/
