//Note: 6-28-2013 McBryan
//The targets, specifically on transdec last year, had hard supports which got mistaken as the sides of the targets
//so we had two very long vertical members which would lead to false-positives or just missing the target
//so maybe instead of extending the lines to find the corners, find if/where it would fall on the line



//#include <cv.h>
//#include <highgui.h>
//#include <stdio.h>
#include "vision/include/WhiteBalance.h"
#include "vision/include/GateDetectorKate.h"
#include "vision/include/GateDetector.h"

// Project Includes
#include "core/include/ConfigNode.h"
#include "core/include/EventHub.h"
#include "core/include/PropertySet.h"

using namespace std;
using namespace cv;


namespace ram {
namespace vision {

//dont need to run whitebalance before this since I'm just going to convert to grayscale.
foundLines::parallelLinesPairs foundLines::hedgeblob(Mat img_hsv)
{
	//Mat img_hsv;
	//cvtColor(img_whitebalance,img_hsv,CV_BGR2HSV);
		
	//use blob detection to find gate
	//find left and right red poles - vertical poles
	vector<Mat> hsv_planes;
	split(img_hsv,hsv_planes);

	//first take any value higher than max and converts it to 0
	//red is a special case because the hue value for red are 0-10 and 170-1980
	//same filter as the other cases followed by an invert
	int minH = 20;
	int maxH = 70;
	blobfinder blob;
	Mat img =blob.OtherColorFilter(hsv_planes,minH,maxH);

	//For attempting to use with canny
	int erosion_type = 0;
	int erosion_size = 1;
	//if( erosion_elem == 0 ){ erosion_type = MORPH_RECT; }
	// else if( erosion_elem == 1 ){ erosion_type = MORPH_CROSS; }
	// else if( erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }
	Mat element = getStructuringElement( erosion_type,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );

  	/// Apply the erosion operation
	Mat erosion_dst;
  	erode(img, erosion_dst, element );
  	//imshow( "Erosion Demo", erosion_dst );


	//imshow("green",img);

	parallelLinesPairs parallelLinesresults = verticalParallelLines(erosion_dst,img_hsv);
	//imshow("ertical filter whitebalacnce",img_whitebalance);
	return(parallelLinesresults);
};


foundLines::parallelLinesPairs foundLines::gateblob(Mat bw, Mat img_whitebalance)
{
	//m_found = false;
	//Mat img_hsv;
	//cvtColor(img_whitebalance,img_hsv,CV_BGR2HSV);
		
	//use blob detection to find gate
	//find left and right red poles - vertical poles
	//vector<Mat> hsv_planes;
	//split(img_hsv,hsv_planes);

	//first take any value higher than max and converts it to 0
	//red is a special case because the hue value for red are 0-10 and 170-1980
	//same filter as the other cases followed by an invert
	
/*int red_minH = GateDetector::returnRedmin();
	int red_maxH = GateDetector::m_redmaxH;
	blobfinder blob;
	Mat img_red =blob.RedFilter(hsv_planes,red_minH,red_maxH);

	//For attempting to use with canny
	int erosion_type = 0;
	int erosion_size = 1;
	//if( erosion_elem == 0 ){ erosion_type = MORPH_RECT; }
	// else if( erosion_elem == 1 ){ erosion_type = MORPH_CROSS; }
	// else if( erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }
	Mat element = getStructuringElement( erosion_type,
                                       Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                       Point( erosion_size, erosion_size ) );

  	/// Apply the erosion operation
	Mat erosion_dst;
  	erode(img_red, erosion_dst, element );
  	//imshow( "Erosion Demo", erosion_dst );


	//imshow("red",img_red);
*/
	
	parallelLinesPairs parallelLinesresults = verticalParallelLines(bw,img_whitebalance);
	//imshow("Vertical filter whitebalacnce",img_whitebalance);
	return(parallelLinesresults);
};

foundLines::parallelLinesPairs foundLines::verticalParallelLines(Mat bw, Mat src)
{
	//use hough lines to find two vertical parallel lines
	//check to see if there is a horizontal line inbetween, preferably at either the top or bottom of the bars
	int cannylow = 40;
	int cannyhigh =80;
	float aspectRatio =1.0; //height/width
	float horizontalslope = 0.5; //metric to determine if horizontal
	float verticleslope = 6.1;
	int hough_threshold = 50;
	int hough_minLineLength = 30;
	int hough_maxLinegap = 5;
	float maxAspectRatio_diff = 0.85;
	int maxdiff = 200;
	float foundAspectRatio = 0;
	int height;
	//int cornerdifference = 10; //how far away opposite corners can be before they're considered part of the square 
	int bdifflimit = 15; //allowable difference in bintercept of horizontal lines before they're considered the same line
	int topdifflimit = 40;
	cv::Canny(bw, bw, cannylow, cannyhigh, 3);
	//imshow("Bw",bw);

	//Step 2: Use Hough to find lines
	// Hough Line Probability Method
    	// dst: Output of the edge detector. It should be a grayscale image (although in fact it is a binary one)
   	// lines: A vector that will store the parameters (r,\theta) of the detected lines
   	// rho : The resolution of the parameter r in pixels. We use 1 pixel.
   	// theta: The resolution of the parameter \theta in radians. We use 1 degree (CV_PI/180)
   	// threshold: The minimum number of intersections to “detect” a line
   	// minLinLength: The minimum number of points that can form a line. Lines with less than this number of points are disregarded.
	// maxLineGap: The maximum gap between two points to be considered in the same line.

	vector<Vec4i> linesP;
  	  HoughLinesP(bw, linesP, 1, CV_PI/180, hough_threshold, hough_minLineLength, hough_maxLinegap );


 //Step 3: Clean up lines
	//calculate slope and yintercept  - used to filter out horizontal and vertical lines
	//then they are saved for each line so I dont have to calculate them again
	float m1, m2, b1,b2,bdiff;
	float m[linesP.size()];
	float b[linesP.size()];

	int rightXi,rightYi,leftXi,leftYi,rightXj,rightYj,leftXj,leftYj;
	int topXi, topYi, bottomXi,bottomYi, topXj, topYj, bottomXj, bottomYj, topdiff;
	
	//keep track of which lines are just repeats (or part of a combined line)
	//matches = how many lines were similiar enough to be the same line
	//horizontal or vertical saves 1 if horizontal and a 2 if vertical
	int repeatabilityP[linesP.size()];
	int matchesP[linesP.size()];
	int horizontalOrVertical[linesP.size()];



	 for( size_t i = 0; i < linesP.size(); i++ )
   	 {
		repeatabilityP[i] = 0;
		matchesP[i] = 0;
		horizontalOrVertical[i] = 0;
      		line(src, Point(linesP[i][0], linesP[i][1]),
   	         	 Point(linesP[i][2], linesP[i][3]), Scalar(0,0,255), 1, 8 );
			//printf("\n m = %f", m1);
	 }
	//imshow("all hough", src);



   	 for( size_t i = 0; i < linesP.size(); i++ )
   	 {
	  if (repeatabilityP[i] < 1)
	  {
		//I have two points to define each line segment
		//first filter through and look for horizontal, slope near 0
		if (linesP[i][0] == linesP[i][2])
			m1 = 10000;
		else
			m1 = float(linesP[i][1]-linesP[i][3])/float(linesP[i][0]-linesP[i][2]);

		b1 = linesP[i][1]-m1*linesP[i][0];
		if (m1 < 0)
			m1 = -m1;

		if (m1 < horizontalslope)
		{
			//printf("\n Horizontal");
			horizontalOrVertical[i] = 1;
			
			//now I need to combine with other horizontal line segments
			 for( size_t j = i+1; j< linesP.size(); j++ )
   			 {
				//if its already labeled repeated then dont bother
				if (repeatabilityP[j] < 1)
				{
					 if (linesP[j][0] == linesP[j][2])
						m2 = 10000;
					else
						m2 = float(linesP[j][1]-linesP[j][3])/float(linesP[j][0]-linesP[j][2]);
					
					b2 = linesP[j][1]-m2*linesP[j][0];
					
					//step 1: check slopes
					if (m2 < horizontalslope)
					{ 
						//slopes are both more of less horizontal
						//check vertical distance between the end points

						//first find left most point of each line segment
						//line i:
						if (linesP[i][0] < linesP[i][2])
						{
							leftXi = linesP[i][0];
							leftYi = linesP[i][1];
							rightXi= linesP[i][2];
							rightYi = linesP[i][3];
						}
						else
						{
							leftXi = linesP[i][2];
							leftYi = linesP[i][3];
							rightXi= linesP[i][0];
							rightYi = linesP[i][1];
						}
						//line j:
						if (linesP[j][0] < linesP[j][2])
						{
							leftXj = linesP[j][0];
							leftYj = leftXi*m2 + b2;
							rightXj= linesP[j][2];
							rightYj = rightXi*m2+b2;
						}
						else
						{
							leftXj = linesP[j][2];
							leftYj = leftXi*m2 + b2;
							rightXj= linesP[j][0];
							rightYj = rightXi*m2+b2;
						}
							
						if (b1 < b2)
							 bdiff = b2-b1;
						else
							bdiff = b1-b2;
						//printf("\n bdiff = %f hieght1 = %d, height2 = %d",bdiff, height1,height2);
						if (bdiff < bdifflimit)
						{
							 //same line, so combine them
							repeatabilityP[j] = 2;
							matchesP[i] = matchesP[i]+1;
							//form the longest line
							if (leftXj < leftXi)
							{
								linesP[i][0] = leftXj;
								linesP[i][1] = leftYj;
							}

							if (rightXj > rightXi)
							{
								linesP[i][2] =rightXj;
								linesP[i][3] =rightYj;
							}
						} //end if a match
					  }//end if mj < 2
				} //end for j repeated
			 } //end for j
			//horizontal line
			//printf(" horizontal lines = %d", matchesP[i]);
			//horizontal
			line(src, Point(linesP[i][0], linesP[i][1]),
   	         	 Point(linesP[i][2], linesP[i][3]), Scalar(255,0,255), 2, 8 );
			
			//recalculate slope here for the new combine line, and save
			if (linesP[i][0] == linesP[i][2])
				m1 = 10000;
			else
				m1 = float(linesP[i][1]-linesP[i][3])/float(linesP[i][0]-linesP[i][2]);
	
			b1 = linesP[i][1]-m1*linesP[i][0];
			//if (m1 < 0)
			//	m1 = -m1;
			m[i] = m1;
			b[i] =b1;

		}//end for m1 < 2
		else if (m1 >verticleslope )
		{
			//Vertical Line

			horizontalOrVertical[i] = 2;
		 	if (linesP[i][1] < linesP[i][3])
			{
				bottomXi = linesP[i][0];
				bottomYi = linesP[i][1];
				topXi= linesP[i][2];
				topYi = linesP[i][3];
			}
			else
			{
				bottomXi = linesP[i][2];
				bottomYi = linesP[i][3];
				topXi= linesP[i][0];
				topYi = linesP[i][1];
				linesP[i][0] = bottomXi;
				linesP[i][1] = bottomYi;
				linesP[i][2] = topXi;		
				linesP[i][3] = topYi;
			}
			//printf("\n i: (%d, %d), (%d, %d)", linesP[i][0],linesP[i][1],linesP[i][2],linesP[i][3]);

			//now I need to combine with vertical line segments
			for( size_t j = i+1; j< linesP.size(); j++ )
   			 {
 				if (linesP[j][0] == linesP[j][2])
					m2 = 10000;
				else
					m2 = float(linesP[j][1]-linesP[j][3])/float(linesP[j][0]-linesP[j][2]);				
				b2 = linesP[j][1]-m2*linesP[j][0];

				if (m2 < 0)
					m2 = -m2;

				//if its already labeled repeated then dont bother
				if (m2 > verticleslope  && repeatabilityP[j] < 1)
				{
				  //slopes line up - now extend j line to length Y coordinates of line i
				  //or we can just compare where first X coordinates

					//line j:
					if (linesP[j][1] < linesP[j][3])	
					{
						bottomXj = linesP[j][0];
						bottomYj = linesP[j][1];
						topXj= linesP[j][2];
						topYj = linesP[j][3];
					}
					else
					{
						bottomXj = linesP[j][2];
						bottomYj = linesP[j][3];
						topXj= linesP[j][0];
						topYj = linesP[j][1];
					}

					if (topXi < topXj)
						topdiff = topXj - topXi;
					else
						topdiff = topXi-topXj;

					//printf("\n Vertical:bottom = %d %d, top= %d %d",bottomXi,bottomXj, topXi, topXj);
					if (topdiff < topdifflimit)
					{
						//same line
						repeatabilityP[j] = 1;
						matchesP[i] = matchesP[i] + 1;

						//find the longest line
						if (topYj > topYi)
						{
               						linesP[i][2] = topXj;
               						linesP[i][3] = topYj;
						}
						if (bottomYj < bottomYi)
						{
               						linesP[i][0] = bottomXj;
               						linesP[i][1] = bottomYj;
						}
					}
				}//end if repeated and slope limit
			}//end for j
			//verticel

			line(src, Point(linesP[i][0], linesP[i][1]),
   	          	 Point(linesP[i][2], linesP[i][3]), Scalar(255,255,0), 2, 8 ); 

			//find slope for new combined line and save
			if (linesP[i][0] == linesP[i][2])
				m1 = 10000;
			else
				m1 = float(linesP[i][1]-linesP[i][3])/float(linesP[i][0]-linesP[i][2]);
	
			b1 = linesP[i][1]-m1*linesP[i][0];
			//if (m1 < 0)
			//	m1 = -m1;
			m[i] = m1;
			b[i] =b1;

		}
		else
		{	//not a horizontal or vertical line

      			//line(src, Point(linesP[i][0], linesP[i][1]),
   	         	// Point(linesP[i][2], linesP[i][3]), Scalar(0,0,255), 1, 8 );
			//printf("\n m = %f", m1);
		}
           }//end if repeatability
   	 }

	int totalVertical= 0;
	int totalHorizontal = 0;
	for( size_t i = 0; i < linesP.size(); i++ )
   	{
		if (horizontalOrVertical[i] == 2)
			totalVertical= totalVertical+1;
		if (horizontalOrVertical[i] == 1)
			totalHorizontal= totalHorizontal+1;
	};
	//printf("\n numberof lines found = %d",linesP.size());

//Step 4: look for horizontal and vertical ones and decide which ones go together
	//I have all the lines
	//look for vertical and horizontal lines with about the same endpoints
	//or vertical lines with roughly the same y coordinates
        checklines lineResults[linesP.size()];
	checklines temp;
	int diff;
	int diff2;
	int widthUpper;
	for(size_t i = 0; i < linesP.size(); i++ )	
	{
		lineResults[i].diffVertical = 999;
		lineResults[i].diffHorizontal =909000;
		if (horizontalOrVertical[i] ==2)
		{	//is vertical
			//find lower and upper point
			if (linesP[i][1]< linesP[i][3])
			{
				lineResults[i].upper1.x = linesP[i][2];
				lineResults[i].upper1.y = linesP[i][3];
				lineResults[i].lower1.x = linesP[i][0];
				lineResults[i].lower1.y = linesP[i][1];			
			}
			else
			{
				lineResults[i].upper1.x = linesP[i][0];
				lineResults[i].upper1.y = linesP[i][1];
				lineResults[i].lower1.x = linesP[i][2];
				lineResults[i].lower1.y = linesP[i][3];
			}
			for(size_t j= 0; j < linesP.size(); j++ )
			{
			//find the upper and lower point of the vertical 
			//find the left and right of the horizontal
			//if two vertical check to see if they're roughly the same 
			//if one vertical and one horizontal check to see if its a corner
				if (horizontalOrVertical[j] ==2 && i!= j)
				{
					//is vertical
					//find upper and lower ones
					if (linesP[j][1]< linesP[j][3])
					{
						temp.upper2.x = linesP[j][2];
						temp.upper2.y = linesP[j][3];
						temp.lower2.x = linesP[j][0];
						temp.lower2.y = linesP[j][1];			
					}
					else
					{
						temp.upper2.x = linesP[j][0];
						temp.upper2.y = linesP[j][1];
						temp.lower2.x = linesP[j][2];
						temp.lower2.y = linesP[j][3];
					}
					widthUpper = abs(lineResults[i].upper1.x - temp.upper2.x);
					//printf("width upper = %d",widthUpper);
					foundAspectRatio = float(lineResults[i].upper1.y-lineResults[i].lower1.y)/float(widthUpper);
					diff = abs(lineResults[i].lower1.y-temp.lower2.y)+abs(lineResults[i].upper1.y-temp.upper2.y);
					diff2 = lineResults[i].diffVertical*1.5;
					if ((widthUpper > 10) && (abs(foundAspectRatio-aspectRatio) <maxAspectRatio_diff)  && (( (diff < lineResults[i].diffVertical) )))
					{	
					  lineResults[i].upper2 = temp.upper2;
					  lineResults[i].lower2 = temp.lower2;
					  lineResults[i].foundVertical = true;
					  lineResults[i].diffVertical = diff;
					  lineResults[i].aspectratio_diff = abs(foundAspectRatio-aspectRatio);
					}

				 // printf("\n aspectRatio = %f, Vert diff = %d", foundAspectRatio, diff);
					

				}
				else if (horizontalOrVertical[j] == 1 && i!=j)
				{
					//is horizontal
					//find upper and lower ones
					if (linesP[j][0]< linesP[j][2])
					{
						temp.right.x = linesP[j][2];
						temp.right.y = linesP[j][3];
						temp.left.x = linesP[j][0];
						temp.left.y = linesP[j][1];			
					}
					else
					{
						temp.right.x = linesP[j][0];
						temp.right.y = linesP[j][1];
						temp.left.x = linesP[j][2];
						temp.left.y = linesP[j][3];;
					}
					//unlike Vertical, only want to check one side, the closer side
					
					//can also use the difference between determined width and actual width...
					diff2 = lineResults[i].diffHorizontal*1.5;
					widthUpper = temp.right.x-temp.left.x;
					height = abs(lineResults[i].upper1.y-lineResults[i].lower1.y);
					if (temp.right.y < lineResults[i].lower1.y)
					{
					 // below the previous line
					 	height = lineResults[i].upper1.y-temp.right.y;
						foundAspectRatio = float(height)/float(widthUpper);
						diff = abs(lineResults[i].lower1.x-temp.left.x);
							if (abs(lineResults[i].lower1.x-temp.right.x)< diff)
								diff = abs(lineResults[i].lower1.x-temp.right.x);
					}
					else if (temp.right.y > lineResults[i].upper1.y)
					{

						height = temp.right.y-lineResults[i].lower1.y;
						foundAspectRatio = float(height)/float(widthUpper);
						diff = abs(lineResults[i].upper1.x-temp.left.x);
							if (abs(lineResults[i].upper1.x-temp.right.x)< diff)
								diff = abs(lineResults[i].upper1.x-temp.right.x);
					}
					else
					{
						//line is in between so not a hedge
						foundAspectRatio = 9000;
						diff = 90000;
					}
					//  printf("\n aspectRatio = %f, horizontal diff = %d", foundAspectRatio, diff);
					//printf("\n widthupper, height = %d %d",widthUpper,height);
				
					//also want to make sure that the horizontal line isn't in the middle of the vertical
					
					if ( (foundAspectRatio < 4) && (diff < lineResults[i].diffHorizontal))
					{	
					  //printf(" aspect ratio difference = %f",abs(foundAspectRatio-aspectRatio));
					  lineResults[i].left = temp.left; 
					  lineResults[i].right = temp.right;
					  lineResults[i].foundHorizontal = true;
					  lineResults[i].diffHorizontal = diff;
					  lineResults[i].aspectratio_diff = abs(foundAspectRatio-aspectRatio);
					}
				}//end horizontalOrVertical
			} //end for j
		} //end if vertical
	}//end for i

parallelLinesPairs final;
final.foundAspectRatio_diff = 900;


	for(size_t i = 0; i < linesP.size(); i++ )	
	{
		//printf("\n i = %d, diff = %d, horizontal diff = %d aspectratio difference = %f", i, lineResults[i].diffVertical,lineResults[i].diffHorizontal,lineResults[i].aspectratio_diff);
		if (lineResults[i].foundVertical == true && lineResults[i].diffVertical <maxdiff)
		{
			line(src, lineResults[i].upper1,lineResults[i].lower1, Scalar(0,0,255), 2, 8 ); 
			line(src, lineResults[i].upper2,lineResults[i].lower2, Scalar(0,255,255), 2, 8 ); 
		}
		else
			lineResults[i].foundVertical = false;
			
		if (lineResults[i].foundHorizontal == true && lineResults[i].diffHorizontal < (maxdiff))
		{	line(src, lineResults[i].left,lineResults[i].right, Scalar(255,0,0), 2, 8 ); 
			line(src, lineResults[i].upper1,lineResults[i].lower1, Scalar(255,0,255), 2, 8 ); 
		}
		else
			lineResults[i].foundHorizontal = false;

		if ((lineResults[i].foundVertical == true || lineResults[i].foundHorizontal == true) && (final.foundAspectRatio_diff > lineResults[i].aspectratio_diff))
		{
			 final.foundAspectRatio_diff = lineResults[i].aspectratio_diff;
			 final.line1_lower = lineResults[i].lower1;
			 final.line1_upper = lineResults[i].upper1;	
			 final.foundtwosides = 0;
			 final.foundHorizontal = 0;
			 if (lineResults[i].foundVertical == true && lineResults[i].foundHorizontal == true)
			{
				//having found both, need to determine if the horizontal is actually between the vertical and if not, which one to trust
				//already know its near line 1, but not which side
			
				       
					if (lineResults[i].right.y < lineResults[i].lower1.y)
					{
					 // below the previous line
					 	height = lineResults[i].upper1.y-temp.right.y;
						foundAspectRatio = float(height)/float(widthUpper);
						diff = abs(lineResults[i].lower1.x-lineResults[i].left.x);
						diff2 = abs(lineResults[i].lower2.x-lineResults[i].right.x);
						if (abs(lineResults[i].lower1.x-lineResults[i].right.x)< diff)
						{
							diff = abs(lineResults[i].lower1.x-lineResults[i].right.x);
							diff2 = abs(lineResults[i].lower2.x-lineResults[i].left.x);
						}
					}
					else if (lineResults[i].right.y > lineResults[i].upper1.y)
					{
						height = temp.right.y-lineResults[i].lower1.y;
						foundAspectRatio = float(height)/float(widthUpper);
						diff = abs(lineResults[i].upper1.x-lineResults[i].left.x);
						diff2 = abs(lineResults[i].upper2.x-lineResults[i].right.x);
						if (abs(lineResults[i].upper1.x-lineResults[i].right.x)< diff)
						{
							diff = abs(lineResults[i].upper1.x-lineResults[i].right.x);
							diff2 = abs(lineResults[i].upper2.x-lineResults[i].left.x);
						}
					}

				if (diff2 < diff*2)
				{
					//THEY BOTH WORK
			 		final.foundtwosides = 1;
					final.foundHorizontal = 1;
		 			final.line2_lower = lineResults[i].lower2;
					final.line2_upper = lineResults[i].upper2;
	  				final.center.x = (lineResults[i].left.x+lineResults[i].right.x)/2;
					final.center.y = (lineResults[i].upper1.y+lineResults[i].lower1.y+lineResults[i].upper2.y+lineResults[i].lower2.y)/4;
					final.width = lineResults[i].right.x - lineResults[i].left.x;
				}
				else
				{
					//one of these three doesn't below
					//find what gives the lowest aspect ratio difference
					if (foundAspectRatio <= lineResults[i].aspectratio_diff)
					{
					//keep horizontal line
						lineResults[i].foundVertical = false;
						lineResults[i].foundHorizontal= true;
					}
					else	
					{
					//keep Vertical line
						lineResults[i].foundVertical = true;
						lineResults[i].foundHorizontal= false;
					}
				
				}
			}
			if (lineResults[i].foundVertical == true && lineResults[i].foundHorizontal == false)
			{
				final.foundtwosides = 1;
		 		final.line2_lower = lineResults[i].lower2;
				final.line2_upper = lineResults[i].upper2;
				final.center.y = (lineResults[i].upper1.y+lineResults[i].lower1.y+lineResults[i].upper2.y+lineResults[i].lower2.y)/4;
				final.center.x = (lineResults[i].upper1.x+lineResults[i].lower1.x+lineResults[i].upper2.x+lineResults[i].lower2.x)/4;
				final.width = abs((lineResults[i].upper2.x+lineResults[i].lower2.x)/2-(lineResults[i].upper1.x+lineResults[i].lower1.x)/2) ;
			  }
			 if (lineResults[i].foundHorizontal == true && lineResults[i].foundVertical == false)
			 {
				final.foundHorizontal = 1;
			 	final.center.x = (lineResults[i].left.x+lineResults[i].right.x)/2;
				final.center.y = (lineResults[i].upper1.y+lineResults[i].lower1.y)/2;
				final.width = (-lineResults[i].left.x+lineResults[i].right.x);
				final.left = lineResults[i].left;
				final.right = lineResults[i].right;
			 }
		}
	}

	//printf("\n Final aspectratio difference = %f vertical = %d, horizontal = %d, upper1=%d,%d upper2 = %d,%d", final.foundAspectRatio_diff,final.foundtwosides, final.foundHorizontal,final.line1_upper.x,final.line1_upper.y, final.line2_upper.x,final.line2_upper.y);

	if (final.foundtwosides == 1 && final.foundHorizontal == 1)
	{
		line(src, final.line1_upper,final.line1_lower, Scalar(0,255,0), 20, 8 ); 
		line(src, final.left,final.right, Scalar(0,255,0), 20, 8 ); 
		line(src, final.line2_upper,final.line2_lower, Scalar(0,255,0), 20, 8 ); 
	}
	else if (final.foundtwosides == 1)
	{
		line(src, final.line1_upper,final.line1_lower, Scalar(0,255,255), 10, 8 ); 
		line(src, final.line2_upper,final.line2_lower, Scalar(0,255,255), 10, 8 ); 

	}
	else if (final.foundHorizontal == 1)
	{
		line(src, final.line1_upper,final.line1_lower, Scalar(255,0,255), 10, 8 ); 
		line(src, final.left,final.right, Scalar(255,0,255), 10, 8 ); 
	}
	if (final.foundtwosides == 1 || final.foundHorizontal == 1)
		circle(src, final.center,10,Scalar( 0, 255, 0),-1,8 );

	//imshow("hough",src);
	return(final);
//just want to return the one with the closest aspect ratio and most points


	//I should have all the lines now
	//look for pairs- which they'll all be since they're all vertical
	//if there are more than two lines, than find the ones with the best aspect ratio

/*
	parallelLinesPairs pairs;
	if (totalVertical ==  1)
	{
		//only have one line
		if (linesP[0][1] < linesP[0][3])
		{
			pairs.line1_lower.x = linesP[0][0];
			pairs.line1_lower.y = linesP[0][1];
			pairs.line1_upper.x = linesP[0][2];
			pairs.line1_upper.y = linesP[0][3];
		}		
		else
		{
			pairs.line1_lower.x = linesP[0][2];
			pairs.line1_lower.y = linesP[0][3];
			pairs.line1_upper.x = linesP[0][0];
			pairs.line1_upper.y = linesP[0][1];
		}
		pairs.line1_height = pairs.line1_upper.y-pairs.line1_lower.y;
		pairs.width = pairs.line1_height/aspectRatio;
		pairs.foundtwoside = 0;		
		//pairs.center.x = pairs.line1_lower.x +(pairs.width)/2;
		//pairs.center.y = (pairs.line1_upper.y+ pairs.line1_lower.y)/2;
		finalPair = pairs;
		circle(src, finalPair.center,3,Scalar( 0, 255, 0),-1,8 );
		
		line(src, finalPair.line1_lower,
   	          	finalPair.line1_upper, Scalar(0,0,255), 5, 8 ); 
		//also, is there a horizontal line?
		int leftx, rightx;
		if (totalHorizontal > 1)
		{	
			for(size_t i = 0; i < linesP.size(); i++ )
			{
				if (horizontalOrVertical[i] == 1)
				{
					//is the totalHorizontal edge about in line with the vertical 
					leftx = linesP[i][0];
					rightx = linesP[i][2];	
					if (linesP[i][2] < linesP[i][0])
					{
						leftx = linesP[i][2];	
						rightx = linesP[i][0];
					}
					//is it inline
					if ( (abs(leftx-pairs.line1_lower.x) < 30 || abs(rightx-pairs.line1_lower.x)<30) && (rightx-leftx)>30)
					{
						//good line
						pairs.foundHorizontal = 1;
						pairs.horizontalAtTop = 0;
					}
					else if ( (abs(leftx-pairs.line1_upper.x) < 30 || abs(rightx-pairs.line1_upper.x)<30) && (rightx-leftx)>30)
					{
						//good line
						pairs.foundHorizontal = 1;
						pairs.horizontalAtTop = 1;
					}
					//now I can determine if the vertical is left or right
				}
			}
		}//end totalHorizontal
	}
	else if (totalVertical > 1)
	{
		for(size_t i = 0; i < linesP.size(); i++ )
		{
			for( size_t j = 0; j < linesP.size(); j++ )
			{ 
				if (i !=j && (horizontalOrVertical[i] == 2)&& (horizontalOrVertical[j] == 2))
				{
					pairs.foundtwoside = 1;

					//always make the line1 the left line
					if (linesP[i][0] < linesP[j][0])
					{
						if (linesP[i][1] < linesP[i][3])
						{
							pairs.line1_lower.x = linesP[i][0];
							pairs.line1_lower.y = linesP[i][1];
							pairs.line1_upper.x = linesP[i][2];
							pairs.line1_upper.y = linesP[i][3];
						}		
						else
						{
							pairs.line1_lower.x = linesP[i][2];
							pairs.line1_lower.y = linesP[i][3];
							pairs.line1_upper.x = linesP[i][0];
							pairs.line1_upper.y = linesP[i][1];
						}

						if (linesP[j][1] < linesP[j][3])
						{
							pairs.line2_lower.x = linesP[j][0];
							pairs.line2_lower.y = linesP[j][1];
							pairs.line2_upper.x = linesP[j][2];
							pairs.line2_upper.y = linesP[j][3];
						}		
						else
						{
							pairs.line2_lower.x = linesP[j][2];
							pairs.line2_lower.y = linesP[j][3];
							pairs.line2_upper.x = linesP[j][0];
							pairs.line2_upper.y = linesP[j][1];
						}
					}
					else
					{
						if (linesP[i][1] < linesP[i][3])
						{
							pairs.line2_lower.x = linesP[i][0];
							pairs.line2_lower.y = linesP[i][1];
							pairs.line2_upper.x = linesP[i][2];
							pairs.line2_upper.y = linesP[i][3];
						}		
						else
						{
							pairs.line2_lower.x = linesP[i][2];
							pairs.line2_lower.y = linesP[i][3];
							pairs.line2_upper.x = linesP[i][0];
							pairs.line2_upper.y = linesP[i][1];
						}

						if (linesP[j][1] < linesP[j][3])
						{
							pairs.line1_lower.x = linesP[j][0];
							pairs.line1_lower.y = linesP[j][1];
							pairs.line1_upper.x = linesP[j][2];
							pairs.line1_upper.y = linesP[j][3];
						}		
						else
						{
							pairs.line1_lower.x = linesP[j][2];
							pairs.line1_lower.y = linesP[j][3];
							pairs.line1_upper.x = linesP[j][0];
							pairs.line1_upper.y = linesP[j][1];
						}
					}
					pairs.line1_height = pairs.line1_upper.y-pairs.line1_lower.y;
					pairs.line2_height = pairs.line2_upper.y-pairs.line2_lower.y;
					pairs.width = abs(pairs.line1_lower.x-pairs.line2_lower.x);
					int lower, upper;
					if (pairs.line1_lower.y < pairs.line2_lower.y)
						lower = pairs.line1_lower.y;
					else
						lower = pairs.line2_lower.y;

					if (pairs.line1_upper.y < pairs.line2_upper.y)
						upper = pairs.line1_upper.y;
					else
						upper = pairs.line2_upper.y;
				
					pairs.center.x = ((pairs.line2_lower.x+pairs.line1_lower.x)+ (pairs.line2_upper.x+pairs.line1_upper.x))/4;
					pairs.center.y = ((pairs.line2_lower.y+pairs.line1_lower.y)+ (pairs.line2_upper.y+pairs.line1_upper.y))/4;
					pairs.height = upper-lower;
					pairs.foundAspectRatio = (float)pairs.height/(float)pairs.width;
					pairs.foundAspectRatio_diff = abs(pairs.foundAspectRatio-aspectRatio);
					//want to see if t
					if (totalHorizontal > 0)
					{
						for( size_t k = 0; k < linesP.size(); k++ )
					   	{
							if (horizontalOrVertical[k] == 1)
							{
								//check to see if its near the top or the bottom
								//check to see if its between the left and the right side
								if ((abs(linesP[k][1]-pairs.line1_upper.y)< 50 || abs(linesP[k][1]-pairs.line2_upper.y)<50 || abs(linesP[k][1]-pairs.line1_lower.y)< 50 || abs(linesP[k][1]-pairs.line2_lower.y)<50) && ((linesP[k][0]>pairs.line1_lower.x-20) || (linesP[k][2]<pairs.line2_lower.x+20)))
								{

									//horizontal line does work
									pairs.foundHorizontal = 1;
									//need to determine if the line is at the upper edge or the lower edge
									if ((abs(linesP[k][1]-pairs.line1_upper.y)< 50) || (abs(linesP[k][1]-pairs.line2_upper.y)<50) )
										pairs.horizontalAtTop = 1;
									else
										pairs.horizontalAtTop = 0;
								}
							
							}//end if horizontalorVertical == 1
						};
					}//end search for horizontal
					else
					{
						pairs.foundHorizontal = 0;
					};
	
					//want to make sure they aren't the same side so require a minimum distance of 50
					if (pairs.width > 50 && ((pairs.foundAspectRatio_diff < finalPair.foundAspectRatio_diff) || (pairs.foundAspectRatio_diff < finalPair.foundAspectRatio_diff*1.25 && pairs.foundHorizontal > finalPair.foundHorizontal)) )
						finalPair = pairs;
				}//end if the same
			}//end forj
		}//end for i

		m_found = true;	
		//line(src, finalPair.line1_lower,
	   	//          	finalPair.line1_upper, Scalar(0,255,0), 5, 8 ); 
		//line(src, finalPair.line2_lower,
	   	//          	finalPair.line2_upper, Scalar(0,255,255), 5, 8 ); 
		if (finalPair.horizontalAtTop == 1 && finalPair.foundHorizontal == 1)
		{
			line(src, finalPair.line1_upper,
	   	          	finalPair.line2_upper, Scalar(255,255,0), 5, 8 ); 
		}
		if (finalPair.horizontalAtTop == 0 && finalPair.foundHorizontal == 1)
		{
			line(src, finalPair.line1_lower,
	   	          	finalPair.line2_lower, Scalar(255,255,0), 5, 8 ); 
		}
		circle(src, finalPair.center,3,Scalar( 0, 255, 0),-1,8 );
	}//end else
*/

};




















Mat foundLines::rectangle(Mat bw, Mat src)
{
	
	//inputs: 
	int cannylow = 40;
	int cannyhigh =100;
	float aspectRatio =1.0*1000; //height/width
	float horizontalslope = 0.5; //metric to determine if horizontal
	float verticleslope = 6.0;
	int hough_threshold = 30;
	int hough_minLineLength = 20;
	int hough_maxLinegap = 10;
	int cornerdifference = 10; //how far away opposite corners can be before they're considered part of the square 
	int bdifflimit = 15; //allowable difference in bintercept of horizontal lines before they're considered the same line
	int topdifflimit = 40;

	//save previous results
	for (unsigned int i=0;i<5;i++)
	{
		previous2[i] = previous[i];
		previous[i] = final[i];	
	}
	//Step 1: Use canny
	//step a: grayscale
	//cv::Mat bw;
	//cv::cvtColor(src, bw, CV_BGR2GRAY);
	//cv::blur(bw, bw, cv::Size(3, 3));

	cv::Canny(bw, bw, cannylow, cannyhigh, 3);
	//Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );
	//imshow("canny", bw);

	//Step 2: Use Hough to find lines
	// Hough Line Probability Method
    	// dst: Output of the edge detector. It should be a grayscale image (although in fact it is a binary one)
   	// lines: A vector that will store the parameters (r,\theta) of the detected lines
   	// rho : The resolution of the parameter r in pixels. We use 1 pixel.
   	// theta: The resolution of the parameter \theta in radians. We use 1 degree (CV_PI/180)
   	// threshold: The minimum number of intersections to “detect” a line
   	// minLinLength: The minimum number of points that can form a line. Lines with less than this number of points are disregarded.
	// maxLineGap: The maximum gap between two points to be considered in the same line.
	//cv::Mat dst = src.clone();


	vector<Vec4i> linesP;
  	  HoughLinesP(bw, linesP, 1, CV_PI/180, hough_threshold, hough_minLineLength, hough_maxLinegap );

	//calculate slope and yintercept  - used to filter out horizontal and vertical lines
	//then they are saved for each line so I dont have to calculate them again
	float m1, m2, b1,b2,bdiff;
	float m[linesP.size()];
	float b[linesP.size()];

	int rightXi,rightYi,leftXi,leftYi,rightXj,rightYj,leftXj,leftYj;
	int topXi, topYi, bottomXi,bottomYi, topXj, topYj, bottomXj, bottomYj, topdiff;
	
	//keep track of which lines are just repeats (or part of a combined line)
	//matches = how many lines were similiar enough to be the same line
	//horizontal or vertical saves 1 if horizontal and a 2 if vertical
	int repeatabilityP[linesP.size()];
	int matchesP[linesP.size()];
	int horizontalOrVertical[linesP.size()];



	 for( size_t i = 0; i < linesP.size(); i++ )
   	 {
		repeatabilityP[i] = 0;
		matchesP[i] = 0;
		horizontalOrVertical[i] = 0;
	 }

   	 for( size_t i = 0; i < linesP.size(); i++ )
   	 {
	  if (repeatabilityP[i] < 1)
	  {
		//I have two points to define each line segment
		//first filter through and look for horizontal, slope near 0
		if (linesP[i][0] == linesP[i][2])
			m1 = 10000;
		else
			m1 = float(linesP[i][1]-linesP[i][3])/float(linesP[i][0]-linesP[i][2]);

		b1 = linesP[i][1]-m1*linesP[i][0];
		if (m1 < 0)
			m1 = -m1;

		if (m1 < horizontalslope)
		{
			//printf("\n Horizontal");
			horizontalOrVertical[i] = 1;
			
			//now I need to combine with other horizontal line segments
			 for( size_t j = i+1; j< linesP.size(); j++ )
   			 {
				//if its already labeled repeated then dont bother
				if (repeatabilityP[j] < 1)
				{
					 if (linesP[j][0] == linesP[j][2])
						m2 = 10000;
					else
						m2 = float(linesP[j][1]-linesP[j][3])/float(linesP[j][0]-linesP[j][2]);
					
					b2 = linesP[j][1]-m2*linesP[j][0];
					
					//step 1: check slopes
					if (m2 < horizontalslope)
					{ 
						//slopes are both more of less horizontal
						//check vertical distance between the end points

						//first find left most point of each line segment
						//line i:
						if (linesP[i][0] < linesP[i][2])
						{
							leftXi = linesP[i][0];
							leftYi = linesP[i][1];
							rightXi= linesP[i][2];
							rightYi = linesP[i][3];
						}
						else
						{
							leftXi = linesP[i][2];
							leftYi = linesP[i][3];
							rightXi= linesP[i][0];
							rightYi = linesP[i][1];
						}
						//line j:
						if (linesP[j][0] < linesP[j][2])
						{
							leftXj = linesP[j][0];
							leftYj = leftXi*m2 + b2;
							rightXj= linesP[j][2];
							rightYj = rightXi*m2+b2;
						}
						else
						{
							leftXj = linesP[j][2];
							leftYj = leftXi*m2 + b2;
							rightXj= linesP[j][0];
							rightYj = rightXi*m2+b2;
						}
							
						if (b1 < b2)
							 bdiff = b2-b1;
						else
							bdiff = b1-b2;
						//printf("\n bdiff = %f hieght1 = %d, height2 = %d",bdiff, height1,height2);
						if (bdiff < bdifflimit)
						{
							 //same line, so combine them
							repeatabilityP[j] = 2;
							matchesP[i] = matchesP[i]+1;
							//form the longest line
							if (leftXj < leftXi)
							{
								linesP[i][0] = leftXj;
								linesP[i][1] = leftYj;
							}

							if (rightXj > rightXi)
							{
								linesP[i][2] =rightXj;
								linesP[i][3] =rightYj;
							}
						} //end if a match
					  }//end if mj < 2
				} //end for j repeated
			 } //end for j
			//horizontal line
			//printf(" horizontal lines = %d", matchesP[i]);
			//horizontal
			line(src, Point(linesP[i][0], linesP[i][1]),
   	         	 Point(linesP[i][2], linesP[i][3]), Scalar(255,0,255), 2, 8 );
			
			//recalculate slope here for the new combine line, and save
			if (linesP[i][0] == linesP[i][2])
				m1 = 10000;
			else
				m1 = float(linesP[i][1]-linesP[i][3])/float(linesP[i][0]-linesP[i][2]);
	
			b1 = linesP[i][1]-m1*linesP[i][0];
			//if (m1 < 0)
			//	m1 = -m1;
			m[i] = m1;
			b[i] =b1;

		}//end for m1 < 2
		else if (m1 >verticleslope )
		{
			//Vertical Line

			horizontalOrVertical[i] = 2;
		 	if (linesP[i][1] < linesP[i][3])
			{
				bottomXi = linesP[i][0];
				bottomYi = linesP[i][1];
				topXi= linesP[i][2];
				topYi = linesP[i][3];
			}
			else
			{
				bottomXi = linesP[i][2];
				bottomYi = linesP[i][3];
				topXi= linesP[i][0];
				topYi = linesP[i][1];
				linesP[i][0] = bottomXi;
				linesP[i][1] = bottomYi;
				linesP[i][2] = topXi;		
				linesP[i][3] = topYi;
			}
			//printf("\n i: (%d, %d), (%d, %d)", linesP[i][0],linesP[i][1],linesP[i][2],linesP[i][3]);

			//now I need to combine with vertical line segments
			for( size_t j = i+1; j< linesP.size(); j++ )
   			 {
 				if (linesP[j][0] == linesP[j][2])
					m2 = 10000;
				else
					m2 = float(linesP[j][1]-linesP[j][3])/float(linesP[j][0]-linesP[j][2]);				
				b2 = linesP[j][1]-m2*linesP[j][0];

				if (m2 < 0)
					m2 = -m2;

				//if its already labeled repeated then dont bother
				if (m2 > verticleslope  && repeatabilityP[j] < 1)
				{
				  //slopes line up - now extend j line to length Y coordinates of line i
				  //or we can just compare where first X coordinates

					//line j:
					if (linesP[j][1] < linesP[j][3])	
					{
						bottomXj = linesP[j][0];
						bottomYj = linesP[j][1];
						topXj= linesP[j][2];
						topYj = linesP[j][3];
					}
					else
					{
						bottomXj = linesP[j][2];
						bottomYj = linesP[j][3];
						topXj= linesP[j][0];
						topYj = linesP[j][1];
					}

					if (topXi < topXj)
						topdiff = topXj - topXi;
					else
						topdiff = topXi-topXj;

					//printf("\n Vertical:bottom = %d %d, top= %d %d",bottomXi,bottomXj, topXi, topXj);
					if (topdiff < topdifflimit)
					{
						//same line
						repeatabilityP[j] = 1;
						matchesP[i] = matchesP[i] + 1;

						//find the longest line
						if (topYj > topYi)
						{
               						linesP[i][2] = topXj;
               						linesP[i][3] = topYj;
						}
						if (bottomYj < bottomYi)
						{
               						linesP[i][0] = bottomXj;
               						linesP[i][1] = bottomYj;
						}
					}
				}//end if repeated and slope limit
			}//end for j
			//verticel

			line(src, Point(linesP[i][0], linesP[i][1]),
   	          	 Point(linesP[i][2], linesP[i][3]), Scalar(255,255,0), 2, 8 ); 

			//find slope for new combined line and save
			if (linesP[i][0] == linesP[i][2])
				m1 = 10000;
			else
				m1 = float(linesP[i][1]-linesP[i][3])/float(linesP[i][0]-linesP[i][2]);
	
			b1 = linesP[i][1]-m1*linesP[i][0];
			//if (m1 < 0)
			//	m1 = -m1;
			m[i] = m1;
			b[i] =b1;

		}
		else
		{	//not a horizontal or vertical line

      			//line(src, Point(linesP[i][0], linesP[i][1]),
   	         	// Point(linesP[i][2], linesP[i][3]), Scalar(0,0,255), 1, 8 );
			//printf("\n m = %f", m1);
		}
           }//end if repeatability
   	 }


	//imshow("hough",src);
	//I should have all the lines now
	//want to find bottom_corner poitns ofas the rectangle
	//can find intercept of all horizontal and verticle lines

	//now to find intersections given a vertical and horizontal line - find the corner
	//also determine if the vertical is to hte left or right of the center of the horizontal

	int Xvalue, Yvalue,Xcenter,Ycenter,Xvert,Yvert;	
	int height;
	int width;

	foundLines finalLines;
	finalLines.left_bottom_corner.foundHedge = 0;
	finalLines.right_bottom_corner.foundHedge = 0;

	finalLines.left_top_corner.foundHedge = 0;
	finalLines.right_top_corner.foundHedge = 0;
	float aspectRatio_diff;
	float foundAspectRatio;
	float rho, theta;
           
	int totalVertical= 0;
 	for( size_t i = 0; i < linesP.size(); i++ )
   	{
		if (horizontalOrVertical[i] == 2)
			totalVertical= totalVertical+1;
	}
	int totalHorizontal= 0;
 	for( size_t i = 0; i < linesP.size(); i++ )
   	{
		if (horizontalOrVertical[i] == 1)
			totalHorizontal= totalHorizontal+1;
	}
	//printf("\n  \n totalVertical = %d", totalVertical);
	foundCorner corners[totalHorizontal*totalVertical];

	int counter =0;
 	for( size_t i = 0; i < linesP.size(); i++ )
   	{
		//printf("\n Repeatability = %d", repeatabilityP[i]);
		//printf(" HorizontalOrVertical %d ", horizontalOrVertical[i]);
		//printf(" Matches = %d",matchesP[i]);
	if (horizontalOrVertical[i] == 1)
		{
			
			//printf(" Horizontal ");	

			//find the center the horizontal line
			Xcenter = (linesP[i][0]+linesP[i][2])/2;
			Ycenter = (linesP[i][1]+linesP[i][3])/2;
			circle(src, Point(Xcenter, Ycenter),5,Scalar( 255, 0, 0),-1,8 );
		
			for( size_t j = 0; j < linesP.size(); j++ )
	   		{
			 	if (horizontalOrVertical[j] == 2)
				{
					//printf("Vertical");
					//determine centerpoint of the vertical line- only really care about the yvalue
					//I only want the yvalue to determine if the horizontal line is above/below the vertical line
					Yvert = (linesP[j][1]+linesP[j][3])/2;
					Xvert = (linesP[j][0]+linesP[j][2])/2;
					circle(src, Point(Xvert, Yvert),3,Scalar( 0, 255, 0),-1,8 );

				 	//determine if vertical is left or right of center of horizontal
					//want to find where the two intersect 
					//algebra:
					//m1 *x + b1 = m2*x+b2	;
					//x(m1-m2) = (b2-b1);
					Xvalue = (b[j]-b[i])/(m[i]-m[j]);
					if (Xvalue < 0)
						Xvalue = -Xvalue;
					Yvalue = m[i]*Xvalue+b[i];

					//want theta and rho of horizontal line - so I know what angle the gate is at
					//x1 cos θ + y1 sin θ = r
					//x2 cos θ + y2 sin θ = r
					//x1 cos θ + y1 sin = x2 cos θ + y2 sin 
					//cos (x1-x2) = sin (y2-y1)
					//tan = sin/cos = (y2-y1)/(x1-x2)
					theta = atan((linesP[i][3]-linesP[i][1])/(linesP[i][0]-linesP[i][2]));
					rho = linesP[i][0]*cos(theta)+linesP[i][1]*sin(theta);
					
					//all comparision between line groups is based onthe aspect ratios
					//aspect ratio= height/width
					height = Yvalue-linesP[j][1]; //take the top Y value of the vertical
					if ((Yvalue-linesP[j][3])>height)
						height = Yvalue-linesP[j][3];

					//width = width of the horizontal bar - just using X values	
					//but I dont want to just use the width. I want the furthest distance fromthe Xvalue

					width = linesP[i][0]-linesP[i][2];

					if ((linesP[i][2]-linesP[i][0]) > width)
					{
						width = linesP[i][2]-linesP[i][0];
					}
					else if ((linesP[i][2]-Xvalue)> width) 
					{
						width =  (linesP[i][2]-Xvalue) ;
					}
					else if ((Xvalue- linesP[i][2])>width)
					{
						width =	 (Xvalue- linesP[i][2]);
					}
					else if ((Xvalue- linesP[i][0])> width)
					{
						width =	 (Xvalue- linesP[i][0]);
					}
					else if ((linesP[i][0]-Xvalue) > width)
					{
						width =	 (linesP[i][0]-Xvalue);
					}
				
					//corners
					//circle(src, Point(Xvalue, Yvalue),5,Scalar(255, 0, 0),6,8 ); 
					//now I want to find the squares based on the corners
					//want to also find squares- do that based on aspect ratio... hmm...
					//I want to determine the point based on if its actually connected to the lines
					//width is the width of the horizontal bar from the X value to the end
					//is the point near the end of the vertical and horizontal line?
					//or do I want to find all corners then figure out which ones best makes a square?
					//or hedge if thats what I have
					//it would be useful to know what side of the corner, the lines attached to - left, right, top or bottom

					//each horizontal line should have (vertical number of lines) number of corners
					///so, there should be #vertical*#horizontal corners
					//if I take the nubmer of corners, I can than find which ones are made up of the same vertical lines
					//and than connect them

					corners[counter].width = 0;
					corners[counter].height = 0;
					corners[counter].horizontal = i;
					corners[counter].vertical = j;
					corners[counter].cornerX = Xvalue;
					corners[counter].cornerY = Yvalue;

					//problem when the corner is around hte middle of the line
					//so if its too close to the middle of hte line to tell, set to 2
					if (abs(Xvalue-Xcenter) < 10)
						corners[counter].left = 2;
					else if (Xvalue <= Xcenter) 
						corners[counter].left = 1;
					else
						corners[counter].left = 0;

					if (abs(Yvalue-Ycenter) < 10)
						corners[counter].bottom = 2;
					else if (Yvalue <= Yvert) 
						corners[counter].bottom = 1;
					else
						corners[counter].bottom = 0;
						
					//find furthest pixel vertical from the corner
					corners[counter].maxY = linesP[i][1];	
					if (abs(corners[counter].maxY - linesP[i][3]) > corners[counter].maxY)
						corners[counter].maxY = linesP[i][3];	
					if (abs(corners[counter].maxY - linesP[j][1]) > corners[counter].maxY)
						corners[counter].maxY = linesP[j][1];	
					if (abs(corners[counter].maxY - linesP[j][3]) > corners[counter].maxY)
						corners[counter].maxY = linesP[j][3];	
					

					if (abs(corners[counter].cornerY- linesP[i][1]) > corners[counter].height)
						corners[counter].height = abs(corners[counter].cornerY- linesP[i][1]);
					if (abs(corners[counter].cornerY- linesP[i][3]) > corners[counter].height)
						corners[counter].height = abs(corners[counter].cornerY- linesP[i][3]);
					if (abs(corners[counter].cornerY- linesP[j][1]) > corners[counter].height)
						corners[counter].height = abs(corners[counter].cornerY- linesP[j][1]);
					if (abs(corners[counter].cornerY- linesP[j][3]) > corners[counter].height)
						corners[counter].height = abs(corners[counter].cornerY- linesP[j][3]);
					
					if (abs(corners[counter].cornerX- linesP[i][0]) > corners[counter].width)
						corners[counter].width = abs(corners[counter].cornerX- linesP[i][0]);
					if (abs(corners[counter].cornerX- linesP[i][2]) > corners[counter].width)
						corners[counter].width = abs(corners[counter].cornerX- linesP[i][2]);
					if (abs(corners[counter].cornerX- linesP[j][0]) > corners[counter].width)
						corners[counter].width = abs(corners[counter].cornerX- linesP[j][0]);
					if (abs(corners[counter].cornerX- linesP[j][2]) > corners[counter].width)
						corners[counter].width = abs(corners[counter].cornerX- linesP[j][2]);

					circle(src, Point(corners[counter].cornerX, corners[counter].cornerY),3,Scalar(0, 0,255),-1,8 );

					counter = counter+1;
				}//end if vertical
			} //end for j
		} //end if horizontal 
	}//end for i
//printf("\nnumber of corners = %d", counter);

int cornerpoints;
int j,k;
//int cornerpointsk;
foundRectangle finalpoints;
//finalpoints.foundAspectRatio_difference = 99999;
//finalpoints.numberofcorners = 0;
int Xother, Yother,diff;
int c3,c4;
int allowableAspectRatio_diff = 100;
int numberoffinals = -1;

for (unsigned int p =0;p<5;p++)
{
	final[p].foundAspectRatio_difference = 9000;
}

for (int i = 0;i<counter;i++)
{
	//for each point go through and find opposite corner on square
	cornerpoints = 1;
	height = corners[i].height;
	width = corners[i].width;
	foundAspectRatio = (float(height)/float(width))*1000;
	if (foundAspectRatio > 1000)
		foundAspectRatio = (float(width)/float(height))*1000;
	if (foundAspectRatio < aspectRatio)
		aspectRatio_diff = aspectRatio-foundAspectRatio;
	else
		aspectRatio_diff = -aspectRatio+foundAspectRatio;

	//printf("\n i %d ,height = %d, width = %d",i,height,width);
	//printf(" aspectratio = %f",foundAspectRatio);

	//want to find the opposite corner
	//but if its too close to the center to tell corners.left == 2
	//so i need to search for those, as well
	for (j = 0;j<counter;j++) 
	{
		if ( ((corners[j].left  != corners[i].left) || (corners[i].left==2) || (corners[j].left==2)) && ((corners[j].bottom != corners[i].bottom) || (corners[i].bottom == 2) || (corners[j].bottom == 2)) )
		{ 
			//find opposite corner
			cornerpoints = 2; //or I could just increment up but this seems safer
			height = abs(corners[i].cornerY - corners[j].cornerY);
			width = abs(corners[i].cornerX - corners[j].cornerX);

			//now look for other corners
			if (corners[i].left == 1)
				Xother = corners[i].cornerX+width;
			else
				Xother = corners[i].cornerX-width;
			if (corners[i].bottom == 1)
				Yother = corners[i].cornerY+height;
			else
				Yother= corners[i].cornerY-height;

			for (k = 0;k<counter;k++)
			{
				if ((corners[k].left == corners[i].left) || corners[k].left == 2 || corners[i].left ==2) 
				{
					//should be on the same vertical line then
					diff = abs(corners[i].cornerX-corners[k].cornerX)+abs(Yother-corners[k].cornerY);
					
				}
				else if (corners[k].bottom == corners[i].bottom)
				{
					//should be on same horizontal line
					diff = abs(Xother-corners[k].cornerX)+abs(corners[i].cornerY-corners[k].cornerY);
				}
				else
				{
					//opposite of square, which I dont want 'cause thats part of j
					diff = 99999;
				}
				if ((k!=i) && (k!=j) && (diff < cornerdifference))
				{
					//looking for a corner at (Xother, corners[i].y, and at coerns[i].x, Yother
					//is where it should be, so part of the square
					cornerpoints = cornerpoints+1;
					if (cornerpoints == 3)
						c3 = k;
					if (cornerpoints > 3)
						c4 = k;
					//may have some difficulties with more points but whatever for now
					//printf(" lots of corners, i = %d j= %d",i,j);
				}
			} //end for k
		} //end if opposite


		foundAspectRatio = (float(height)/float(width))*1000;	
		aspectRatio_diff = abs(aspectRatio-foundAspectRatio);

		//printf("\n aspectratio = %f",foundAspectRatio);

		//if ((aspectRatio_diff < finalpoints.foundAspectRatio_difference) || ((aspectRatio_diff < finalpoints.foundAspectRatio_difference*1.5) && (cornerpoints > finalpoints.numberofcorners)))

//only save the top 5
		if (aspectRatio_diff < allowableAspectRatio_diff && width > 30 && height > 30)
		{	
			printf("\n width = %d, height   %d",width, height);
			numberoffinals = numberoffinals+1;
			//foundRectangle* finalpoints = new foundRectangle;
			finalpoints.corner1.x = corners[i].cornerX;
			finalpoints.corner1.y = corners[i].cornerY;
			finalpoints.foundAspectRatio = foundAspectRatio;
			finalpoints.foundAspectRatio_difference = aspectRatio_diff;
			finalpoints.numberofcorners = cornerpoints;
			if (cornerpoints < 2)
			{
				//only found one point		
				if (corners[i].left == 1)
					finalpoints.corneropposite.x = corners[i].cornerX+width;
				else
					finalpoints.corneropposite.x = corners[i].cornerX-width;

				if (corners[i].bottom == 1)
					finalpoints.corneropposite.y = corners[i].cornerY+height;
				else
					finalpoints.corneropposite.y = corners[i].cornerY-height;
			}
			else
			{

				finalpoints.corner2.x = corners[j].cornerX;
				finalpoints.corner2.y = corners[j].cornerY;
				finalpoints.corneropposite.x = corners[j].cornerX;
				finalpoints.corneropposite.y = corners[j].cornerY;
				if (cornerpoints > 2)
				{
					 finalpoints.corner3.x = corners[c3].cornerX;
					 finalpoints.corner3.y = corners[c3].cornerY;
					 if (cornerpoints > 3)
					 {
						finalpoints.corner4.x = corners[c4].cornerX;
						finalpoints.corner4.y = corners[c4].cornerY;
					 } //end corner > 3
				}//end corner > 2
				finalpoints.c1 = i;	
				finalpoints.c2 = j;
				finalpoints.c3 = c3;
				finalpoints.c4 = c4;
			}
			finalpoints.height = abs(finalpoints.corner1.y-finalpoints.corneropposite.y);
			finalpoints.width = abs(finalpoints.corner1.x-finalpoints.corneropposite.x);
			finalpoints.center.x = (finalpoints.corner1.x+finalpoints.corneropposite.x)/2;
			finalpoints.center.y = (finalpoints.corner1.y+finalpoints.corneropposite.y)/2;

	//printf("\n number of corners = %d,  i=%d, j=%d,k=%d,p=%d, aspect = %f, height =%d width  =%d", finalpoints.numberofcorners,finalpoints.c1,finalpoints.c2,finalpoints.c3,finalpoints.c4,finalpoints.foundAspectRatio_difference, finalpoints.height, finalpoints.width);



			int used = 0;

			for (unsigned int p=0;p<5;p++)
			{
				if (aspectRatio_diff < final[p].foundAspectRatio_difference && used == 0)
				{		
					for (unsigned int piter=p; piter<4; piter++)				
					{						
						int pcounter = 4-p;
						final[pcounter] = final[pcounter-1];
					}
					final[p] = finalpoints;
				 	used = 1;
				}
				
			}
			
		}  //end if aspect ratio

	}//end for j

} //end for i
//printf("\n\n fNEXT");

for (unsigned int i =0;i<5;i++)
{
	if (final[i].foundAspectRatio_difference < 200)
	{
		cv::rectangle(src, final[i].corner1, final[i].corneropposite, Scalar(0,0,50*i), 2, 8, 0);

		circle(src, final[i].corner1,5,Scalar(0, 0,255),-1,8 );

		circle(src, final[i].center,5,Scalar(0,255,255),-1,8 );
		if (final[i].numberofcorners >1 )
			circle(src, final[i].corner2,5,Scalar(0, 255,0),-1,8 );
		if (final[i].numberofcorners >2 )
			circle(src, final[i].corner3,5,Scalar(255, 0,255),-1,8 );
		if (final[i].numberofcorners >3)
			circle(src, final[i].corner4,5,Scalar(100, 0,255),-1,8 );
	}
 
	//printf("\n yay i  = %d, center =( %d, %d ), height %d, width %d",i,final[i].center.x,final[i].center.y, final[i].height, final[i].width);

}

//check previous frames
//checking the center of the different squares
for (unsigned int i =0;i<5;i++)
{
	final[i].previousdiff = 100;
	final[i].previous2diff = 100;
	for (unsigned int j =0;j<5;j++)
	{
		if (final[i].foundAspectRatio_difference < 200 && previous[j].foundAspectRatio_difference < 200)
		{
			diff= abs(final[i].center.x-previous[j].center.x)+abs(final[i].center.y-previous[j].center.y);
			if (diff < final[i].previousdiff)
				final[i].previousdiff = diff;
		}//end if
	}//end for j
	for (unsigned int k =0;k<5;k++)
	{
		if (final[i].foundAspectRatio_difference < 200 && previous2[k].foundAspectRatio_difference < 200)
		{
			diff = abs(final[i].center.x-previous2[k].center.x)+abs(final[i].center.y-previous2[k].center.y);
			if (diff < final[i].previous2diff)
				final[i].previous2diff = diff;
		}//end if
	}//end for k

	if (final[i].previous2diff < 40 && final[i].previousdiff < 20)
		cv::rectangle(src, final[i].corner1, final[i].corneropposite, Scalar(0,0,50*i), 10, 8, 0);

	//printf("\n i  = %d, previous = %d, %d, height %d, width %d",i,final[i].previousdiff,final[i].previous2diff, final[i].height, final[i].width);
}//end  for i






/*
					//how far away is the corner from teh end of the lines
	
					//printf(" width = %d, Xvalue = %d, other %d ", width, Xvalue,linesP[i][0]);

					foundAspectRatio = (float(height)/float(width));

					//compared calculated aspect ratios- closest one to the known aspect ratio wins
					if (foundAspectRatio < aspectRatio)
						aspectRatio_diff = aspectRatio-foundAspectRatio;
					else
						aspectRatio_diff = -aspectRatio+foundAspectRatio;

					//printf("\n AspectRatio = %f, height %d, width =%d diff = %f ", foundAspectRatio,height,width,aspectRatio_diff);	

					if ((Xvalue < Xcenter) && (Yvalue <= Yvert)) //&& (aspectRatio_diff < 2) 
					{	
								finalLines.left_bottom_corner.foundHedge = 1;
								finalLines.left_bottom_corner.cornerX = Xvalue;
								finalLines.left_bottom_corner.cornerY = Yvalue;
								finalLines.left_bottom_corner.foundWidth = width;
								finalLines.left_bottom_corner.foundHeight = height;
								finalLines.left_bottom_corner.estimatedWidth = float(height)/aspectRatio;
								finalLines.left_bottom_corner.estimatedHeight = float(width)*aspectRatio;
								finalLines.left_bottom_corner.foundAspectRatio = foundAspectRatio;
								finalLines.left_bottom_corner.foundAspectRatio_difference = aspectRatio_diff;
								finalLines.left_bottom_corner.horizontalX1 = linesP[i][0];
								finalLines.left_bottom_corner.horizontalY1 = linesP[i][1];
								finalLines.left_bottom_corner.horizontalX2 = linesP[i][2];
								finalLines.left_bottom_corner.horizontalY2 = linesP[i][3];
								finalLines.left_bottom_corner.verticalX1 = linesP[j][0];
								finalLines.left_bottom_corner.verticalY1 = linesP[j][1];
								finalLines.left_bottom_corner.verticalX2 = linesP[j][2];
								finalLines.left_bottom_corner.verticalY2 = linesP[j][3];
								finalLines.left_bottom_corner.rho = rho;
								finalLines.left_bottom_corner.theta = theta;
						circle(src, Point(finalLines.left_bottom_corner.cornerX, finalLines.left_bottom_corner.cornerY),5,Scalar(0, 0,255),6,8 );
					}

					if ((Xvalue < Xcenter) && (Yvalue >= Yvert)) //&& (aspectRatio_diff < 2) 
					{	
								finalLines.left_top_corner.foundHedge = 1;
								finalLines.left_top_corner.cornerX = Xvalue;
								finalLines.left_top_corner.cornerY = Yvalue;
								finalLines.left_top_corner.foundWidth = width;
								finalLines.left_top_corner.foundHeight = height;
								finalLines.left_top_corner.estimatedWidth = float(height)/aspectRatio;
								finalLines.left_top_corner.estimatedHeight = float(width)*aspectRatio;
								finalLines.left_top_corner.foundAspectRatio = foundAspectRatio;
								finalLines.left_top_corner.foundAspectRatio_difference = aspectRatio_diff;
								finalLines.left_top_corner.horizontalX1 = linesP[i][0];
								finalLines.left_top_corner.horizontalY1 = linesP[i][1];
								finalLines.left_top_corner.horizontalX2 = linesP[i][2];
								finalLines.left_top_corner.horizontalY2 = linesP[i][3];
								finalLines.left_top_corner.verticalX1 = linesP[j][0];
								finalLines.left_top_corner.verticalY1 = linesP[j][1];
								finalLines.left_top_corner.verticalX2 = linesP[j][2];
								finalLines.left_top_corner.verticalY2 = linesP[j][3];
								finalLines.left_top_corner.rho = rho;
								finalLines.left_top_corner.theta = theta;
						circle(src, Point(finalLines.left_top_corner.cornerX, finalLines.left_top_corner.cornerY),5,Scalar(100, 0,255),8,8 );
					}
					if ((Xvalue > Xcenter) && (Yvalue <= Yvert)) //&& (aspectRatio_diff < 2) 
					{	
								finalLines.right_bottom_corner.foundHedge = 1;
								finalLines.right_bottom_corner.cornerX = Xvalue;
								finalLines.right_bottom_corner.cornerY = Yvalue;
								finalLines.right_bottom_corner.foundWidth = width;
								finalLines.right_bottom_corner.foundHeight = height;
								finalLines.right_bottom_corner.estimatedWidth = float(height)/aspectRatio;
								finalLines.right_bottom_corner.estimatedHeight = float(width)*aspectRatio;
								finalLines.right_bottom_corner.foundAspectRatio = foundAspectRatio;
								finalLines.right_bottom_corner.foundAspectRatio_difference = aspectRatio_diff;
								finalLines.right_bottom_corner.horizontalX1 = linesP[i][0];
								finalLines.right_bottom_corner.horizontalY1 = linesP[i][1];
								finalLines.right_bottom_corner.horizontalX2 = linesP[i][2];
								finalLines.right_bottom_corner.horizontalY2 = linesP[i][3];
								finalLines.right_bottom_corner.verticalX1 = linesP[j][0];
								finalLines.right_bottom_corner.verticalY1 = linesP[j][1];
								finalLines.right_bottom_corner.verticalX2 = linesP[j][2];
								finalLines.right_bottom_corner.verticalY2 = linesP[j][3];
								finalLines.right_bottom_corner.rho = rho;
								finalLines.right_bottom_corner.theta = theta;
						circle(src, Point(finalLines.right_bottom_corner.cornerX, finalLines.right_bottom_corner.cornerY),5,Scalar(0, 50,255),10,8 );
					}
					if ((Xvalue > Xcenter) && (Yvalue >= Yvert)) //&& (aspectRatio_diff < 2) 
					{	
								finalLines.right_top_corner.foundHedge = 1;
								finalLines.right_top_corner.cornerX = Xvalue;
								finalLines.right_top_corner.cornerY = Yvalue;
								finalLines.right_top_corner.foundWidth = width;
								finalLines.right_top_corner.foundHeight = height;
								finalLines.right_top_corner.estimatedWidth = float(height)/aspectRatio;
								finalLines.right_top_corner.estimatedHeight = float(width)*aspectRatio;
								finalLines.right_top_corner.foundAspectRatio = foundAspectRatio;
								finalLines.right_top_corner.foundAspectRatio_difference = aspectRatio_diff;
								finalLines.right_top_corner.horizontalX1 = linesP[i][0];
								finalLines.right_top_corner.horizontalY1 = linesP[i][1];
								finalLines.right_top_corner.horizontalX2 = linesP[i][2];
								finalLines.right_top_corner.horizontalY2 = linesP[i][3];
								finalLines.right_top_corner.verticalX1 = linesP[j][0];
								finalLines.right_top_corner.verticalY1 = linesP[j][1];
								finalLines.right_top_corner.verticalX2 = linesP[j][2];
								finalLines.right_top_corner.verticalY2 = linesP[j][3];
								finalLines.right_top_corner.rho = rho;
								finalLines.right_top_corner.theta = theta;
						circle(src, Point(finalLines.right_top_corner.cornerX, finalLines.right_top_corner.cornerY),5,Scalar(0, 255,255),12,8 );
					}

/-
					else if ((Xvalue < Xcenter) && (aspectRatio_diff < 2) && (Yvalue >= Ycenter))
					{
						if ((finalLines.left_top_corner.foundHedge < 1) || ((finalLines.left_top_corner.foundHedge ==1) && (aspectRatio_diff < finalLines.left_top_corner.foundAspectRatio_difference))) 
						{
	
								finalLines.left_top_corner.foundHedge = 1;
								finalLines.left_top_corner.cornerX = Xvalue;
								finalLines.left_top_corner.cornerY = Yvalue;
								finalLines.left_top_corner.foundWidth = width;
								finalLines.left_top_corner.foundHeight = height;
								finalLines.left_top_corner.estimatedWidth = float(height)/aspectRatio;
								finalLines.left_top_corner.estimatedHeight = float(width)*aspectRatio;
								finalLines.left_top_corner.foundAspectRatio = foundAspectRatio;
								finalLines.left_top_corner.foundAspectRatio_difference = aspectRatio_diff;
								finalLines.left_top_corner.horizontalX1 = linesP[i][0];
								finalLines.left_top_corner.horizontalY1 = linesP[i][1];
								finalLines.left_top_corner.horizontalX2 = linesP[i][2];
								finalLines.left_top_corner.horizontalY2 = linesP[i][3];
								finalLines.left_top_corner.verticalX1 = linesP[j][0];
								finalLines.left_top_corner.verticalY1 = linesP[j][1];
								finalLines.left_top_corner.verticalX2 = linesP[j][2];
								finalLines.left_top_corner.verticalY2 = linesP[j][3];
								finalLines.left_top_corner.rho = rho;
								finalLines.left_top_corner.theta = theta;


						}
						
						//circle(src, Point(finalLines.left_corner.cornerX, finalLines.left_corner.cornerY),5,Scalar( 0, 0, 255 ),-1,8 );
					}

					else  if((Xvalue > Xcenter) && (aspectRatio_diff < 2) && (Yvalue <= Ycenter))
					{
						if ((finalLines.right_bottom_corner.foundHedge < 1) || ((finalLines.right_bottom_corner.foundHedge ==1) && (aspectRatio_diff < finalLines.right_bottom_corner.foundAspectRatio_difference))) 
						{
	
								finalLines.right_bottom_corner.foundHedge = 1;
								finalLines.right_bottom_corner.cornerX = Xvalue;
								finalLines.right_bottom_corner.cornerY = Yvalue;
								finalLines.right_bottom_corner.foundWidth = width;
								finalLines.right_bottom_corner.foundHeight = height;
								finalLines.right_bottom_corner.estimatedWidth = float(height)/aspectRatio;
								finalLines.right_bottom_corner.estimatedHeight = float(width)*aspectRatio;
								finalLines.right_bottom_corner.foundAspectRatio = foundAspectRatio;
								finalLines.right_bottom_corner.foundAspectRatio_difference = aspectRatio_diff;
								finalLines.right_bottom_corner.horizontalX1 = linesP[i][0];
								finalLines.right_bottom_corner.horizontalY1 = linesP[i][1];
								finalLines.right_bottom_corner.horizontalX2 = linesP[i][2];
								finalLines.right_bottom_corner.horizontalY2 = linesP[i][3];
								finalLines.right_bottom_corner.verticalX1 = linesP[j][0];
								finalLines.right_bottom_corner.verticalY1 = linesP[j][1];
								finalLines.right_bottom_corner.verticalX2 = linesP[j][2];
								finalLines.right_bottom_corner.verticalY2 = linesP[j][3];
								finalLines.right_bottom_corner.rho = rho;
								finalLines.right_bottom_corner.theta = theta;						
						}						
						//circle(src, Point(finalLines.right_corner.cornerX, finalLines.right_corner.cornerY),5,Scalar( 0, 255, 0 ),-1,8 );
					}


					else  if((Xvalue > Xcenter) && (aspectRatio_diff < 2) && (Yvalue >= Ycenter))
					{
						if ((finalLines.right_top_corner.foundHedge < 1) || ((finalLines.right_top_corner.foundHedge ==1) && (aspectRatio_diff < finalLines.right_top_corner.foundAspectRatio_difference))) 
						{
	
								finalLines.right_top_corner.foundHedge = 1;
								finalLines.right_top_corner.cornerX = Xvalue;
								finalLines.right_top_corner.cornerY = Yvalue;
								finalLines.right_top_corner.foundWidth = width;
								finalLines.right_top_corner.foundHeight = height;
								finalLines.right_top_corner.estimatedWidth = float(height)/aspectRatio;
								finalLines.right_top_corner.estimatedHeight = float(width)*aspectRatio;
								finalLines.right_top_corner.foundAspectRatio = foundAspectRatio;
								finalLines.right_top_corner.foundAspectRatio_difference = aspectRatio_diff;
								finalLines.right_top_corner.horizontalX1 = linesP[i][0];
								finalLines.right_top_corner.horizontalY1 = linesP[i][1];
								finalLines.right_top_corner.horizontalX2 = linesP[i][2];
								finalLines.right_top_corner.horizontalY2 = linesP[i][3];
								finalLines.right_top_corner.verticalX1 = linesP[j][0];
								finalLines.right_top_corner.verticalY1 = linesP[j][1];
								finalLines.right_top_corner.verticalX2 = linesP[j][2];
								finalLines.right_top_corner.verticalY2 = linesP[j][3];
								finalLines.right_top_corner.rho = rho;
								finalLines.right_top_corner.theta = theta;						
						}						
						//circle(src, Point(finalLines.right_corner.cornerX, finalLines.right_corner.cornerY),5,Scalar( 0, 255, 0 ),-1,8 );
					}
//-/
				}//end if vertical
			} //end for j
		} //end if horizontal 
	}//end for i
*/

//Needs to be editted to take into account four corners
/*
	//but if totalVertical= 1, then I can only have a left or a right. I can't have both
	if ((totalVertical == 1) && (finalLines.right_bottom_corner.foundHedge==1) && (finalLines.left_bottom_corner.foundHedge==1))
	{
		//only save the one wiht the smallest aspectratio_difference
		printf("\n left = %f, right = %f", finalLines.left_bottom_corner.foundAspectRatio_difference, finalLines.right_bottom_corner.foundAspectRatio_difference);
		if (finalLines.left_bottom_corner.foundAspectRatio_difference < finalLines.right_bottom_corner.foundAspectRatio_difference)
		{
			finalLines.right_bottom_corner.foundHedge = 0;

		}
		else
		{
			finalLines.left_bottom_corner.foundHedge = 0;
		}

	}

	//if left and right ones were found, see if the horizontal bar is the same
	//if so, then indicate we found both corners of the hedge
*/
/*
	if (finalLines.right_bottom_corner.foundHedge == 1 )
	{
			line(src, Point(finalLines.right_bottom_corner.horizontalX1, finalLines.right_bottom_corner.horizontalY1),
					   	         	 Point(finalLines.right_bottom_corner.horizontalX2, finalLines.right_bottom_corner.horizontalY2), Scalar(255,50,255), 12, 8 );
			line(src, Point(finalLines.right_bottom_corner.verticalX1, finalLines.right_bottom_corner.verticalY1),
				   	         	 Point(finalLines.right_bottom_corner.verticalX2, finalLines.right_bottom_corner.verticalY2), Scalar(0,0,255), 12, 8 );
	}
	if (finalLines.right_top_corner.foundHedge == 1)
	{
		line(src, Point(finalLines.right_top_corner.horizontalX1, finalLines.right_top_corner.horizontalY1),
		   	         	 Point(finalLines.right_top_corner.horizontalX2, finalLines.right_top_corner.horizontalY2), Scalar(255,50,255), 6, 8 );
		line(src, Point(finalLines.right_top_corner.verticalX1, finalLines.right_top_corner.verticalY1),
	   	         	 Point(finalLines.right_top_corner.verticalX2, finalLines.right_top_corner.verticalY2), Scalar(0,255,255), 6, 8 );
		printf("\n Partial Hedge Found - Right Side");
	}
	if (finalLines.left_bottom_corner.foundHedge == 1)
	{
		line(src, Point(finalLines.left_bottom_corner.horizontalX1, finalLines.left_bottom_corner.horizontalY1),
		   	         	 Point(finalLines.left_bottom_corner.horizontalX2, finalLines.left_bottom_corner.horizontalY2), Scalar(255,0,255), 6, 8 );
		line(src, Point(finalLines.left_bottom_corner.verticalX1, finalLines.left_bottom_corner.verticalY1),
		   	         	 Point(finalLines.left_bottom_corner.verticalX2, finalLines.left_bottom_corner.verticalY2), Scalar(0,255,0), 6, 8 );
		printf("\n Partial Hedge Found - Left Side");
	}
	if (finalLines.left_top_corner.foundHedge == 1)
	{
		line(src, Point(finalLines.left_top_corner.horizontalX1, finalLines.left_top_corner.horizontalY1),
		   	         	 Point(finalLines.left_top_corner.horizontalX2, finalLines.left_top_corner.horizontalY2), Scalar(255,0,255), 6, 8 );
		line(src, Point(finalLines.left_top_corner.verticalX1, finalLines.left_top_corner.verticalY1),
		   	         	 Point(finalLines.left_top_corner.verticalX2, finalLines.left_top_corner.verticalY2), Scalar(255,0,0), 6, 8 );
		printf("\n Partial Hedge Found - Left Side");
	}
*/
	/*
circle(src, Point(finalLines.right_bottom_corner.cornerX,finalLines.right_bottom_corner.cornerY),5,Scalar(0, 255, 0),8,8 );
circle(src, Point(finalLines.right_top_corner.cornerX, finalLines.right_top_corner.cornerY),5,Scalar(0, 255, 100),8,8 );
circle(src, Point(finalLines.left_bottom_corner.cornerX, finalLines.left_bottom_corner.cornerY),5,Scalar(0, 0,255),4,8 );
circle(src, Point(finalLines.left_top_corner.cornerX, finalLines.left_top_corner.cornerY),5,Scalar(0, 100, 255),4,8 );
*/

	//imshow("Final",src);

	//printf("\n \n complete!!!");
return( src);
	
}
}//end namspace
}//end nsamepace
