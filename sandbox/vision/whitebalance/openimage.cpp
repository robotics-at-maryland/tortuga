//#include <cv.h>
//#include <highgui.h>
//#include <stdio.h>
//#include "whitebalance.h"
#include "blobfinder.h"

int blobfinder::ImageFilter(char *argv)
{
	Mat img;
	Mat img_whitebalance;
	Mat img_hsv;
	Mat img_temp;
	Mat img_red, img_green, img_yellow; //thresholded for these colors
	printf("Hello \n");
	img = imread(argv, CV_LOAD_IMAGE_COLOR);   // Read the file

	if (img.data) 
	{
		
		_keypoints_red_previous  = _keypoints_red;
		_keypoints_green_previous  = _keypoints_green;
		_keypoints_yellow_previous  = _keypoints_yellow;

		_keypoints_red_previous2  = _keypoints_red_previous;
		_keypoints_green_previous2  = _keypoints_green_previous;
		_keypoints_yellow_previous2  = _keypoints_yellow_previous;



		imshow("Display",img);
		
		//Performs white balance on the image
		//Only input required is the original image
		//output is another image
		img_whitebalance = whitebalance(img);
		//imshow("White Balance",img_whitebalance);			
		

		//Color Thresholding
		//Step 1: Convert to HSV space, HSV space is less susceptible to lighting changes
		//HSV stands for Hue, saturation and value. In theory, we can do 'color thresholding' just based on Hue
		cvtColor(img_whitebalance,img_hsv,CV_BGR2HSV);
		
		//Step 2:Threshold with pre-defined levels
		//Hue range from 0 to 180 in OpenCV
		int red_minH, red_maxH, green_minH, green_maxH, yellow_minH, yellow_maxH;
		yellow_minH = 15; //10
		yellow_maxH =40; //40
		red_minH =15; //15 works well 
		green_minH = 30;//30
		green_maxH = 60; //60
		//these levels should be defined in an XML file and not just loaded here

		//attempt -output image is CV_8U
		//inRange(img_hsv,Scalar(red_minH,0,0),Scalar(red_maxH,256,256),img_red);
		//imshow("Red InRange",img_red);
		
		vector<Mat> hsv_planes;
		split(img_hsv,hsv_planes);

		//first take any value higher than max and converts it to 0
		//red is a special case because the hue value for red are 0-10 and 170-1980
		threshold(hsv_planes[0],img_red,red_minH,180,THRESH_BINARY_INV);
		//second take any value lower than min and converts to 0
		//threshold(img_red,img_red,red_minH,10,THRESH_TOZERO);

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
		
		//merge(hsv_planes,img_red);
		//cvtColor(img_red,img_red,CV_HSV2RGB);
		//imshow("Red-planes",img_red);
		//	Mat img_gray;
		//cvtColor(img_red,img_gray,CV_RGB2GRAY);
		//imshow("gray!",img_gray);
		
		//threshold(img_gray,img_gray,10,255,THRESH_BINARY);
		//imshow("gray2",img_gray);
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
		CvPoint point;
		CvPoint point2;
		CvPoint point3;
		printf("\n Red");
		for (int i=0;i<_keypoints_red.size();i++)
		{		
			point.x = _keypoints_red[i].pt.x;
			point.y=  _keypoints_red[i].pt.y;
			printf("\n Size: %f,angle %f, response = %f",_keypoints_red[i].size,_keypoints_red[i].angle, _keypoints_red[i].response);
			circle(img_whitebalance,point,5,Scalar(0,0,255),1,8,0);
			printf("\n Keypoint = %d, (%d, %d)",i,point.x,point.y);
		}	
		printf("\n Yellow");

		for (int i=0;i<_keypoints_yellow.size();i++)
		{		
			point.x = _keypoints_yellow[i].pt.x;
			point.y=  _keypoints_yellow[i].pt.y;
			printf("\n Size: %f,angle %f, response = %f",_keypoints_yellow[i].size,_keypoints_yellow[i].angle, _keypoints_yellow[i].response);
			circle(img_whitebalance,point,5,Scalar(255,255,0),1,8,0);
			printf("\n Keypoint = %d, (%d, %d)",i,point.x,point.y);
		}
		printf("\n Green");
		for (int i=0;i<_keypoints_green.size();i++)
		{		
			point.x = _keypoints_green[i].pt.x;
			point.y=  _keypoints_green[i].pt.y;
			printf("\n Size: %f,angle %f, response = %f",_keypoints_green[i].size,_keypoints_green[i].angle, _keypoints_green[i].response);
			circle(img_whitebalance,point,5,Scalar(0,255,0),1,8,0);
			printf("\n Keypoint = %d, (%d, %d)",i,point.x,point.y);
		}




		int point_difference;
		int point_difference2;
		int point_difference3;
		int point_difference4;
		int size_difference;
		int size_difference2;






		//want to compare the found blobs to the previously found blobs
		for (int i=0;i<_keypoints_yellow.size();i++)
		{		
				point.x = _keypoints_yellow[i].pt.x;
				point.y=  _keypoints_yellow[i].pt.y;
			for(int j = 0;j<_keypoints_yellow_previous.size();j++)
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
			
					
				if ((size_difference < 3) && (point_difference < 30) && (point_difference2 <30))
				{
					printf("\n Size: %f,angle %f, response = %f",_keypoints_yellow[i].size,_keypoints_yellow[i].angle, _keypoints_yellow[i].response);
					circle(img_whitebalance,point,10,Scalar(255,255,0),1,8,0);
					printf("\n Keypoint = %d, (%d, %d)",i,point.x,point.y);
				}
			}//end for j

			for(int k = 0;k<_keypoints_yellow_previous.size();k++)
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
				
					
				if ((size_difference < 5) && (point_difference < 30) && (point_difference2 <30))
				{
					printf("\n Size: %f,angle %f, response = %f",_keypoints_yellow[i].size,_keypoints_yellow[i].angle, _keypoints_yellow[i].response);
					circle(img_whitebalance,point,7,Scalar(0,255,255),1,8,0);
					printf("\n Keypoint = %d, (%d, %d)",i,point.x,point.y);
				}
			}//end for k
		}//end for i

//Red Points!
		//want to compare the found blobs to the previously found blobs
		for (int i=0;i<_keypoints_red.size();i++)
		{		
				point.x = _keypoints_red[i].pt.x;
				point.y=  _keypoints_red[i].pt.y;
			for(int j = 0;j<_keypoints_red_previous.size();j++)
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
			
					
				if ((size_difference < 3) && (point_difference < 30) && (point_difference2 <30))
				{
					printf("\n Size: %f,angle %f, response = %f",_keypoints_red[i].size,_keypoints_red[i].angle, _keypoints_red[i].response);
					circle(img_whitebalance,point,10,Scalar(0,0,255),1,8,0);
					printf("\n Keypoint = %d, (%d, %d)",i,point.x,point.y);
				}
			}//end for j

			for(int k = 0;k<_keypoints_red_previous.size();k++)
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
				
					
				if ((size_difference < 5) && (point_difference < 30) && (point_difference2 <30))
				{
					printf("\n Size: %f,angle %f, response = %f",_keypoints_red[i].size,_keypoints_red[i].angle, _keypoints_red[i].response);
					circle(img_whitebalance,point,7,Scalar(0,180,255),1,8,0);
					printf("\n Keypoint = %d, (%d, %d)",i,point.x,point.y);
				}
			}//end for k
		}//end for i









		//want to compare the found blobs to the previously found blobs
		for (int i=0;i<_keypoints_green.size();i++)
		{		
				point.x = _keypoints_green[i].pt.x;
				point.y=  _keypoints_green[i].pt.y;
			for(int j = 0;j<_keypoints_green_previous.size();j++)
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
			
					
				if ((size_difference < 3) && (point_difference < 30) && (point_difference2 <30))
				{
					printf("\n Size: %f,angle %f, response = %f",_keypoints_green[i].size,_keypoints_green[i].angle, _keypoints_green[i].response);
					circle(img_whitebalance,point,10,Scalar(0,255,0),1,8,0);
					printf("\n Keypoint = %d, (%d, %d)",i,point.x,point.y);
				}
			}//end for j

			for(int k = 0;k<_keypoints_green_previous.size();k++)
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
				
					
				if ((size_difference < 5) && (point_difference < 30) && (point_difference2 <30))
				{
					printf("\n Size: %f,angle %f, response = %f",_keypoints_green[i].size,_keypoints_green[i].angle, _keypoints_green[i].response);
					circle(img_whitebalance,point,7,Scalar(255,255,0),1,8,0);
					printf("\n Keypoint = %d, (%d, %d)",i,point.x,point.y);
				}
			}//end for k
		}//end for i
		


		imshow("Found Blobs",img_whitebalance);
		//imshow("img_red",img_red);
		//cvWaitKey(0);
		
	}
	else
	{
		printf("Unable to Load Image \n");
	}
	return 1;
}	
