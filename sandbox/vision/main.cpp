#include <iostream>
#include <math.h>
#include <cstdlib>
#include <unistd.h>
#include "cv.h"
#include "highgui.h"
#include <string.h>

using namespace std;

int distance_from_line(int avgxs[], IplImage* img);
int angle_from_center(int argxs[], IplImage* img);
void hough(IplImage* img);



	//cvHough
	void hough(IplImage* img)
	{
	    IplImage* color_dst =img;//cvLoadImage("DSC00099.jpg", 0 );
		IplImage* src = cvCreateImage(cvGetSize(img), 8, 1);
		cvCvtColor(img,src,CV_BGR2GRAY);

		IplImage* dst;

		CvMemStorage* storage = cvCreateMemStorage(0);
		CvSeq* lines = 0;
		int i;
	
		//if( !src )
		//	return;
		
		dst = cvCreateImage( cvGetSize(src), 8, 1 );
//		color_dst = cvCreateImage( cvGetSize(src), 8, 3 );

//		IplConvKernel* element = cvCreateStructuringElementEx(2*2+1, 2*2+1, 2, 2, CV_SHAPE_RECT, 0 );
//		cvDilate(dst,dst,element);
//		cvCvtColor( dst, color_dst, CV_GRAY2BGR );	
		cvCanny( src, dst, 50, 200, 3 );
//		cvShowImage("Testing",dst);

	#if 0
		lines = cvHoughLines2( dst, storage, CV_HOUGH_STANDARD, 1, CV_PI/180, 10, 0, 0 );

		for( i = 0; i < MIN(lines->total,100); i++ )
		{
			float* line = (float*)cvGetSeqElem(lines,i);
			float rho = line[0];
			float theta = line[1];
			CvPoint pt1, pt2;
			double a = cos(theta), b = sin(theta);
			double x0 = a*rho, y0 = b*rho;
			pt1.x = cvRound(x0 + 1000*(-b));
			pt1.y = cvRound(y0 + 1000*(a));
			pt2.x = cvRound(x0 - 1000*(-b));
			pt2.y = cvRound(y0 - 1000*(a));
			cvLine( color_dst, pt1, pt2, CV_RGB(255,0,0), 3, CV_AA, 0 );
		}
	#else
		lines = cvHoughLines2( dst, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI/180, 10, 50, 10 );
		CvPoint start,end;
		
		start.x=start.y=end.x=end.y=0;
		int missed = 0;
		int temp;
		double angle;
		for( i = 0; i < lines->total; i++ )
		{
			CvPoint* line = (CvPoint*)cvGetSeqElem(lines,i);
			if (((line[0].x < 5)&&(line[1].x < 5)) || ((line[0].x > color_dst->width-5)&&(line[1].x > color_dst->width-5)) 
				|| ((line[0].y < 5)&&(line[1].y < 5)) || ((line[0].y > color_dst->height-5)&&(line[1].y > color_dst->height-5))) {
					missed++;
					cout<<"Missed++"<<endl;
			}
			else {
				if(line[1].y < line[0].y) {
						temp = line[0].y;
						line[0].y = line[1].y;
						line[1].y = temp;
						temp = line[0].x;
						line[0].x = line[1].x;
						line[1].x = temp;
				}
				start.x += line[0].x;
				start.y += line[0].y;
				end.x += line[1].x;
				end.y += line[1].y;
			}
			
//			cvLine( color_dst, line[0], line[1], CV_RGB(255,0,0), 3, CV_AA, 0 );
			
		}
		int total = lines->total - missed;
		start.x/=total;
		start.y/=total;
		end.x/=total;
		end.y/=total;
		
		angle = atan2((end.y - start.y),(end.x - start.x));
		cout<<"Angle: "<<angle<<endl; 
		
		cvLine(color_dst,start,end,CV_RGB(255,0,255), 3, CV_AA, 0);
	#endif
		cvShowImage( "Testing", color_dst );
	}


//Only meant for use on images after correct has been called on them.
int mask_red(IplImage* img, bool alter_img, int threshold)
{
	char* data=img->imageData;
	int width=img->width;
	int height=img->height;
	int count=0;
	int pixel_count=0;
	int r=0;
	int g=0;
	int b=0;

	for (int y=0; y<height; y++)
		for (int x=0; x<width; x++)
		{
			b=(data[count]+256)%256;
			g=(data[count+1]+256)%256;
			r=(data[count+2]+256)%256;
			
			if (r>g && r>b && r>threshold)
			{
				pixel_count++;
				if (alter_img)
				{
					data[count]=255;
					data[count+1]=255;
					data[count+2]=255;
				}
			}
			else
			{
				if (alter_img)
				{
					data[count]=0;
					data[count+1]=0;
					data[count+2]=0;
				}
			}
			
			count+=3;
		}
	return pixel_count;
}

int guess_line(IplImage* img)
{
	char* data=img->imageData;
	int width=img->width;
	int height=img->height;

	int avgxs[height];
	int r=0;
	int g=0;
	int b=0;
	int count=0;
	int minx=0;
	int maxx=0;
	int miny=0;
	int avgx=0;
	int countOfx=0;
	int temp;
	int goRight=0;
	bool on=false;
	for (int y=0; y<height; y++)
	{
		countOfx=0;
		int sumx=0;
		for (int x=0; x<width; x++)
		{
		
			b=(data[count]+256)%256;

			if (b==255 && on==false)
			{
				sumx+=x;
				countOfx++;
			}
			count+=3;
		}			
		avgx=sumx/countOfx;
		avgxs[y]=avgx;
	}	
	
	for (int j=0;j<5;j++) {
		for (int y=0;y<height-1;y++) {
			if ((avgxs[y] != 0) && (avgxs[y+1] != 0)) {
				temp = (avgxs[y] + avgxs[y+1]) / 2;
				avgxs[y] = temp;
				avgxs[y+1] = temp;
			}
		}
	}
	for (int y=0; y<height; y++)
	{
		avgx=avgxs[y];
		if (avgx==0)
			continue;
		goRight-=(width/2)-avgx;
		data[3*avgx+3*width*y]=0;
		data[3*avgx+3*width*y+1]=0;
	}
	
	distance_from_line(avgxs,img);
	angle_from_center(avgxs, img);
	goRight/=countOfx;
	cout<<endl<<goRight<<endl;
	return goRight;
}
		

int mask_orange(IplImage* img, bool alter_img, bool strict)
{
	char* data=img->imageData;
	int width=img->width;
	int height=img->height;
	int count=0;
	int pixel_count=0;
	int r=0;
	int g=0;
	int b=0;
	float r_over_g_min;
	float r_over_g_max;
	float b_over_r_max;
	
	if (!strict)
	{
		r_over_g_min=.6;
		r_over_g_max=.95;
		b_over_r_max=.5;
	}
	else
	{
		r_over_g_min=.75;
		r_over_g_max=.85;
		b_over_r_max=.4;
	}
	for (int y=0; y<height; y++)
		for (int x=0; x<width; x++)
		{
			b=(data[count]+256)%256;
			g=(data[count+1]+256)%256;
			r=(data[count+2]+256)%256;
			
			if (b<0 || g<0 || r<0 || b>255 ||g>255||r>255)
				cout<<"WTF?!?!?!?!?!"<<g <<endl;
			if (r>r_over_g_min*g && r_over_g_max*g>r && b_over_r_max*r > b)
			{
				if (alter_img)
					data[count]=data[count+1]=data[count+2]=255;
				pixel_count++;
			}
			else if (alter_img)
				data[count]=data[count+1]=data[count+2]=0;
			count+=3;
		}
		
	return pixel_count;
}

void mask_with_input(IplImage* img)
{
	const int ORANGE='0';
	char a=0;
	cout<<"Select Mask Description"<<endl;
	cout<<"0: Strict Orange"<<endl;
	cout<<"1: Lenient Orange"<<endl;
	cout<<"More coming someday... one might hope"<<endl;
	a=cvWaitKey(-1);
	
	if (a=='0')
	{
		mask_orange(img,true,true);
	}
	if (a=='1')
	{
		mask_orange(img,true,false);
	}
}

void thin_blue_line(IplImage* img)
{
	char* data=img->imageData;
	int width=img->width;
	int height=img->height;
	int half_width=width/2;
	
	for (int y=0; y<height; y++)
	{
		data[3*half_width+3*width*y]=255;
		data[3*half_width+3*width*y+1]=0;
		data[3*half_width+3*width*y+2]=0;
	}
}


int distance_from_line(int avgxs[], IplImage* img) {
	int height = img->height;
	int half_avgxs[height/2];
	int count = 0;
	int sum_of_abs = 0;
	for(int i=height/2;i<height;i++) {
		half_avgxs[count] = avgxs[i];
	}
	for(int i=0;i<height/2;i++) {
		sum_of_abs += abs(half_avgxs[i]);
	}
	cout<<sum_of_abs<<endl;
	return sum_of_abs;
}

int angle_from_center(int argxs[], IplImage* img) {
	double startx,starty,endx,endy;
	int height = img->height;
	starty = 0;
	startx = argxs[(int)starty];
	endy = height/2;
	endx = argxs[(int)endy];
	cout<<atan((endx-endy)/(endy-starty));
	return atan((endx-endy)/(endy-starty));
}
	
void correct(IplImage* img)
{
	char* data=img->imageData;
	int width=img->width;
	int height=img->height;
	
	srand ( time(NULL) );
	
	int red_sum=0;
	int green_sum=0;
	int blue_sum=0;
	int r;
	int g;
	int b;
	int total=0;
	
	for (int to_avg=0; to_avg<100;to_avg++)
	{
		int x = rand() % width;
		int y = rand() % height;
		int count=3*x+3*width*y;
	
		b=(data[count]+256)%256;
		g=(data[count+1]+256)%256;
		r=(data[count+2]+256)%256;
		
		if (b>r && g>r)//its a watery color
		{
			++total;
			blue_sum+=b;
			green_sum+=g;
			red_sum+=r;
		}
	}	
	
	r=red_sum/total;
	g=green_sum/total;
	b=blue_sum/total;

	g=g-r;
	b=b-r;	
	r=0;

	cout<<"Color of water:" <<r << " "<<g<<" "<< b<<endl;
	int r2;
	int g2;
	int b2;
	int	diffr;
	int diffg;
	int diffb;
	int count=0;

	
	for (int y=0; y<height; y++)
		for (int x=0; x<width; x++)
		{
			b2=(data[count]+256)%256;
			g2=(data[count+1]+256)%256;
			r2=(data[count+2]+256)%256;
			diffr=r2-r;
			diffg=g2-g;
			diffb=b2-b;
			
			//Remove Grays
			int max=MAX(MAX(diffr,diffg),diffb);
			int min=MIN(MIN(diffr,diffg),diffb);
			
			if (max-min < .250 * (max + min) )
			{
				data[count]=0;
				data[count+1]=0;
				data[count+2]=0;
			}
			else
			{
				if (diffb<=0)
					data[count]=0;
				else
					data[count]=diffb;
			
				if (diffg<=0)
					data[count+1]=0;
				else
					data[count+1]=diffg;
			
				if (diffr<=0)
					data[count+2]=0;
				else
					data[count+2]=diffr;
			}
			count+=3;
		}
}

void filter(IplImage* img, bool red_flag, bool green_flag, bool blue_flag)
{
	char* data=img->imageData;
	int width=img->width;
	int height=img->height;
	int count=0;
	
	for (int y=0; y<height; y++)
		for (int x=0; x<width; x++)
		{
			if (!blue_flag)
				data[count]=0;
			if (!green_flag)
				data[count+1]=0;
			if (!red_flag)
				data[count+2]=0;
			count+=3;
		}
	
}

void to_ratios(IplImage* img)
{
	char* data=img->imageData;
	int width=img->width;
	int height=img->height;
	int count=0;
	int pixel_count=0;
	int r=0;
	int g=0;
	int b=0;
	
	for (int y=0; y<height; y++)
		for (int x=0; x<width; x++)
		{
			b=(data[count]+256)%256;
			g=(data[count+1]+256)%256;
			r=(data[count+2]+256)%256;
			data[count]=(double)(b)/(b+g+r) *100;
			data[count+1]=(double)(g)/(b+g+r) *100;
			data[count+2]=(double)(r)/(b+g+r) *100;
			count+=3;
		}
}

int red_blue(IplImage* img, float ratio)
{
	char* data=img->imageData;
	int width=img->width;
	int height=img->height;
	int count=0;
	int pixel_count=0;
	int r=0;
	int g=0;
	int b=0;
	int total=0;
	for (int y=0; y<height; y++)
		for (int x=0; x<width; x++)
		{
			b=(data[count]+256)%256;
			g=(data[count+1]+256)%256;
			r=(data[count+2]+256)%256;
			for (float z=1.0;z>=.7;z-=.1)
				if (r>ratio*b*z)
				{
					data[count]=255*z;
					data[count+1]=255*z;
					data[count+2]=255*z;
					total+=10*z;
					break;
				}
			count+=3;
		}
		
	return total;
}

int white_detect(IplImage* percents, IplImage* base)
{
	char* data=percents->imageData;
	char* data2=base->imageData;
	int width=percents->width;
	int height=percents->height;
	int count=0;
	int pixel_count=0;
	int r=0;
	int g=0;
	int b=0;
	int r2=0;
	int g2=0;
	int b2=0;
	int total=0;
	int total2=0;
	
	int blackx=0;
	int blacky=0;
	int whitex=0;
	int whitey=0;
	
	int xdist=0;
	int ydist=0;
	int minx=999999;
	int maxx=0;
	int miny=999999;
	int maxy=0;
	for (int y=2; y<height-2; y++)
	{
		count=3*2+3*width*y;
		for (int x=2; x<width-2; x++)
		{
			b=(data[count]+256)%256;
			g=(data[count+1]+256)%256;
			r=(data[count+2]+256)%256;
			b2=(data2[count]+256)%256;
			g2=(data2[count+1]+256)%256;
			r2=(data2[count+2]+256)%256;
			if (b>25 && g>25 && r>25)
			{
				if (b2>100 && g2>100 && r2>100)
				{
					data2[count]=0;
					data2[count+1]=0;
					data2[count+2]=255;
					whitex+=x;
					whitey+=y;
					total++;
					minx=min(x,minx);
					maxx=max(x,maxx);
					miny=min(y,miny);
					maxy=max(y,maxy);
				}
//				else
//				{
//					data2[count]=0;
//					data2[count+1]=0;
//					data2[count+2]=0;
//				}
			}
//			else
//			{
//					data2[count]=0;
//					data2[count+1]=0;
//					data2[count+2]=0;				
//			}
			count+=3;
		}
	}	
	count=0;
	for (int y=miny;y<maxy;y++)
	{
		count=3*minx+3*width*y;
		for (int x=minx; x<maxx; x++)
		{
			b2=(data2[count]+256)%256;
			g2=(data2[count+1]+256)%256;
			r2=(data2[count+2]+256)%256;
			if (b2+g2+r2 <150)
			{
				data2[count]=255;
				data2[count+1]=0;
				data2[count+2]=0;
				blackx+=x;
				blacky+=y;
				total2++;
			}
			count+=3;
		}
	}
	
	if (total>0 && total2>0)
	{
		whitex/=total;
		whitey/=total;
		blackx/=total2;
		blacky/=total2;
	
		xdist=blackx-whitex;
		ydist=blacky-whitey;
	
	
		float distance=sqrt(xdist*xdist+ydist*ydist);
		cout<<"White Center:"<<whitex<<","<<whitey<<endl;
		cout<<"Black Center:"<<blackx<<","<<blacky<<endl;
		cout<<distance<<endl;
		
		if (distance<30)
		cout<<"We've almost certainly found the bin!!  DROP THAT MARKER!!! WOOHOOHOOOHOO!!!!!!"<<endl;
	}
	
	return min(total,total2);
}

int main (int argc, char * const argv[]) {

	CvCapture* camCapture=cvCaptureFromFile("underwater.mov");
//	CvCapture* camCapture=cvCaptureFromCAM(0);
	cvNamedWindow("After_Analysis", CV_WINDOW_AUTOSIZE );
	cvNamedWindow("Before_Analysis", CV_WINDOW_AUTOSIZE );
	cvNamedWindow("Testing", CV_WINDOW_AUTOSIZE );
	cvNamedWindow("Bin_go", CV_WINDOW_AUTOSIZE);
	IplImage* frame=NULL;
	IplImage* starterFrame=NULL;
	IplImage* houghFrame=NULL;
	IplImage* analysis=NULL;
	IplImage* binFrame=NULL;
	char key=0;
	bool paused=false;
	bool red_flag=true;
	bool green_flag=true;
	bool blue_flag=true;
	bool redraw=false;
	bool mask_on=false;
	bool filter_on=true;
	bool correct_on=false;
	bool orange_pipe_detect=true;
	bool center_line_on=true;
	bool ratios_on=true;
	bool hough_on=false;
	long frame_count=0;
	int speed=1;
	bool found=false;
	int okay=cvGrabFrame(camCapture);
	frame=cvRetrieveFrame(camCapture);
	starterFrame=cvCreateImage(cvGetSize(frame),8,3);
	analysis=cvCreateImage(cvGetSize(frame),8,3);
	binFrame=cvCreateImage(cvGetSize(frame),8,3);
	houghFrame=cvCreateImage(cvGetSize(frame),8,3);
	while(true)
	{
		key=cvWaitKey(25);
		//Start of input checking
		if (key=='q')
		{
			cout<<key<<" Goodbye."<<endl;
			return 0;
		}
		if (key=='+')
		{
			speed++;
		}
		if (key=='h')
		{
			hough_on=!hough_on;
			cout<<key<<"Hough transformation:"<<hough_on<<endl;
		}
		if (key=='-')
		{
			speed--;
		}
		if (key=='j')
		{
			orange_pipe_detect=!orange_pipe_detect;
			cout<<key<<" Orange_pipe_detection:"<<orange_pipe_detect<<endl;
		}
		if (key=='c')
		{
			correct_on=!correct_on;
		}
		if (key=='s')
		{
			speed=1;
		}
		if (key=='p')
		{
			paused=!paused;
			cout<<key<<" Pause:"<<paused<<endl;
		}
		if (key=='f')
		{
			filter_on=!filter_on;
			cout<<key<<" Filter:"<<filter_on<<endl;
			cout<<"Filtering flags are r, g, and b, to turn colors on and off"<<endl;
		}
		if (key=='r')
		{
			red_flag=!red_flag;
			cout<<key<<" Red:"<<red_flag<<endl;
		}
		if (key=='g')
		{
			green_flag=!green_flag;
			cout<<key<<" Green:"<<green_flag<<endl;
		}
		if (key=='b')
		{
			blue_flag=!blue_flag;
			cout<<key<<" Blue:"<<blue_flag<<endl;
		}
		if (key=='m')
		{
			mask_on=!mask_on;
			cout<<key<<" Masking:"<<mask_on<<endl;
		}
		if (key=='t')
		{
			ratios_on=!ratios_on;
			cout<<key<<" Ratios:"<<ratios_on<<endl;
		}
		if (key=='l')
		{
			center_line_on=!center_line_on;
			cout<<key<<" Center Line:"<<center_line_on<<endl;
		}
		if (key==' ')
		{
			redraw=true;
		}
		//End of input checking


		if (paused && redraw)
		{
			redraw=false;
			cvCopyImage(frame,analysis);
						
			if (correct_on)
			{
				correct(analysis);
				mask_red(analysis,true,10);
			}
			if (filter_on)
			{
				filter(analysis,red_flag,green_flag,blue_flag);
			}
			if (mask_on)
			{
				mask_with_input(analysis);
			}
			cvShowImage("After_Analysis",analysis);
		}
				
		if (!paused)
		{
//			if (found)
//			{
//				int okay=cvGrabFrame(camCapture);
//				frame=cvRetrieveFrame(camCapture);
//				frame_count+=1;
//			}
//			else
//			{
				for (int x=0; x<speed; x++)
				{
					int okay=cvGrabFrame(camCapture);
					frame=cvRetrieveFrame(camCapture);
				}
			frame_count+=speed;
//			}
			

			
			cvCopyImage(frame,starterFrame);

			cvCopyImage(frame,binFrame);
			cvShowImage("Before_Analysis", starterFrame);

			if (ratios_on)
			{
				int binCount=0;
				to_ratios(frame);
				if (found==true || frame_count>0)
				{
					binCount=white_detect(frame,binFrame);
				}
				int pipe_count=red_blue(frame,2.0);
				cout<<pipe_count<<endl;
				if (pipe_count>10000)
				{
					found=true;
					if (frame_count<0)
						frame_count=0;
				}
				else
				{
					found=false;
					frame_count-=speed*2;
				}
				if (binCount>500)
				{
					cout<<"WE FOUND THE BIN!!! DROP THE MARKER!!!"<<endl;
				}
			}
			else
			{
				if (orange_pipe_detect)
				{
					if (!found && frame_count>10)
					{
						frame_count=0;
						int orange_count=mask_orange(frame,0,true);
						if (orange_count>1000)
						{
							cout<<orange_count<<endl;
							found=1;
						}
						else
						{
							found=0;
						}
					}
					else if (found)
					{
						int orange_count=mask_orange(frame,1,0);
						int left_or_right;
						if ((left_or_right=guess_line(frame))>20)
							cout<<"go right"<<endl;
						else if (left_or_right<-20)
							cout<<"go left"<<endl;

						if (orange_count<250)
							found=0;

					}
				}
			}
			if (hough_on)
				{
					cvCopyImage(frame,houghFrame);
					hough(houghFrame);
				}	

			if (center_line_on)
				thin_blue_line(frame);
			
			cvShowImage("After_Analysis",frame);
			cvShowImage("Bin_go",binFrame);
		}
	}
    return 0;
}
