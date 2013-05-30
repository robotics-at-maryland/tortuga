
/**
* Automatic perspective correction for quadrilateral objects. See the tutorial at
* http://opencv-code.com/tutorials/automatic-perspective-correction-for-quadrilateral-objects/
*/
#include "rectangle.h"

int rectangle(char *argv)
{
	foundLines left,right;
	cv::Mat src = cv::imread(argv, CV_LOAD_IMAGE_COLOR);
	if (src.empty())
		return -1;
	
	cv::Mat bw;
	cv::cvtColor(src, bw, CV_BGR2GRAY);
	cv::blur(bw, bw, cv::Size(3, 3));

	//imshow("rectangle1", bw);


	cv::Canny(bw, bw, 40, 100, 3);
	//Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );


	//imshow("canny", bw);

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
  	  HoughLinesP(bw, linesP, 1, CV_PI/180, 35, 30, 10 );

	//calculate slope and yintercept  - used to filter out horizontal and vertical lines
	//then they are saved for each line so I dont have to calculate them again
	float m1, m2, b1,b2,bdiff;
	float m[linesP.size()];
	float b[linesP.size()];

	int height1,height2,rightXi,rightYi,leftXi,leftYi,rightXj,rightYj,leftXj,leftYj;
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

		if (m1 < 2)
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
					if (m2 < 2)
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
						if (bdiff < 40)
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
			//line(src, Point(linesP[i][0], linesP[i][1]),
   	         	// Point(linesP[i][2], linesP[i][3]), Scalar(255,0,0), 2, 8 );
			
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
		else if (m1 >6)
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
				if (m2 > 6 && repeatabilityP[j] < 1)
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
					if (topdiff < 50)
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

			//line(src, Point(linesP[i][0], linesP[i][1]),
   	          	// Point(linesP[i][2], linesP[i][3]), Scalar(0,255,255), 2, 8 ); 

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




	//I should have all the lines now

	//now to find intersections given a vertical and horizontal line - find the corner
	//also determine if the vertical is to hte left or right of the center of the horizontal

	int Xvalue, Yvalue,Xcenter,Ycenter;	
	int height;
	int width;
	float aspectRatio =0.75; //height/width
	foundLines finalLines;
	finalLines.left_corner.foundHedge = 0;
	finalLines.right_corner.foundHedge = 0;
	float aspectRatio_diff;
	float foundAspectRatio;
	float rho, theta;

	//have an issue if there is only one vertical line and two horizontals
	//it could get saved as both a left and right corner - which is wrong
	//so I just count the number of vertical lines here, and at the end pick the best one
	int totalVertical= 0;
 	for( size_t i = 0; i < linesP.size(); i++ )
   	{
		if (horizontalOrVertical[i] == 2)
			totalVertical= totalVertical+1;
	}
	//printf("\n  \n totalVertical = %d", totalVertical);

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
			//circle(src, Point(Xcenter, Ycenter),5,Scalar( 255, 0, 0),-1,8 );
		
			for( size_t j = 0; j < linesP.size(); j++ )
	   		{
			 	if (horizontalOrVertical[j] == 2)
				{
					//printf("Vertical");

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
				

					//printf(" width = %d, Xvalue = %d, other %d ", width, Xvalue,linesP[i][0]);

					foundAspectRatio = (float(height)/float(width));

					//compared calculated aspect ratios- closest one to the known aspect ratio wins
					if (foundAspectRatio < aspectRatio)
						aspectRatio_diff = aspectRatio-foundAspectRatio;
					else
						aspectRatio_diff = -aspectRatio+foundAspectRatio;

					//printf("\n AspectRatio = %f, height %d, width =%d diff = %f ", foundAspectRatio,height,width,aspectRatio_diff);	

					if ((Xvalue < Xcenter) && (aspectRatio_diff < 2))
					{
						if ((finalLines.left_corner.foundHedge < 1) || ((finalLines.left_corner.foundHedge ==1) && (aspectRatio_diff < finalLines.left_corner.foundAspectRatio_difference))) 
						{
	
								finalLines.left_corner.foundHedge = 1;
								finalLines.left_corner.cornerX = Xvalue;
								finalLines.left_corner.cornerY = Yvalue;
								finalLines.left_corner.foundWidth = width;
								finalLines.left_corner.foundHeight = height;
								finalLines.left_corner.estimatedWidth = float(height)/aspectRatio;
								finalLines.left_corner.estimatedHeight = float(width)*aspectRatio;
								finalLines.left_corner.foundAspectRatio = foundAspectRatio;
								finalLines.left_corner.foundAspectRatio_difference = aspectRatio_diff;
								finalLines.left_corner.horizontalX1 = linesP[i][0];
								finalLines.left_corner.horizontalY1 = linesP[i][1];
								finalLines.left_corner.horizontalX2 = linesP[i][2];
								finalLines.left_corner.horizontalY2 = linesP[i][3];
								finalLines.left_corner.verticalX1 = linesP[j][0];
								finalLines.left_corner.verticalY1 = linesP[j][1];
								finalLines.left_corner.verticalX2 = linesP[j][2];
								finalLines.left_corner.verticalY2 = linesP[j][3];
								finalLines.left_corner.rho = rho;
								finalLines.left_corner.theta = theta;


						}
						
						//circle(src, Point(finalLines.left_corner.cornerX, finalLines.left_corner.cornerY),5,Scalar( 0, 0, 255 ),-1,8 );
					}
					else  if((Xvalue > Xcenter) && (aspectRatio_diff < 2))
					{
						if ((finalLines.right_corner.foundHedge < 1) || ((finalLines.right_corner.foundHedge ==1) && (aspectRatio_diff < finalLines.right_corner.foundAspectRatio_difference))) 
						{
	
								finalLines.right_corner.foundHedge = 1;
								finalLines.right_corner.cornerX = Xvalue;
								finalLines.right_corner.cornerY = Yvalue;
								finalLines.right_corner.foundWidth = width;
								finalLines.right_corner.foundHeight = height;
								finalLines.right_corner.estimatedWidth = float(height)/aspectRatio;
								finalLines.right_corner.estimatedHeight = float(width)*aspectRatio;
								finalLines.right_corner.foundAspectRatio = foundAspectRatio;
								finalLines.right_corner.foundAspectRatio_difference = aspectRatio_diff;
								finalLines.right_corner.horizontalX1 = linesP[i][0];
								finalLines.right_corner.horizontalY1 = linesP[i][1];
								finalLines.right_corner.horizontalX2 = linesP[i][2];
								finalLines.right_corner.horizontalY2 = linesP[i][3];
								finalLines.right_corner.verticalX1 = linesP[j][0];
								finalLines.right_corner.verticalY1 = linesP[j][1];
								finalLines.right_corner.verticalX2 = linesP[j][2];
								finalLines.right_corner.verticalY2 = linesP[j][3];
								finalLines.right_corner.rho = rho;
								finalLines.right_corner.theta = theta;						
						}
						
						//circle(src, Point(finalLines.right_corner.cornerX, finalLines.right_corner.cornerY),5,Scalar( 0, 255, 0 ),-1,8 );
					
					}
				}//end if vertical
			} //end for j
		} //end if horizontal 
	}//end for i

	//but if totalVertical= 1, then I can only have a left or a right. I can't have both
	if ((totalVertical == 1) && (finalLines.right_corner.foundHedge==1) && (finalLines.left_corner.foundHedge==1))
	{
		//only save the one wiht the smallest aspectratio_difference
		printf("\n left = %f, right = %f", finalLines.left_corner.foundAspectRatio_difference, finalLines.right_corner.foundAspectRatio_difference);
		if (finalLines.left_corner.foundAspectRatio_difference < finalLines.right_corner.foundAspectRatio_difference)
		{
			finalLines.right_corner.foundHedge = 0;

		}
		else
		{
			finalLines.left_corner.foundHedge = 0;
		}

	}

	//if left and right ones were found, see if the horizontal bar is the same
	//if so, then indicate we found both corners of the hedge

	if (finalLines.right_corner.foundHedge == 1 && finalLines.left_corner.foundHedge == 1)
	{
		 if ((finalLines.right_corner.theta - finalLines.left_corner.theta < 1.0) && (finalLines.right_corner.rho - finalLines.left_corner.rho < 1.0))	
		{
			printf("\n Full Hedge Found!");
			finalLines.foundFullHedge = 1;
			line(src, Point(finalLines.right_corner.horizontalX1, finalLines.right_corner.horizontalY1),
					   	         	 Point(finalLines.right_corner.horizontalX2, finalLines.right_corner.horizontalY2), Scalar(0,0,255), 4, 8 );
			line(src, Point(finalLines.right_corner.verticalX1, finalLines.right_corner.verticalY1),
				   	         	 Point(finalLines.right_corner.verticalX2, finalLines.right_corner.verticalY2), Scalar(0,0,255), 4, 8 );
			line(src, Point(finalLines.left_corner.verticalX1, finalLines.left_corner.verticalY1),
		   	         	 Point(finalLines.left_corner.verticalX2, finalLines.left_corner.verticalY2), Scalar(0,0,255), 4, 8 );
		}
	}
	else if (finalLines.right_corner.foundHedge == 1)
	{
		line(src, Point(finalLines.right_corner.horizontalX1, finalLines.right_corner.horizontalY1),
		   	         	 Point(finalLines.right_corner.horizontalX2, finalLines.right_corner.horizontalY2), Scalar(0,255,255), 4, 8 );
		line(src, Point(finalLines.right_corner.verticalX1, finalLines.right_corner.verticalY1),
	   	         	 Point(finalLines.right_corner.verticalX2, finalLines.right_corner.verticalY2), Scalar(0,255,255), 4, 8 );
		printf("\n Partial Hedge Found - Right Side");
	}
	else if (finalLines.left_corner.foundHedge == 1)
	{
		line(src, Point(finalLines.left_corner.horizontalX1, finalLines.left_corner.horizontalY1),
		   	         	 Point(finalLines.left_corner.horizontalX2, finalLines.left_corner.horizontalY2), Scalar(255,0,0), 2, 8 );
		line(src, Point(finalLines.left_corner.verticalX1, finalLines.left_corner.verticalY1),
		   	         	 Point(finalLines.left_corner.verticalX2, finalLines.left_corner.verticalY2), Scalar(255,0,0), 2, 8 );
		printf("\n Partial Hedge Found - Left Side");
	}
	

	//imshow("Final",src);

	printf("\n \n complete!!!");
	return 0;
}
