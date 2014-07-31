/*
White balance filter
Input:Mat Image
Output: Mat Image that has been white balanced

Performs Minkowski Method for white balance
Doesn't require any thresholding or inputs

based off of matlab code general_cc.m based on J. Van de Weijer
2013 Kate McBryan
*/

/*
Saliency filter
Input:Mat Image
Output: Mat Image that only has salient parts

Based on Saliency filter from Sebastian Montabone (found on the interwebs)

2014 Kate McBryan
*/


#include "vision/include/WhiteBalance.h"
#include "vision/include/saliency.h"


namespace ram {
namespace vision {

Mat HueShifter_RedMinusGreen(Mat img, int hueshift_int, int bluramount)
{
	Mat img_hsv;	
	cvtColor(img,img_hsv,CV_RGB2HSV);

	//double hueshift = m_hueshiftnumber;
	vector<Mat> hsv_planes;
	split(img_hsv,hsv_planes);

	//printf("\n hueshift = %d, %f",hueshift_int,hueshift);
//	imshow("start",hsv_planes[0]);
	Scalar_<uint8_t> bgrPixel;
	uint8_t*pixelPtr2 = (uint8_t*)hsv_planes[0].data;
	int cn2 = hsv_planes[0].channels();
	for(int i = 0; i < hsv_planes[0].rows; i++)
	{
	    for(int j = 0; j < hsv_planes[0].cols; j++)
	    {
		bgrPixel.val[0] = pixelPtr2[i*hsv_planes[0].cols*cn2 + j*cn2 + 0]; // H
		bgrPixel.val[0] = int(bgrPixel.val[0]+hueshift_int);
		pixelPtr2[i*hsv_planes[0].cols*cn2 + j*cn2 + 0] = int(int(bgrPixel.val[0])%180);
		// do something with BGR values...
	    }
	}

	//imshow("end",hsv_planes[0]);
	merge(hsv_planes,img_hsv);
	Mat img_final;
	cvtColor(img_hsv,img_final,CV_HSV2BGR);
//	imshow("HueShift", img_final);

	//now subtract out the green channels
	split(img_final,hsv_planes);

//	imshow("PreBLUE", hsv_planes[0]);
//	imshow("PreRed", hsv_planes[2]);
//	imshow("PreGreen", hsv_planes[1]);
	//equalizeHist(hsv_planes[0], hsv_planes[0]);
	//equalizeHist(hsv_planes[1], hsv_planes[1]);
	//equalizeHist(hsv_planes[2], hsv_planes[2]);

//	int bluramount = m_bluramount;
	if (bluramount%2 < 1)		
	{
		//even and bad
		bluramount= bluramount+1;
	}

//	medianBlur (hsv_planes[0],hsv_planes[0], bluramount);
	medianBlur (hsv_planes[1],hsv_planes[1], bluramount);
	medianBlur (hsv_planes[2],hsv_planes[2], bluramount);
	
//	imshow("red", hsv_planes[2]);	
//	imshow("blue", hsv_planes[0]);
//	imshow("Green", hsv_planes[1]);
	//hsv_planes[2] = hsv_planes[2]-hsv_planes[1]; //subract out the green

	Mat tempred;
	subtract(hsv_planes[2],hsv_planes[1],tempred, noArray(), -1);
	hsv_planes[2] = tempred;
//	imshow("hsvplane2",hsv_planes[2]);
	return(hsv_planes[2]);
};

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

	//Input.copyTo(image_output);
	printf("Whitebalancedone");
	return image_output;
};	


Mat SaliencyFilter(Mat img)
{

	Mat image_output(img.size(),img.type());
	img.copyTo(image_output);

	if (img.data) 
	{
		printf("141");
		//Mat image_output(img.size(),img.type());
		//Mat image_output;
		Mat img_gray;
		cvtColor(img,img_gray,CV_BGR2GRAY);
		printf("\n 147");
		//Mat img_hsv;
		//cvtColor(img,img_hsv,CV_BGR2HSV);
		//vector<Mat> hsv_planes;
		//vector<Mat> sal_planes;
		//split(img,sal_planes);
		//Mat erosion_dst;
		//Mat thresh;
		namedWindow( "SalienceInput", CV_WINDOW_AUTOSIZE );
		imshow( "SalienceInput",img);


		IplImage* dstImg = cvCreateImage(cvSize(img.cols, img.rows), 8, 1);
		IplImage* srcImg= new IplImage(img_gray);
		
		printf("164");
		//get saliency
		Saliency saliency;
		saliency.calcIntensityChannel(srcImg, dstImg);
		Mat img_salience = cvarrToMat(dstImg,true);
		image_output = img_salience.clone();

		printf("172");
		namedWindow( "Salience", CV_WINDOW_AUTOSIZE );
		imshow( "Salience",image_output);
		printf("175");
		return image_output;
	}
	else
	{
		printf("No IMage!! for Salien");
		return image_output;
	}
}



Mat SaliencyFilterSingle(Mat img)
{
	Mat image_output(img.size(),img.type());
	img.copyTo(image_output);

	if (!img.empty()) 
	{
		//Mat image_output(img.size(),img.type());
		//Mat image_output;
		Mat img_gray; //(img.size(), CV_8U);
		cout<< "\n img.channels() = "<< img.channels();
		cout<< "img.type() = " <<img.type();

		if (img.channels()==  3)
			cvtColor(img,img_gray,CV_BGR2GRAY);
		else if (img.channels() == 1)
		{
			img_gray = img;
		}
		else
			printf("\n WTF in SaliencyFilter");

		IplImage* dstImg = cvCreateImage(cvSize(img_gray.cols, img_gray.rows), 8, 1);

		IplImage* srcImg= new IplImage(img_gray);

		//get saliency
		Saliency saliency;
		saliency.calcIntensityChannel(srcImg, dstImg);
		Mat img_salience(dstImg);

		image_output=img_salience.clone();
		cvReleaseImage(&dstImg);


	//	namedWindow( "Salience1", CV_WINDOW_AUTOSIZE );
	//	cvShowImage( "Salience1",srcImg);

		//namedWindow( "Salience", CV_WINDOW_AUTOSIZE );
		//imshow( "Salience",m_saliency);
		printf("175");
//		cvReleaseImage(&srcImg);

//		return image_output;
	}
	else
	{
		printf("No IMage!! for Salien");
//		return image_output;
		image_output=img.clone();

	}
return (image_output);
}





}//end namespace vision
}//end namespace RAM


