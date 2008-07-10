//Michael Levashov
//Algorithm for the sonar fft code
//05/13/08

#include "fft_sonar.h"
//#include <iostream>

using namespace std;

fft_sonar::fft_sonar(int p_num, int length)
{
	ping_num=p_num;
	size=length;
	transforms=new double[size*ping_num];
}

fft_sonar::~fft_sonar()
{
	delete transforms;
}

//Stores the data and takes the fourier transforms for all pingers
int fft_sonar::update_sample(int** data)
{
	//Copy the data over
	for(k=0; k<ping_num*size; k++)
		transforms[k]=(double) data[k/size][k%size];

	//Do a fourier transform on the data
	for(k=0; k<ping_num; k++)
		if(gsl_fft_real_radix2_transform(&transforms[k*size], 1, size) != GSL_SUCCESS)
			return -1; //Failed doing a fourier transform

	return 0;
}

//Actually multiplies the two signals and transforms them back
int fft_sonar::convolve(int pinger1, int pinger2, double *result)
{
	//This is done because of the way that gsl stores the data
	result[0]=transforms[size*pinger1]*transforms[size*pinger2];
	for(int j=1; j<size/2; j++)
	{
		result[j]=transforms[size*pinger1+j]*transforms[size*pinger2+j]+transforms[size*pinger1+size-j]*transforms[size*pinger2+size-j];
		//result[size-j]=data1[size-j]*data2[j]-data2[size-j]*data1[j];
		result[size-j]=transforms[size*pinger1+size-j]*transforms[size*pinger2+j]-transforms[size*pinger2+size-j]*transforms[size*pinger1+j];
	}
	result[size/2]=transforms[size*pinger1+size/2]*transforms[size*pinger2+size/2];

	if(gsl_fft_halfcomplex_radix2_backward(result,1,size) == GSL_SUCCESS)
		return 0;
	else
		return -1;
}
