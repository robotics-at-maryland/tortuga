#include <iostream>
#include <sstream>
#include <math.h>
#include <cstdlib>
#include <unistd.h>
#include "cv.h"
#include "highgui.h"
#include <string>
#include "vision/include/main.h"
#include "vision/include/ProcessList.h"
#include "vision/include/VisionCommunication.h"

/* 
	Daniel Hakim
	VISION CODE!!! 
	DO NOT TOUCH.
*/
using namespace std;
using namespace ram::vision;


//Light constants
#define MINFRAMESON 3
#define MINFRAMESOFF 3
#define MAXFRAMESON 7
#define MAXFRAMESOFF 7

extern "C"{
  int giveMeFive();
}
int giveMeFive(){
  return 5;
}

static int goVision=1;
extern "C"{
  void killVision();
}
void killVision(){
  goVision=0;
}

int gateDetect(IplImage* percents, IplImage* base, int* gatex, int* gatey)
{
	char* data=percents->imageData;
	char* data2=base->imageData;
	int width=percents->width;
	int height=percents->height;

	int* columnCounts=(int*) calloc(sizeof(int),width);
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
					data2[count]=255;
					data2[count+1]=255;
					data2[count+2]=255;
					whitex+=x;
					whitey+=y;
					total++;
					minx=min(x,minx);
					maxx=max(x,maxx);
					miny=min(y,miny);
					maxy=max(y,maxy);
					++columnCounts[x];
				}
				else
					data2[count]=data2[count+1]=data2[count+2]=0;
			}
			else
				data2[count]=data2[count+1]=data2[count+2]=0;
			count+=3;
		}
	}
		whitex/=total;
		whitey/=total;
		
		if (total>500)
		{
			int indexR;
			int indexL;
			*gatex=whitex;
			*gatey=whitey;
			int testCol=columnCounts[whitex];
			int state=0;
			for (indexR=whitex;indexR<width;indexR++)
			{
				if (columnCounts[indexR]>2*testCol)
					state++;
				else
					state=0;
				if (state==5)
					break;
			}
			if (state==5)
			{
				for (indexL=whitex;indexL>0;indexL--)
				{
					if (columnCounts[indexL]>2*testCol)
						state++;
					else
						state=0;
					if (state==5)
						break;
				}
			}
			if (state!=5)
			{
				//No gate here
				whitex=whitey=-1;
			}
			else
			{
				count=3*indexL+3*width*whitey;
				for (int x=indexL; x<indexR;x++)
				{
					data2[count]=data2[count+2]=0;
					data2[count+1]=255;
					count+=3;
				}
			}
		}
	free(columnCounts);
	return whitex!=-1;
	}

	int runVision(int argc, char** argv)
	{
		cvNamedWindow("test",CV_WINDOW_AUTOSIZE);
		ProcessList* pl=new ProcessList();
		pl->addStep("show");
		pl->addStep("white_detect");
		pl->addStep("show");
		pl->addStep("reset");
		pl->addStep("mask_red");
		pl->addStep("show");
		pl->addStep("reset");
		pl->addStep("mask_orange");
		pl->addStep("show");

		if ((pl->toCall.back())!="end")
			pl->toCall.push_back("end");

//		walk(img,pl);
		run(pl);
		cvWaitKey(0);
		delete pl;
	}
	
	//subtract two images, display the result
	void diff(IplImage* img, IplImage* oldImg, IplImage* destination)
	{
		char* data=img->imageData;
		char* data2=oldImg->imageData;
		char* dest=destination->imageData;
		int width=img->width;
		
		if (width!=oldImg->width || width!=destination->width)
		{
			//cout<<"Error, width changed"<<endl;
			return;
		}
		int height=img->height;
		if (height!=oldImg->height || height!=destination->height)
		{
			//cout<<"error, height changed"<<endl;
			return;
		}
		
		int	count=0;
		int r;
		int g;
		int b;
		int r2;
		int g2;
		int b2;
		for (int y=0; y<height; y++)
			for (int x=0; x<width; x++)
			{
				b=(data[count]+256)%256;
				g=(data[count+1]+256)%256;
				r=(data[count+2]+256)%256;
				b2=(data2[count]+256)%256;
				g2=(data2[count+1]+256)%256;
				r2=(data2[count+2]+256)%256;
				dest[count]=abs(b-b2);
				dest[count+1]=abs(g-g2);
				dest[count+2]=abs(r-r2);
//				dest[count]=data[count]-data2[count];
//				dest[count+1]=data[count+1]-data2[count+1];
//				dest[count+2]=data[count+2]-data2[count+2];
				count+=3;
			}
		count=0;
		int numPerEight=0;
			for (int x=0;x<width*height/8;x++)
			{
				numPerEight=0;
				for (int z=0; z<8;z++)
				{
					if (dest[count]>5 && dest[count+1]>5 && dest[count+2]>5)
					{
						numPerEight++;
					}
					count+=3;
				}
				if (numPerEight>3)
				
					dest[count-24]=dest[count-23]=dest[count-22]=dest[count-21]=dest[count-20]=dest[count-19]=
					dest[count-18]=dest[count-17]=dest[count-16]=dest[count-15]=dest[count-14]=dest[count-13]=
					dest[count-12]=dest[count-11]=dest[count-10]=dest[count-9] =dest[count-8] =dest[count-7]=
					dest[count-6] =dest[count-5] =dest[count-4 ]=dest[count-3] =dest[count-2] =dest[count-1]=255;
				else
					dest[count-24]=dest[count-23]=dest[count-22]=dest[count-21]=dest[count-20]=dest[count-19]=
					dest[count-18]=dest[count-17]=dest[count-16]=dest[count-15]=dest[count-14]=dest[count-13]=
					dest[count-12]=dest[count-11]=dest[count-10]=dest[count-9] =dest[count-8] =dest[count-7]=
					dest[count-6] =dest[count-5] =dest[count-4 ]=dest[count-3] =dest[count-2] =dest[count-1]=0;
			}
	}
	
	//cvHough
	double hough(IplImage* img)
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
//		cvShowImage("Hough",dst);

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
			pt1.x = cvRound(x0 + 4000*(-b));
			pt1.y = cvRound(y0 + 4000*(a));
			pt2.x = cvRound(x0 - 4000*(-b));
			pt2.y = cvRound(y0 - 4000*(a));
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
					//cout<<"Missed++"<<endl;
			}
			else {
				if(line[0].y < line[1].y) {
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
			
			cvLine( color_dst, line[0], line[1], CV_RGB(255,0,0), 3, CV_AA, 0 );
		}
		int total = lines->total - missed;
		if (total>0)
		{
			start.x/=total;
			start.y/=total;
			end.x/=total;
			end.y/=total;
		
			int xdiff = (end.x - start.x);		//deal with y's being flipped
			int ydiff = -1*(end.y - start.y);
			angle = atan2((ydiff),(xdiff));
			//cout<<"Xdiff: "<<xdiff<<endl;
			//cout<<"Ydiff: "<<ydiff<<endl;
			//cout<<"Angle: "<<angle<<endl; 
			//cout<<"startx: "<<start.x<<" endx: "<<end.x<<endl;
		
			cvLine(color_dst,start,end,CV_RGB(255,0,255), 3, CV_AA, 0);
		}
	#endif
	return angle;

//		cvShowImage( "Hough", color_dst );
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

#define OUTSIZE 5
typedef struct
{
	int x;
	int y;
}Pos;
typedef std::list<Pos> PosList;

void explore(IplImage* img, int x, int y, int* out, int color)
{	
	PosList toExplore;
	int width=img->width;
	int height=img->height;
	unsigned char* data=(unsigned char*)img->imageData;
	out[0]=0;
	out[1]=999999;
	out[2]=999999;
	out[3]=-999999;
	out[4]=-999999;
	
	Pos start,temp;
	start.x=x;
	start.y=y;
	toExplore.push_back(start);
	while (!(toExplore.empty()))
	{
		Pos &p=toExplore.back();
		x=p.x;
		y=p.x;
		toExplore.pop_back();
		int count=3*x+3*width*y;
		if ((data[count]>100 && data[count+1]>100 && data[count+2]>100))
		{
			out[0]++;
			out[1]=min(out[1],x);
			out[2]=min(out[2],y);
			out[3]=max(out[3],x);
			out[4]=max(out[4],y);
			
			data[count]=0;
			data[count+1]=0;
			data[count+2]=color;
			
			if (x>0)
			{
				temp.x=x-1;
				temp.y=y;
				toExplore.push_back(temp);
			}
			if (x<width-1)
			{
				temp.x=x+1;
				temp.y=y;
				toExplore.push_back(temp);
			}
			if (y>0)
			{
				temp.x=x;
				temp.y=y-1;
				toExplore.push_back(temp);
			}
			if (y<height-1)
			{
				temp.x=x;
				temp.y=y+1;
				toExplore.push_back(temp);
			}
		}
	}
}
CvPoint find_flash(IplImage* img, bool display)
{
	int width=img->width;
	int height=img->height;
	unsigned char* data=(unsigned char*)img->imageData;
	CvPoint center;
	int out[5];
	int count=0;
	int color=175;
	for (int y=0; y<height; y++)
		for (int x=0; x<width;x++)
		{
			
			out[0]=0;
			out[1]=999999;
			out[2]=-999999;
			out[3]=999999;
			out[4]=-999999;
			

			if (data[count]>0&&data[count+1]>0&&data[count+2]>0)
			{
				explore(img,x,y,out,color);
				color+=20;
				CvPoint bl,br,tl,tr;
				tl.x=bl.x=out[1];
				tr.x=br.x=out[3];
				tl.y=tr.y=out[4];
				bl.y=br.y=out[2];
				int w=out[3]-out[1];
				int h=out[4]-out[2];
				center.x=(out[3]+out[1])/2;
				center.y=(out[4]+out[2])/2;
				
				if (display && out[0]>25)
				{
					//cout<<"Data from explore"<<endl<<"Count: "<<out[0]<<"min x: "<<out[1]<<"min y: "<<out[2]<<"max x: "<<out[3]<<"max y: "<<out[4]<<endl; 
				}
				if (out[0]>25 && w>5 && w<20 && h>5 && h<20)
				{

					cvLine(img, tl, tr, CV_RGB(0,0,255), 3, CV_AA, 0 );
					cvLine(img, tl, bl, CV_RGB(0,0,255), 3, CV_AA, 0 );
					cvLine(img, tr, br, CV_RGB(0,0,255), 3, CV_AA, 0 );
					cvLine(img, bl, br, CV_RGB(0,0,255), 3, CV_AA, 0 );
					return center;
				}
				
			}
			
			count+=3;
		}
	//Return a 0,0 point if not found
	center.x=0;
	center.y=0;
	return center;
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
	//cout<<endl<<goRight<<endl;
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
				//cout<<"WTF?!?!?!?!?!"<<g <<endl;
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
	char a='0';
	//cout<<"Select Mask Description"<<endl;
	//cout<<"0: Strict Orange"<<endl;
	//cout<<"1: Lenient Orange"<<endl;
	//cout<<"More coming someday... one might hope"<<endl;
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
	//cout<<sum_of_abs<<endl;
	return sum_of_abs;
}

int angle_from_center(int argxs[], IplImage* img) {
	double startx,starty,endx,endy;
	int height = img->height;
	starty = 0;
	startx = argxs[(int)starty];
	endy = height/2;
	endx = argxs[(int)endy];
	//cout<<atan((endx-endy)/(endy-starty));
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

	//cout<<"Color of water:" <<r << " "<<g<<" "<< b<<endl;
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

int white_detect(IplImage* percents, IplImage* base, int* binx, int* biny)
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
		//cout<<"White Center:"<<whitex<<","<<whitey<<endl;
		//cout<<"Black Center:"<<blackx<<","<<blacky<<endl;
		//cout<<distance<<endl;
		
		if (distance<30)
		{
			//cout<<"We've almost certainly found the bin!!  DROP THAT MARKER!!! WOOHOOHOOOHOO!!!!!!"<<endl;
			*binx=(whitex+blackx)/2;
			*biny=(whitey+blacky)/2;
		}
		else
		{
			*binx=-1;
			*biny=-1;
		}
	}
	return min(total,total2);
}

extern "C"{
  VisionCommunication* getCommunicator();
}

extern VisionCommunication* vc;
VisionCommunication* getCommunicator()
{
	return vc;
}

extern "C" {
  int visionStart();
}
int visionStart()
{
  goVision=1;
//  CvCapture* camCapture=cvCaptureFromFile("underwater.avi");
	
	VisionData  duplicateMe;
	VisionData *buffer1,*buffer2;
	
	buffer1=new VisionData();
	buffer2=new VisionData();
	vc->safe=&buffer1;
	
	int swapper=2;	
	
	CvCapture* camCapture=cvCaptureFromCAM(0);
	//cvNamedWindow("After_Analysis", CV_WINDOW_AUTOSIZE );
	cvNamedWindow("Before_Analysis", CV_WINDOW_AUTOSIZE );
	//cvNamedWindow("Hough", CV_WINDOW_AUTOSIZE );
	//cvNamedWindow("Bin_go", CV_WINDOW_AUTOSIZE);
	//cvNamedWindow("Flash", CV_WINDOW_AUTOSIZE);
	//cvNamedWindow("Movement", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Gate",CV_WINDOW_AUTOSIZE);
	IplImage* unscaledFrame=NULL;
	IplImage* frame=NULL;
	IplImage* starterFrame=NULL;
	IplImage* houghFrame=NULL;
	IplImage* analysis=NULL;
	IplImage* binFrame=NULL;
	IplImage* flashFrame=NULL;
	IplImage* oldFrame=NULL;
	IplImage* moveFrame=NULL;
	IplImage* gateFrame=NULL;
	CvPoint lightCenter;
	lightCenter.x=0;
	lightCenter.y=0;
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
	bool show_flashing=true;
	bool startCounting=false;
	bool show_movement=true;
	bool show_gate=true;
	long frame_count=0;
	int lightFramesOn=0;
	int lightFramesOff=0;
	int speed=1;
	int blinks=0;
	int spooky=0;
	bool found=false;

	int okay=cvGrabFrame(camCapture);
	frame=cvCreateImage(cvSize(200,200),8,3);
	unscaledFrame=cvRetrieveFrame(camCapture);
	cvResize(unscaledFrame,frame);

	starterFrame=cvCreateImage(cvGetSize(frame),8,3);	
	analysis=cvCreateImage(cvGetSize(frame),8,3);
	binFrame=cvCreateImage(cvGetSize(frame),8,3);
	gateFrame=cvCreateImage(cvGetSize(frame),8,3);
	houghFrame=cvCreateImage(cvGetSize(frame),8,3);
	flashFrame=cvCreateImage(cvGetSize(frame),8,3);
	oldFrame=cvCreateImage(cvGetSize(frame),8,3);
	moveFrame=cvCreateImage(cvGetSize(frame),8,3);
	while(goVision)
	{
	  
	    key=cvWaitKey(25);
	  /*
		  //Start of input checking
		if (key=='a')
		{
			show_gate=!show_gate;
			cout<<key<<"Gate detection:"<<show_gate<<endl;
		}
		if (key=='q')
		{
			cout<<key<<" Goodbye."<<endl;
			return 0;
		}
		if (key=='u')
		{
			show_flashing=!show_flashing;
			cout<<key<<" Showing flashing stuff:"<<show_flashing<<endl;
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
		*/

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
			//cvShowImage("After_Analysis",analysis);
		}
				
		if (!paused)
		  {
		    if (swapper==1)
		      buffer1->frameNumCheck=buffer2->frameNumCheck+1;
		    else //swapper==2
		      buffer2->frameNumCheck=buffer1->frameNumCheck+1;

		    if (swapper==1)
		      buffer1->frameNum=buffer2->frameNum+1;
		    else //swapper==2
		      buffer2->frameNum=buffer1->frameNum+1;
		    
		    for (int x=0; x<speed; x++)
		      {
			int okay=cvGrabFrame(camCapture);
			unscaledFrame=cvRetrieveFrame(camCapture);
			cvResize(unscaledFrame,frame);
		      }
		    
		    if (swapper==1)
		      {
			buffer1->width=cvGetSize(frame).width;
			buffer1->height=cvGetSize(frame).height;
		      }
		    else// if(swapper==2)
		      {
			buffer2->width=cvGetSize(frame).width;
			buffer2->height=cvGetSize(frame).height;
		      }
		    frame_count+=speed;
		    
		    cvCopyImage(starterFrame,oldFrame);//Put old frame into oldFrame
		    cvCopyImage(frame,starterFrame);//Put new frame into starterFrame
		    
		    cvCopyImage(frame,binFrame);
		    cvCopyImage(frame,gateFrame);
			cvShowImage("Before_Analysis", starterFrame);
		    
		    if (ratios_on)
			{
				int binCount=0;
				to_ratios(frame);

				if (show_gate)
				{
					int gatex;
					int gatey;
					bool gateFound=gateDetect(frame,gateFrame,&gatex,&gatey);
					if (gateFound==true)
						cout<<"Gate Found!\n"<<gatex<<" "<<gatey<<endl;
						else
						cout<<"No gate"<<endl;
					cvShowImage("Gate",gateFrame);
				}


			if (found==true || frame_count>0)
			  {
			    int binX=0;
			    int binY=0;
			    binCount=white_detect(frame,binFrame,&binX,&binY);
			    if (swapper==1)
			      {
				buffer1->binx=binX;
				buffer1->biny=binY;
			      }
			    else //swapper==2
			      {
				buffer2->binx=binX;
				buffer2->biny=binY;
			      }
			  }
			int pipe_count=red_blue(frame,2.0);
			//cout<<pipe_count<<endl;
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
			    //cout<<"WE FOUND THE BIN!!! DROP THE MARKER!!!"<<endl;
			    if (swapper==1)
			      buffer1->binVisible=1;
			    else //swapper==2
			      buffer2->binVisible=1;
			  }
			else
			  {
			    if (swapper==1)
			      buffer1->binVisible=0;
			    else //swapper==2
			      buffer2->binVisible=0;
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
				    //    cout<<orange_count<<endl;
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
				if ((left_or_right=guess_line(frame))>20){}
				  //cout<<"go right"<<endl;
				else if (left_or_right<-20){}
				  //cout<<"go left"<<endl;
				
				if (orange_count<250)
				  found=0;
				
				if (swapper==1)
				  buffer1->distFromVertical=left_or_right;
				else //swapper==2
				  buffer2->distFromVertical=left_or_right;
			      }
			    
			    if (swapper==1)
			      buffer1->pipeVisible=found;
			    else //swapper==2
			      buffer2->frameNum=found;
			  }
		      }
		    if (hough_on)
		      {
			cvCopyImage(frame,houghFrame);
			if (swapper==1)
			  buffer1->angle=hough(houghFrame);
			else
			  buffer2->angle=hough(houghFrame);
		      }	
		    
		    if (show_flashing)
		      {
			if (lightFramesOff>20)
			  {
			    //					cout<<"Its been 20 frames without seeing the light-like object, maybe it was just a reflection, im starting the count over"<<endl;
			    lightFramesOn=0;
			    lightFramesOff=0;
			    blinks=0;
			    spooky=0;
			    startCounting=false;
			  }
			if (lightFramesOn>20)
			  {
			    //					cout<<"Its been 20 frames of seeing the light-like object, its not flashing, guess its just something shiny"<<endl;
			    lightFramesOn=0;
			    lightFramesOff=0;
			    blinks=0;
			    spooky=0;
			    startCounting=false;
			  }
			if (spooky>5)
			  {
			    //					cout<<"Somethings wrong, its staying off for too short/long, or staying on for too short/long, and its happened a spooky number of times, its not the light."<<endl;
			    lightFramesOn=0;
			    lightFramesOff=0;
			    blinks=0;
			    spooky=0;
			    startCounting=false;
			  }					
			if (blinks>3)
			  {
			    //cout<<"This thing has blinked "<<blinks<<" times, WE FOUND THE LIGHT GUYS!!"<<endl;
			    if (swapper==1)
			      buffer1->lightVisible=true;
			    else //swapper==2
			      buffer2->lightVisible=true;
			    //paused=true;
			  }
			else
			  {
			    if (swapper==1)
			      buffer1->lightVisible=false;
			    else //swapper==2
			      buffer2->lightVisible=false;
			  }
			cvCopyImage(frame, flashFrame);
			CvPoint p=find_flash(flashFrame, show_flashing);
			if (p.x!=0 && p.y!=0)
			  {
			    if (swapper==1)
			      {
				buffer1->redLightx=p.x;
				buffer1->redLighty=p.y;
			      }
			    else//swapper==2
			      {
				buffer2->redLightx=p.x;
				buffer2->redLighty=p.y;
			      }	
			    if (lightCenter.x==0 && lightCenter.y==0)
			      {
				//	cout<<"I see a light-like object"<<endl;
				startCounting=true;
			      }
			    else {
			      //						if (lightCenter.x<p.x)
			      //							cout<<"Its moving left"<<endl;
			      //						if (lightCenter.y<p.y)
			      //							cout<<"Its moving up"<<endl;
			      //						if (lightCenter.x>p.x)
			      //							cout<<"Its moving right"<<endl;
			      //						if (lightCenter.y>p.y)
			      //							cout<<"Its moving down"<<endl;
			      //						if (lightCenter.x==p.x && lightCenter.y==p.y)
			      //							cout<<"Its not moving... did someone put a flashing light on the sub or something... or are we stopped... uh oh..."<<endl;
			      
			    }
			    lightCenter.x=p.x;
			    lightCenter.y=p.y;
			    if (startCounting)
			      {
				if (lightFramesOff>0)
				  {
				    blinks++;
				    //  cout<<"The light has been off for "<<lightFramesOff<<" frames, now its coming back on"<<endl;
				  }
				
				if (lightFramesOff<MINFRAMESOFF || lightFramesOff>MAXFRAMESOFF)
				  {
				    spooky++;
				  }
				lightFramesOn++;
				lightFramesOff=0;
			      }
			  } 
			else
			  {
			    if (lightCenter.x!=0 && lightCenter.y!=0)
			      //cout<<"Light's out"<<endl;
			    
			    lightCenter.x=p.x;
			    lightCenter.y=p.y;
			    
			    if (startCounting)
			      {
				if (lightFramesOn>0)
				  {
				    //cout<<"The light has been on for "<<lightFramesOn<<" frames, now its gone"<<endl;
				  }
				
				if (lightFramesOn<MINFRAMESON || lightFramesOn>MAXFRAMESON)
				  {
				    spooky++;
				  }
				
				lightFramesOff++;
				lightFramesOn=0;
			      }
			    //	paused=true;
			  }
		      }
		    
		    if (center_line_on)
		      thin_blue_line(frame);
		    
		    if (show_movement)
		      diff(starterFrame,oldFrame,moveFrame);
		    
		    //cvShowImage("After_Analysis",frame);
		    //cvShowImage("Bin_go",binFrame);
		    //cvShowImage("Flash",flashFrame);
		    if (show_movement){}
		      //cvShowImage("Movement",moveFrame);
		    
		    
		  }
	if (swapper==1)
	  {
	    getCommunicator()->safe=&buffer1;
	    swapper=2;
	  }
	else //swapper==2
	  {
	    getCommunicator()->safe=&buffer2;
	    swapper=1;
	  }
	}
	
	return goVision;

}

void run (ProcessList *pl) {
	CvCapture* camCapture=cvCaptureFromFile("underwater.mov");

	//	CvCapture* camCapture=cvCaptureFromCAM(0);
	IplImage* unscaledFrame=NULL;
	IplImage* frame=NULL;
	IplImage* starterFrame=NULL;
	IplImage* houghFrame=NULL;
	IplImage* analysis=NULL;
	IplImage* binFrame=NULL;
	IplImage* flashFrame=NULL;
	IplImage* oldFrame=NULL;
	IplImage* moveFrame=NULL;
	IplImage* result=NULL;
	CvPoint lightCenter;
	lightCenter.x=0;
	lightCenter.y=0;
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
	bool show_flashing=true;
	bool startCounting=false;
	bool show_movement=true;
	long frame_count=0;
	int lightFramesOn=0;
	int lightFramesOff=0;
	int speed=1;
	int blinks=0;
	int spooky=0;
	bool found=false;

	int okay=cvGrabFrame(camCapture);
	frame=cvCreateImage(cvSize(200,200),8,3);
	unscaledFrame=cvRetrieveFrame(camCapture);
	cvResize(unscaledFrame,frame);

	starterFrame=cvCreateImage(cvGetSize(frame),8,3);	
	analysis=cvCreateImage(cvGetSize(frame),8,3);
	binFrame=cvCreateImage(cvGetSize(frame),8,3);
	houghFrame=cvCreateImage(cvGetSize(frame),8,3);
	flashFrame=cvCreateImage(cvGetSize(frame),8,3);
	result=cvCreateImage(cvGetSize(frame),8,3);
	moveFrame=cvCreateImage(cvGetSize(frame),8,3);
	int windowCount=0;
	for (StringListIterator i=pl->toCall.begin();*i!="end";i++)
	{
		if (*i=="show")
		{
			ostringstream os;
			os<<"Results"<<windowCount;
			string windowName=os.str();
			cvNamedWindow(windowName.c_str(), CV_WINDOW_AUTOSIZE );
			windowCount++;
		}
	}
	
	while (true)
	{
		key=cvWaitKey(25);
		int windowCount=0;
		if (!paused)
		{			
			for (int x=0; x<speed; x++)
				{
					int okay=cvGrabFrame(camCapture);
					unscaledFrame=cvRetrieveFrame(camCapture);
					cvResize(unscaledFrame,frame);
				}
			frame_count+=speed;
		}
	
		cvCopyImage(frame,result);	
		string lastStep="Start";
		for (StringListIterator i=pl->toCall.begin();*i!="end";lastStep=*i,i++)
		{
			if (*i=="show")
			{
			        ostringstream os;
				os<<"Results"<<windowCount;
				string windowName=os.str();
				cvShowImage(windowName.c_str(),result);
				windowCount++;
			}	
			else if (*i=="end")
				return;
 			else if (*i=="hough")
			{
				hough(result);
			}
			else if (*i=="reset")
			{
				cvCopyImage(frame,result);
			}
			else if (*i=="diff")
			{
				diff(result,frame,moveFrame);
				cvCopyImage(moveFrame,result);
			}
			else if (*i=="mask_red")
			{
				mask_red(result,true,10);
			}
			else if (*i=="find_flash")
			{
				CvPoint p=find_flash(result, show_flashing);
			} 
			else if (*i=="guess_line")
			{
				guess_line(result);
					int left_or_right;
				if ((left_or_right=guess_line(result))>20)
				{
					//cout<<"go right"<<endl;
				}
				else if (left_or_right<-20)
				{
					//cout<<"go left"<<endl;
				}
				else
				{
					//cout<<"go straight"<<endl;
				}
			}	
			else if (*i=="mask_orange")
			{
				int orange_count=mask_orange(result,1,0);
			}
			else if (*i=="mask_with_input")
			{
				mask_with_input(result);
			}
			else if (*i=="angle_from_center")
			{
			}
			else if (*i== "correct")
			{
				correct(result);
			}
			else if (*i=="filter")
			{	
				filter(result,red_flag,green_flag,blue_flag);
			}
			else if (*i=="to_ratios")
			{
				to_ratios(result);
			}
			else if (*i=="red_blue")
			{
				int pipe_count=red_blue(result,2.0);
			}
			else if (*i=="white_detect")
			{
			  int ignoreBinx,ignoreBiny;
				cvCopyImage(result,binFrame);
				to_ratios(binFrame);
				//cout<<white_detect(binFrame,result,&ignoreBinx,&ignoreBiny)<<endl;
			}
			else
			{}
				//cout<<"Unrecognized function"<<endl;
		}
	}
	return;
}

void walk(IplImage *img, ProcessList *pl) {

	IplImage* unscaledFrame=NULL;
	IplImage* frame=NULL;
	IplImage* starterFrame=NULL;
	IplImage* houghFrame=NULL;
	IplImage* analysis=NULL;
	IplImage* binFrame=NULL;
	IplImage* flashFrame=NULL;
	IplImage* oldFrame=NULL;
	IplImage* moveFrame=NULL;
	IplImage* result=img;
	CvPoint lightCenter;
	lightCenter.x=0;
	lightCenter.y=0;
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
	bool show_flashing=true;
	bool startCounting=false;
	bool show_movement=true;
	long frame_count=0;
	int lightFramesOn=0;
	int lightFramesOff=0;
	int speed=1;
	int blinks=0;
	int spooky=0;
	bool found=false;

	int windowCount=0;
	for (StringListIterator i=pl->toCall.begin();*i!="end";i++)
	{
		if (*i=="show")
		{
			ostringstream os;
			os<<"Results"<<windowCount;
			string windowName=os.str();
			cvNamedWindow(windowName.c_str(), CV_WINDOW_AUTOSIZE );
			windowCount++;
		}
	}

	windowCount=0;

	string lastStep="Start";
	for (StringListIterator i=pl->toCall.begin();*i!="end";lastStep=*i,i++)
	{
		if (*i=="show")
		{
			ostringstream os;
			os<<"Results"<<windowCount;
			string windowName=os.str();
			cvShowImage(windowName.c_str(),img);
			windowCount++;
		}	
		else if (*i=="end")
			return;
		else if (*i=="hough")
		{
			hough(img);
		}
		else if (*i=="reset")
		{
			cvCopyImage(frame,img);
		}
		else if (*i=="diff")
		{
			//cout<<"No prev image, dumbass"<<endl;
		}
		else if (*i=="mask_red")
		{
			mask_red(img,true,10);
		}
		else if (*i=="find_flash")
		{
			CvPoint p=find_flash(img, show_flashing);
		} 
		else if (*i=="guess_line")
		{
			guess_line(img);
				int left_or_right;
			if ((left_or_right=guess_line(img))>20)
			{
					//cout<<"go right"<<endl;
			}
			else if (left_or_right<-20)
			{
				//cout<<"go left"<<endl;
			}
			else
			{
			//cout<<"go straight"<<endl;
			}
		}
		else if (*i=="mask_orange")
		{
			int orange_count=mask_orange(img,1,0);
		}
		else if (*i=="mask_with_input")
		{
			mask_with_input(img);
		}
		else if (*i=="angle_from_center")
		{
		}
		else if (*i== "correct")
		{
			correct(img);
		}
		else if (*i=="filter")
		{	
			filter(img,red_flag,green_flag,blue_flag);
		}
		else if (*i=="to_ratios")
		{
			to_ratios(img);
		}
		else if (*i=="red_blue")
		{
			int pipe_count=red_blue(img,2.0);
		}
		else if (*i=="white_detect")
		{
		  int binx, biny;
			cvCopyImage(img,binFrame);
			to_ratios(binFrame);
			//cout<<white_detect(binFrame,img,&binx,&biny)<<endl;
		}
		else
		{
			//cout<<"Unrecognized function"<<endl;
		}
	}
	return;
}

