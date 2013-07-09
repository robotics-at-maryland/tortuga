/*
White balance filter
Input:Mat Image
Output: Mat Image that has been white balanced

Performs Minkowski Method for white balance
Doesn't require any thresholding or inputs

based off of matlab code by: ????
*/

#include <cv.h>
#include <highgui.h>
#include <stdio.h>
using namespace std;
using namespace cv;
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
//#include "ImageFilter.h"
#include "blobfinder.h"
#include "rectangle.h"

int main(int argc,char*argv[]);
string convertInt(int number);

int main(int argc, char* argv[])
{
	//want to take in argv which is a path and image name without hte numbers
	//start is the the first image to look at
	//end is the last image to look at
	int imagefirst = 1;
	int imagelast = 2;

	if (argc > 2)
	{
		imagefirst = atoi(argv[2]);
		imagelast = atoi(argv[3]);
	}
	else if (argc = 2)
	{
		imagefirst = 10;
		imagelast = 20;
	}
	if (imagefirst < 1)
		imagefirst = 1;
	if (imagelast < imagefirst)
		imagelast = imagefirst+1;

	string name_root = (string)argv[1];
	string number;
	string filename;
	string finalnumber;
	string zeros;	
	string filetype  =".png";
	blobfinder blobs;

	
	char buffer[3];


	for (int i=imagefirst; i<imagelast; i++)
	{	
		//convert i to strind
		number = convertInt(i);
		if ( i < 10)
		{	//add 3 zeros
			zeros = "000";
		}
		else if (i<100)
		//add 2 zeros
		{	
			zeros = "00";
		}
		filename = name_root+zeros+number+filetype;
	
		char* a = new char[filename.size()+1];
		a[filename.size()] = 0;
		memcpy(a,filename.c_str(),filename.size());
		printf("%s",a);	
		
		//blob finder
		//blobs.ImageFilter(a);

		//find gate/hedge
		rectangle(a);	
		//cvWaitKey(0);

		 
	}; //end for loop

	return 1;
};	

string convertInt(int number)
{
	stringstream ss;
	ss<< number;
        return ss.str();
};
