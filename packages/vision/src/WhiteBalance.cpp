/*
White balance filter
Input:Mat Image
Output: Mat Image that has been white balanced

Performs Minkowski Method for white balance
Doesn't require any thresholding or inputs

based off of matlab code general_cc.m based on J. Van de Weijer
2013 Kate McBryan
*/

#include "vision/include/WhiteBalance.h"

namespace ram {
namespace vision {

Mat WhiteBalance(Mat Input)
{

	//int njet = 0;
	double mink_norm = 5;
	//int sigma = 0;
	Mat image_output(Input.size(),Input.type());
	Input.copyTo(image_output);
	
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
	
	vector<Mat>output_planes;
	split(image_output,output_planes);

	//vector<Mat>output_planes2;
	//split(image_output,output_planes2);

	//int output_plane_blue = 2;
	//int output_plane_green = 1;
	//int output_plane_red = 0;

	//output_planes[output_plane_blue] = output_planes2[0];
	//output_planes[output_plane_green] = output_planes2[1];
	//output_planes[output_plane_red] = output_planes2[2];


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
	//printf("tempb = %f,%f,%f", tempb.val[0],tempb.val[1],tempb.val[2]);
	float tempb_b = tempb.val[0];
	float tempb_g = tempb.val[1];
	float tempb_r = tempb.val[2];

	//float temporary = tempb.val[output_plane_blue];
	white_b=pow(tempb_b,mink_inverse);	//tempb_b should be sum(kleur)*mask_im2
	white_g=pow(tempb_g,mink_inverse);
	white_r=pow(tempb_r,mink_inverse);

	//printf("\n white_b, r,g = %f, %f, %f", white_b,white_g,white_r);
	som = sqrt(pow(white_r,2)+pow(white_g,2)+pow(white_b,2));
	//printf("Som = %f \n", som);
	//printf(" %f %f %f ", white_b, white_g, white_r);
	white_b = white_b/som;
	white_g = white_g/som;
	white_r = white_r/som;

	//printf("white b, g, r = %f %f %f \n",white_b,white_g,white_r);

	double scale_b, scale_g, scale_r;
	double sqrtthree= 1.732050807;
	
	scale_b = (1/(white_b*sqrtthree));
	scale_g = (1/(white_g*sqrtthree));
	scale_r = (1/(white_r*sqrtthree));
	//printf(" scale = %f %f %f", scale_b, scale_g, scale_r);
	
//switch from output_planes from 0,1,2, to 2,1,0 since I think it maybe in BGR instead of RGB
	output_planes[0] = output_planes[0]*scale_b;
	output_planes[1] = output_planes[1]*scale_g;
	output_planes[2] = output_planes[2]*scale_r;

	vector<Mat>output_planes2;
	split(image_output,output_planes2);

	int output_plane_blue = 2;
	int output_plane_green = 1;
	int output_plane_red = 0;

	output_planes2[output_plane_blue] = output_planes[0];
	output_planes2[output_plane_green] = output_planes[1];
	output_planes2[output_plane_red] = output_planes[2];


	
	merge(output_planes2,image_output);
	
	//cvNamedWindow("Output",CV_WINDOW_AUTOSIZE);
	//imshow("Output",image_output);
	//cvWaitKey(0);

	return image_output;
}	
}//end namespace vision
}//end namespace RAM
