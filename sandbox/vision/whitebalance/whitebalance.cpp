/*
White balance filter
Input:Mat Image
Output: Mat Image that has been white balanced

Performs Minkowski Method for white balance
Doesn't require any thresholding or inputs

based off of matlab code by: ????
*/

#include "blobfinder.h"

Mat blobfinder::whitebalance(Mat Input)
{

	int njet = 0;
	double mink_norm = 5;
	int sigma = 0;
	Mat image_output;
	Input.copyTo(image_output);
	
	//display Input Image
	//cvNamedWindow("Display",CV_WINDOW_AUTOSIZE);
	//imshow("Display",Input);
	
	//Size of input
	int width = Input.rows; 
	int height = Input.cols;
	
	//Perform Minkowski Method for Mink_norm ~-=-1
	
	//So, kleur is saturating at 255, which I don't want. 
	Mat Input2(width,height,CV_32F);
	Input.convertTo(Input2,CV_32F);
	
	Mat kleur(width,height,CV_32F);//(Input);	
	pow(Input2,mink_norm,kleur);

	double white_b,white_g,white_r,som;
	CvScalar tempb;
	
	Mat ColSum;
	//Mat = cvCreateMat(1,width,CV_64FC1);
	reduce(kleur, ColSum, 0, CV_REDUCE_SUM,CV_64F);
	Mat ColSum2;
	reduce(ColSum,ColSum2,1,CV_REDUCE_SUM,CV_64F);
//	cout << "ColSum2 " << ColSum2;
//	cout <<"ColSUm " << ColSum;
	vector<Mat>output_planes;
	split(image_output,output_planes);

	vector<Mat>bgr_planes;
	split(kleur, bgr_planes);
	Scalar temp;
	temp.val[0] = 0;
	temp.val[1] = 0;
	temp.val[2] = 0;
	temp=sum(output_planes[0]); 
	
	double mink_inverse = 1/mink_norm;
	//printf("\n mink_norm = %f, inverse = %f", mink_norm, mink_inverse);
	
	tempb =sum(kleur);

	float tempb_b = tempb.val[0];

	printf("tempb = %f %f %f", tempb.val[0], tempb.val[1], tempb.val[2]);
	
	double tempb_g = tempb.val[1];
	double tempb_r = tempb.val[2];

	white_b=pow(tempb_b,mink_inverse);	
	white_g=pow(tempb_g,mink_inverse);
	white_r=pow(tempb_r,mink_inverse);
	som = sqrt(pow(white_r,2)+pow(white_g,2)+pow(white_b,2));
	printf("Som = %f \n", som);
	printf(" %f %f %f ", white_b, white_g, white_r);
	white_b = white_b/som;
	white_g = white_g/som;
	white_r = white_r/som;
	printf("white b, g, r = %f %f %f \n",white_b,white_g,white_r);

	double scale_b, scale_g, scale_r;

	scale_b = (1/(white_b*2));
	scale_g = (1/(white_g*2));
	scale_r = (1/(white_r*2));
	printf(" scale = %f %f %f", scale_b, scale_g, scale_r);
	
	output_planes[0] = output_planes[0]*scale_b;
	output_planes[1] = output_planes[1]*scale_g;
	output_planes[2] = output_planes[2]*scale_r;
	
	merge(output_planes,image_output);
	
	//cvNamedWindow("Output",CV_WINDOW_AUTOSIZE);
	//imshow("Output",image_output);
	//cvWaitKey(0);

	return image_output;
}	
