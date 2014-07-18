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

Mat ShapeRecon(Mat erosion_dst, Mat img)
{
printf("Starting shaperecon");

	//CONTOURS WITH SALIENCY
	int lowThreshold= 70;
	int ratio = 3;
	int kernel_size = 3;
	int morph_size = 2;
	Mat element = getStructuringElement( MORPH_ELLIPSE, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );


	Canny(erosion_dst,erosion_dst, lowThreshold, lowThreshold*ratio, kernel_size );
	morphologyEx(erosion_dst,erosion_dst,MORPH_GRADIENT, element);




	  vector<vector<Point> > contours;
	  vector<Vec4i> hierarchy;
	RNG rng(12345);


	 findContours( erosion_dst, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

	  /// Draw contours
	Mat drawing = Mat::zeros( erosion_dst.size(), CV_8UC3 );

	printf("Made it through drawing contors!");



	//convex hull
	   vector<vector<Point> >hull( contours.size() );
	   for(unsigned int i = 0; i < contours.size(); i++ )
	      {  convexHull( Mat(contours[i]), hull[i], false ); 
	      }

	//Match shapes- need reference 
	Mat ref_img(img.size(),img.type());
	Mat ref_canny;
	ref_img = imread("../GateRef2.png");
	if (!ref_img.data)
	{
		rectangle(ref_img, Point(50,100), Point(350,105), Scalar( 255, 255, 255 ),10,10, 0);
	}
	//ref_img = SaliencyFilter(ref_img);


	cvtColor(ref_img,ref_canny,CV_BGR2GRAY);
	Canny(ref_canny,ref_canny, lowThreshold, lowThreshold*ratio, kernel_size );
	morphologyEx(ref_canny,ref_canny,MORPH_GRADIENT, element);



	namedWindow( "RefImageC", CV_WINDOW_AUTOSIZE );
	imshow("RefImageC",ref_canny);


	 vector<vector<Point> > ref_contours;
	  vector<Vec4i> ref_hierarchy;

	findContours(ref_canny, ref_contours, ref_hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );


	//   vector<vector<Point> >ref_hull( contours.size() );
	//   for(unsigned int i = 0; i < ref_contours.size(); i++ )
	//      {  convexHull( Mat(ref_contours[i]), ref_hull[i], false );  }


	for (unsigned int i = 0; i < ref_contours.size(); i++) 
	{
		if (ref_contours[i].size() > 100 )
		{
		Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
		drawContours( ref_img, ref_contours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
		}
	}
	namedWindow( "RefImage", CV_WINDOW_AUTOSIZE );
	imshow("RefImage",ref_img);
	printf("\n NEW FRAME");

double area_ref, area, area_final;
double rect_area_ref, rect_area;
double ref_AR, AR, AR_final; //aspect ratio
double AL, ref_AL, AL_final; //arch length
double perimeter, perimeter_final, perimeter_ref;
RotatedRect box, box_ref;
double bestMatch;
	////double hu[7];
	//double hu_ref[7];

//	double match;
	for (int idx = 0; idx >= 0; idx = hierarchy[idx][0]) {
	if (contours[idx].size() > 50)
		{
		for (unsigned int i = 0; i < ref_contours.size(); i++) {
			if (ref_contours[i].size() > 100)
			{
				
				vector<Point>& a = ref_contours[i];
				vector<Point>& b = contours[idx];
				//std::cout << "a.size = " << a.size() << ", b.size = " << b.size();
				bestMatch = matchShapes(a, b, CV_CONTOURS_MATCH_I3, 0);
				box = minAreaRect(contours[idx]);
				box_ref = minAreaRect(ref_contours[i]);
				rect_area = double(box.size.width*box.size.height);
				rect_area_ref = double(box_ref.size.width*box_ref.size.height);

				area_ref = contourArea(ref_contours[i]);
				area = contourArea(contours[idx]);

				if (box_ref.size.height < box_ref.size.width)
					ref_AR = double(box_ref.size.height/box_ref.size.width);
				else
					ref_AR = double(box_ref.size.width/box_ref.size.height);

				if (box.size.height < box.size.width)
					AR = double(box.size.height/box.size.width);
				else
					AR = double(box.size.width/box.size.height);

				if (double(area_ref/rect_area_ref) < double(area/rect_area))
					area_final = 1- double(area_ref/rect_area_ref) / double(area/rect_area) ;
				else
					area_final = 1- double(area/rect_area)/ double(area_ref/rect_area_ref) ;



				AL = arcLength(contours[idx],TRUE);
				ref_AL = arcLength(ref_contours[i],TRUE);

				
				if (AR < ref_AR)
					AR_final = 1- AR/ref_AR;
				else
					AR_final = 1- ref_AR/AR;

				// AL/ALref is just a size measurement, and not really a shape comparison

				if (AL < ref_AL)
					AL_final = 1- double(AL/area)/double(ref_AL/area_ref);
				else
					AL_final = 1- double(ref_AL/area_ref)/double(AL/area);

				if (AL < 2*(box.size.width+box.size.height))
					perimeter = 1-AL/((box.size.width+box.size.height)*2);
				else
					perimeter = 1-((box.size.width+box.size.height)*2)/AL;

				if (ref_AL < 2*(box_ref.size.width+box_ref.size.height))
					perimeter_ref = 1-ref_AL/((box_ref.size.width+box_ref.size.height)*2);
				else
					perimeter_ref = 1-((box_ref.size.width+box_ref.size.height)*2)/ref_AL;

				if ((perimeter) <(perimeter_ref))
					perimeter_final =1-double( double(perimeter) / double(perimeter_ref));
				else
					perimeter_final = 1-double( double(perimeter_ref) / double(perimeter));


printf("Area_final = %f, AR_final = %f, AL_final = %f Perimeter_final = %f, Area/Perimeter = %f, %f i = %d \n", area_final, AR_final, AL_final, perimeter_final, double(perimeter), double(perimeter_ref),i);
	
				double bestMatch_ref, AR_final_ref, area_final_ref;
				Scalar color = Scalar(255,255,255);
					bestMatch_ref = 0.0001;
					AR_final_ref = 1.2;
					area_final_ref = 1.2;

				if (i == 11)
				{
					color = Scalar(255,255,255);
					bestMatch_ref = 2.0;
					AR_final_ref = 0.5;
					area_final_ref = .5;
				}		
				if (i == 10)
				{
					color = Scalar(0,255,0);
					bestMatch_ref = 2.0;
					AR_final_ref = .4;
					area_final_ref = .7;
				}
				if (i == 4)
				{
					color = Scalar(255,0,255);
					bestMatch_ref = .3;
					AR_final_ref = .2;
					area_final_ref = .1;
				}

				if (i == 5)
				{
					color = Scalar(0,255,255);
					bestMatch_ref = 3.0;
					AR_final_ref = .3;
					area_final_ref = .3;
				}

				if (i == 0)
				{
					color = Scalar(0,0,255);
					bestMatch_ref = 2.0;
					AR_final_ref = .7;
					area_final_ref = .7;
				}
/*
				if (abs(perimeter_final) < .2)
				{		
					Scalar color = Scalar(0,0,255);
					//drawContours( img, contours, idx, color, 1, 8, vector<Vec4i>(), 0, Point() );
					drawContours( img, hull, idx, color, 15, 8, vector<Vec4i>(), 0, Point() );
		  		}
				if (abs(AL_final) < 0.1)
				{		
					Scalar color = Scalar(0,255,0);
					//Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
					drawContours( img, contours, idx, color, 10, 8, vector<Vec4i>(), 0, Point() );
					//drawContours( img, hull, idx, color, 10, 8, vector<Vec4i>(), 0, Point() );
		  		}
				if (area_final < 0.2)
				{		
					Scalar color = Scalar(255,0,0);
					//Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
					drawContours( img, contours, idx, color, 5, 8, vector<Vec4i>(), 0, Point() );
					//drawContours( img, hull, idx, color, 10, 8, vector<Vec4i>(), 0, Point() );
		  		}
				if (AR_final < 0.2)
				{		
					Scalar color = Scalar(255,255,255);
					//Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
					drawContours( img, contours, idx, color, 3, 8, vector<Vec4i>(), 0, Point() );
					//drawContours( img, hull, idx, color, 10, 8, vector<Vec4i>(), 0, Point() );
		  		}
*/
				if (bestMatch < bestMatch_ref && AR_final< AR_final_ref && area_final<area_final_ref )
				{		
					printf("bestmatch = %f, ref = %f, AR = %f, %f, area = %f, %f \n",bestMatch,bestMatch_ref, AR_final,AR_final_ref, area_final, area_final_ref);
					//Scalar color = Scalar(255,0,255);
					//Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
					drawContours( img, contours, idx, color, 1, 8, vector<Vec4i>(), 0, Point() );
					//drawContours( img, hull, idx, color, 10, 8, vector<Vec4i>(), 0, Point() );
		  		}
			}

		};
	  };
	};

int j= 0;
	for (unsigned int i = 0; i < ref_contours.size(); i++) {
		j = i;
		if (ref_contours[i].size() > 100)
		{
	
			Scalar color = Scalar(255,255,255);
			if (j == 11)
			{
				color = Scalar(255,0,0);
			}
			else if (j == 10)
			{
				color = Scalar(0,255,0);
			}
			else if (j == 4)
			{
				color = Scalar(255,0,255);
			}
			else if (j == 5)
			{
				color = Scalar(0,255,255);
			}
			else if (j == 0)
			{
				color = Scalar(0,0,255);
			}
			//Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
			drawContours( ref_img, ref_contours, i, color, 1, 8, vector<Vec4i>(), 0, Point() );
		}
	}

   /// Show in a window
   namedWindow( "Hull demo", CV_WINDOW_AUTOSIZE );
   imshow( "Hull demo", img);

   namedWindow( "R", CV_WINDOW_AUTOSIZE );
   imshow( "R", ref_img);


		cvtColor(erosion_dst,img,CV_GRAY2RGB);

		return img;
	
}


}//end namespace vision
}//end namespace RAM


