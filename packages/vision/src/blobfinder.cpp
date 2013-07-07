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


blobfinder::blobfinder()
{
//initialization
/*
	_keypoints_red_previous2  = 0;
	_keypoints_green_previous2  = 0;
	_keypoints_yellow_previous2  = 0;
	_keypoints_red_previous  = 0;
	_keypoints_green_previous  = 0;
	_keypoints_yellow_previous  = 0;
	_keypoints_red = 0;
	_keypoints_green = 0;
	_keypoints_yellow = 0;
*/
}

Mat blobfinder::DetectBuoys(Mat img_whitebalance,ColorFilter *m_redFilter, ColorFilter *m_greenFilter, ColorFilter *m_yellowFilter)
{

/*
Lots of code taken out because I was carrying data from multiple frames
thats getting moved to the estimator
*/

   //convert from Image* to Mat
   // Mat img = input->asIplImage();
  //output = input;
   //White balance image
	
   // Mat img_whitebalance = cv::Mat::zeros(img.rows,img.cols,CV_8UC(3));
   // img_whitebalance = WhiteBalance(img); 

    //imshow("White Balance",img_whitebalance);

	Mat img_hsv;
	Mat img_temp;
	Mat img_red, img_green, img_yellow; //thresholded for these colors

/*
	_keypoints_red_previous2  = _keypoints_red_previous;
	_keypoints_green_previous2  = _keypoints_green_previous;
	_keypoints_yellow_previous2  = _keypoints_yellow_previous;
	_keypoints_red_previous  = _keypoints_red;
	_keypoints_green_previous  = _keypoints_green;
	_keypoints_yellow_previous  = _keypoints_yellow;
*/


//	CvPoint point;
//	CvPoint point2;
//	CvPoint point3;
//	unsigned int i;
//	unsigned int j,k;

	/*
	for (i=0;i<_keypoints_red_previous.size();i++)
	{		
		point.x = _keypoints_red_previous[i].pt.x;
		point.y=  _keypoints_red_previous[i].pt.y;
		printf("\n \n previous (%d, %d)",point.x,point.y);

	}
	for (i=0;i<_keypoints_red_previous2.size();i++)
	{		
		point.x = _keypoints_red_previous2[i].pt.x;
		point.y=  _keypoints_red_previous2[i].pt.y;
		printf(" previous2 (%d, %d)",point.x,point.y);

	}
*/

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
		img_red =RedFilter(hsv_planes,red_minH,red_maxH);
		img_yellow = OtherColorFilter(hsv_planes,yellow_minH,yellow_maxH);
		img_green = OtherColorFilter(hsv_planes,green_minH,green_maxH);
		//imshow("Red Single Plane",img_red);
		//imshow("Yellow Plane",img_yellow);
		//imshow("Green Plane",img_green);
		
		//only do this for visualization reasons
		hsv_planes[2] = img_red;
		hsv_planes[0] = img_yellow;
		hsv_planes[1] = img_green;
		merge(hsv_planes,img_whitebalance);

		//Step 3: Blob detection! On the thresheld (thresholded?) image	

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
		/*
		//printf("\n Red");
		for (i=0;i<_keypoints_red.size();i++)
		{		
			point.x = _keypoints_red[i].pt.x;
			point.y=  _keypoints_red[i].pt.y;
			//printf("\n Size: %f,angle %f, response = %f",_keypoints_red[i].size,_keypoints_red[i].angle, _keypoints_red[i].response);
			circle(img_whitebalance,point,5,Scalar(0,0,150),1,8,0);
			//printf("\n Keypoint = %d, (%d, %d)",i,point.x,point.y);
		}	
		//printf("\n Yellow");

		for (i=0;i<_keypoints_yellow.size();i++)
		{		
			point.x = _keypoints_yellow[i].pt.x;
			point.y=  _keypoints_yellow[i].pt.y;
			//printf("\n Size: %f,angle %f, response = %f",_keypoints_yellow[i].size,_keypoints_yellow[i].angle, _keypoints_yellow[i].response);
			circle(img_whitebalance,point,5,Scalar(0,255,255),1,8,0);
			//printf("\n Keypoint = %d, (%d, %d)",i,point.x,point.y);
		}
		//printf("\n Green");
		for (i=0;i<_keypoints_green.size();i++)
		{		
			point.x = _keypoints_green[i].pt.x;
			point.y=  _keypoints_green[i].pt.y;
			//printf("\n Size: %f,angle %f, response = %f",_keypoints_green[i].size,_keypoints_green[i].angle, _keypoints_green[i].response);
			circle(img_whitebalance,point,5,Scalar(0,150,0),1,8,0);
			//printf("\n Keypoint = %d, (%d, %d)",i,point.x,point.y);
		}


		int point_reference = 80;
		int size_reference = 7;
		int size_reference_small = 5;
		int point_difference;
		int point_difference2;
		int size_difference;

		//want to compare the found blobs to the previously found blobs
		for (i=0;i<_keypoints_yellow.size();i++)
		{		
				point.x = _keypoints_yellow[i].pt.x;
				point.y=  _keypoints_yellow[i].pt.y;
			

			for(j = 0;j<_keypoints_yellow_previous.size();j++)
			{ 
				//compare location and size
				point2.x = _keypoints_yellow_previous[j].pt.x;
				point2.y=  _keypoints_yellow_previous[j].pt.y;
				
				
				if (point.x < point2.x)
					{point_difference = point2.x - point.x;}
				else
					{point_difference = point.x - point2.x;}
				
				//check the y
				
				if (point.y < point2.y)
					{point_difference2 = point2.y - point.y;}
				else
					{point_difference2 = point.y - point2.y;}

				if (_keypoints_yellow[i].size < _keypoints_yellow_previous[j].size)
					size_difference = _keypoints_yellow_previous[j].size-_keypoints_yellow[i].size;
				else
					{size_difference = _keypoints_yellow[i].size-_keypoints_yellow_previous[j].size;}
			
					
				if ((size_difference < size_reference_small) && (point_difference < point_reference) && (point_difference2 <point_reference))
				{
					//printf("\n Size: %f,angle %f, response = %f",_keypoints_yellow[i].size,_keypoints_yellow[i].angle, _keypoints_yellow[i].response);
					circle(img_whitebalance,point,10,Scalar(255,255,0),1,8,0);
					//printf("\n Keypoint = %d, (%d, %d)",i,point.x,point.y);
				}
			}//end for j

			for(k = 0;k<_keypoints_yellow_previous2.size();k++)
			{ 
				//compare location and size
				point3.x = _keypoints_yellow_previous2[k].pt.x;
				point3.y=  _keypoints_yellow_previous2[k].pt.y;
								
					if (point.x < point3.x)
						{point_difference = point3.x - point.x;}
					else
						{point_difference = point.x - point3.x;}
					
					//check the y
					
					if (point.y < point3.y)
						{point_difference2 = point3.y - point.y;}
					else
						{point_difference2 = point.y - point3.y;}
	
					if (_keypoints_yellow[i].size < _keypoints_yellow_previous2[k].size)
						size_difference = _keypoints_yellow_previous2[k].size-_keypoints_yellow[i].size;
					else
						{size_difference = _keypoints_yellow[i].size-_keypoints_yellow_previous2[k].size;}
				
					
				if ((size_difference < size_reference) && (point_difference < point_reference) && (point_difference2 <point_reference))
				{
					printf("\n Size: %f,angle %f, response = %f",_keypoints_yellow[i].size,_keypoints_yellow[i].angle, _keypoints_yellow[i].response);
					circle(img_whitebalance,point,7,Scalar(0,255,255),1,8,0);
					printf("\n Keypoint = %d, (%d, %d)",i,point.x,point.y);
				}
			}//end for k
		}//end for i

//Red Points!
		//want to compare the found blobs to the previously found blobs
		for (i=0;i<_keypoints_red.size();i++)
		{		
				point.x = _keypoints_red[i].pt.x;
				point.y=  _keypoints_red[i].pt.y;
			for(j = 0;j<_keypoints_red_previous.size();j++)
			{ 
				//compare location and size
				point2.x = _keypoints_red_previous[j].pt.x;
				point2.y=  _keypoints_red_previous[j].pt.y;
				
				
				if (point.x < point2.x)
					{point_difference = point2.x - point.x;}
				else
					{point_difference = point.x - point2.x;}
				
				//check the y
				
				if (point.y < point2.y)
					{point_difference2 = point2.y - point.y;}
				else
					{point_difference2 = point.y - point2.y;}

				if (_keypoints_red[i].size < _keypoints_red_previous[j].size)
					size_difference = _keypoints_red_previous[j].size-_keypoints_red[i].size;
				else
					{size_difference = _keypoints_red[i].size-_keypoints_red_previous[j].size;}
			
					
				if ((size_difference <size_reference_small) && (point_difference <point_reference) && (point_difference2 <point_reference))
				{
					printf("\n Size: %f,angle %f, response = %f",_keypoints_red[i].size,_keypoints_red[i].angle, _keypoints_red[i].response);
					circle(img_whitebalance,point,10,Scalar(0,0,255),1,8,0);
					printf("\n Keypoint = %d, (%d, %d)",i,point.x,point.y);
				}
			}//end for j

			for(k = 0;k<_keypoints_red_previous2.size();k++)
			{ 
				//compare location and size
				point3.x = _keypoints_red_previous2[k].pt.x;
				point3.y=  _keypoints_red_previous2[k].pt.y;
								
					if (point.x < point3.x)
						{point_difference = point3.x - point.x;}
					else
						{point_difference = point.x - point3.x;}
					
					//check the y
					
					if (point.y < point3.y)
						{point_difference2 = point3.y - point.y;}
					else
						{point_difference2 = point.y - point3.y;}
	
					if (_keypoints_red[i].size < _keypoints_red_previous2[k].size)
						size_difference = _keypoints_red_previous2[k].size-_keypoints_red[i].size;
					else
						{size_difference = _keypoints_red[i].size-_keypoints_red_previous2[k].size;}
				
					
				if ((size_difference <size_reference) && (point_difference < point_reference) && (point_difference2 <point_reference))
				{
					printf("\n Size: %f,angle %f, response = %f",_keypoints_red[i].size,_keypoints_red[i].angle, _keypoints_red[i].response);
					circle(img_whitebalance,point,7,Scalar(0,180,255),1,8,0);
					printf("\n Keypoint = %d, (%d, %d)",i,point.x,point.y);
				}
			}//end for k
		}//end for i


		//want to compare the found blobs to the previously found blobs
		for (i=0;i<_keypoints_green.size();i++)
		{		
				point.x = _keypoints_green[i].pt.x;
				point.y=  _keypoints_green[i].pt.y;
			for(j = 0;j<_keypoints_green_previous.size();j++)
			{ 
				//compare location and size
				point2.x = _keypoints_green_previous[j].pt.x;
				point2.y=  _keypoints_green_previous[j].pt.y;
				
				
				if (point.x < point2.x)
					{point_difference = point2.x - point.x;}
				else
					{point_difference = point.x - point2.x;}
				
				//check the y
				
				if (point.y < point2.y)
					{point_difference2 = point2.y - point.y;}
				else
					{point_difference2 = point.y - point2.y;}

				if (_keypoints_green[i].size < _keypoints_green_previous[j].size)
					size_difference = _keypoints_green_previous[j].size-_keypoints_green[i].size;
				else
					{size_difference = _keypoints_green[i].size-_keypoints_green_previous[j].size;}
			
					
				if ((size_difference < size_reference_small) && (point_difference < point_reference) && (point_difference2 <point_reference))
				{
					printf("\n Size: %f,angle %f, response = %f",_keypoints_green[i].size,_keypoints_green[i].angle, _keypoints_green[i].response);
					circle(img_whitebalance,point,10,Scalar(0,20,0),1,8,0);
					printf("\n Keypoint = %d, (%d, %d)",i,point.x,point.y);
				}
			}//end for j

			for(k = 0;k<_keypoints_green_previous2.size();k++)
			{ 
				//compare location and size
				point3.x = _keypoints_green_previous2[k].pt.x;
				point3.y=  _keypoints_green_previous2[k].pt.y;
								
					if (point.x < point3.x)
						{point_difference = point3.x - point.x;}
					else
						{point_difference = point.x - point3.x;}
					
					//check the y
					
					if (point.y < point3.y)
						{point_difference2 = point3.y - point.y;}
					else
						{point_difference2 = point.y - point3.y;}
	
					if (_keypoints_green[i].size < _keypoints_green_previous2[k].size)
						size_difference = _keypoints_green_previous2[k].size-_keypoints_green[i].size;
					else
						{size_difference = _keypoints_green[i].size-_keypoints_green_previous2[k].size;}
				
					
				if ((size_difference < size_reference) && (point_difference < point_reference) && (point_difference2 <point_reference))
				{
					printf("\n Size: %f,angle %f, response = %f",_keypoints_green[i].size,_keypoints_green[i].angle, _keypoints_green[i].response);
					circle(img_whitebalance,point,7,Scalar(0,80,0),1,8,0);
					printf("\n Keypoint = %d, (%d, %d)",i,point.x,point.y);
				}
			}//end for k
		}//end for i
		
	for (i=0;i<_keypoints_red.size();i++)
	{		
		point.x = _keypoints_red[i].pt.x;
		point.y=  _keypoints_red[i].pt.y;
		printf("Red current (%d, %d)",point.x,point.y);

	}

*/

		//imshow("Found Blobs",img_whitebalance);
		//imshow("img_red",img_red);
		//cvWaitKey(0);
		

	//convert back to Imatg*
	
	//output->setData(img_whitebalance.data,false);
//output = img_whitebalance;
return (img_whitebalance);

}

/*
foundblob blobfinder::getKeyPoints(KeyPoint blob)
{

	foundblob finalblob;
	finalblob.centerx = blob.pt.x;
	finalblob.centery = blob.pt.y;
	finalblob.range = blob.size;
	return (finalblob);

}
*/
unsigned int blobfinder::getYellowSize()
{
	return (_keypoints_yellow.size());
}
unsigned int blobfinder::getRedSize()
{
	return (_keypoints_red.size());
}
unsigned int blobfinder::getGreenSize()
{
	return (_keypoints_green.size());
}

vector<KeyPoint> blobfinder::getYellow()
{
	return (_keypoints_yellow);
}

vector<KeyPoint> blobfinder::getRed()
{
	return (_keypoints_red);
}
vector<KeyPoint> blobfinder::getGreen()
{
	return (_keypoints_green);
}
Mat blobfinder::RedFilter(vector<Mat> hsv_planes,int red_minH, int red_maxH)
{
		Mat img_red;
		threshold(hsv_planes[0],img_red,red_minH,255,THRESH_TOZERO);
		threshold(img_red,img_red,red_maxH,0,THRESH_TOZERO_INV);
		threshold(img_red,img_red,1,255,THRESH_BINARY_INV);
		threshold(img_red,img_red,1,255,THRESH_BINARY);
		return(img_red);
};
Mat blobfinder::OtherColorFilter(vector<Mat> hsv_planes,int minH, int maxH)
{
		Mat img;
		threshold(hsv_planes[0],img,maxH,0,THRESH_TOZERO_INV);
		threshold(img,img,minH,255,THRESH_TOZERO);
		threshold(img,img,1,255,THRESH_BINARY);
		
		
		return(img);
};
Mat blobfinder::SaturationFilter(vector<Mat> hsv_planes,int minH,int maxH)
{

		Mat img;
		threshold(hsv_planes[1],img,maxH,0,THRESH_TOZERO_INV);
		threshold(img,img,minH,255,THRESH_TOZERO);
		threshold(img,img,1,255,THRESH_BINARY);
		//double alpha = 0.5;
		// double beta = ( 1-alpha );
		// addWeighted( img, alpha, img_saturation, beta, 0.0, img);
		return(img);
};

Mat blobfinder::LuminanceFilter(vector<Mat> hsv_planes,int minH,int maxH)
{

		Mat img;
		threshold(hsv_planes[2],img,maxH,0,THRESH_TOZERO_INV);
		threshold(img,img,minH,255,THRESH_TOZERO);
		threshold(img,img,1,255,THRESH_BINARY);
		//double alpha = 0.5;
		// double beta = ( 1-alpha );
		// addWeighted( img, alpha, img_saturation, beta, 0.0, img);
		return(img);
};

}//end namspace vision
}//end namespace RAM
