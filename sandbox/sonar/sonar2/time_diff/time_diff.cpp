//Michael Levashov
//time_diff calculates the difference in time steps between two signals
//It takes the following inputs:

//	signal1[] - array of voltage amplitudes
//	signal2[] - array of voltage amplitudes
//	nsize - size of the arrays (must be same for both)
//	sampfreq - sampling frequency for the sonar, in Hz
//	maxfreq - maximum frequency of the sampling
//	minfreq - minimum frequency of the sampling

#include <cmath>
#include <gsl/gsl_vector.h>
#include "sonar_quadfit.h"
#include "fft_sonar.h"
#include "time_diff.h"
#include <iostream>

using namespace std;

namespace ram {
namespace sonar {

time_diff::time_diff(int hydr_num, int ssize, int ssampfreq, int maxfreq, int minfreq)
{
	sampfreq=ssampfreq;
	size=ssize;
	//find out how many intervals we can have
	n=1+2*maxfreq/(maxfreq-minfreq); //Should round toward 0 during int division
	if(n/minfreq > size/sampfreq)
		n=size*minfreq/sampfreq; //Should round toward 0 during int division
	
	maxima_x=new int[n];
	low_offsets=new int[n];
	high_offsets=new int[n];
	transform=new double[size];
	y=gsl_vector_alloc(n);
	quadfit=new sonar_quadfit(n);
	fft=new fft_sonar(hydr_num, size);

	//generate low_offsets and high_offsets to store the scanning ranges
	//First, before the central peak
	for(int k=0; k<(int)(n-1)/2; k++)
	{
		low_offsets[k]=(int)ceil((k-(n-1)/2)*float(sampfreq)/minfreq);
		high_offsets[k]=(int)floor((k-(n-1)/2)*float(sampfreq)/maxfreq);
	}
	//Then, at the central peak
	low_offsets[(int)(n-1)/2]=-1;
	high_offsets[(int)(n-1)/2]=1;
	//After the central peak
	for(int k=(int)(n-1)/2; k<n; k++)
	{
		low_offsets[k]=(int)ceil((k-(n-1)/2)*float(sampfreq)/maxfreq);
		high_offsets[k]=(int)floor((k-(n-1)/2)*float(sampfreq)/minfreq);
	}
}

time_diff::~time_diff()
{
	gsl_vector_free(y);
	delete maxima_x;
	delete low_offsets;
	delete high_offsets;
	delete transform;
	delete quadfit;
	delete fft;
}

/* Update the data values
 * data needs to be a 'hydr_num'-sized array of pointers to 'size'-sized arrays
 */
int time_diff::update(int** data)
{
	return fft->update_sample(data);
}

//Calculates the actual time difference between the signals
int time_diff::calc_time_diff(int hydr1, int hydr2, double &dtime)
{
	//Fourier transform the data, multiply, transform back
	status=fft->convolve(hydr1, hydr2, transform);
	if(status!=0)
	{
		cout<<"Error doing the convolution";
		return -1;
	}

	//Find the absolute maximum
	center=get_array_max(transform,0,size-1);
	
	cout<<"Center: "<<center<<", value: "<<transform[center]<<endl;
	//Find the rest of the maxima
	for(int k=0; k<n; k++)
	{
		maxima_x[k]=get_array_max(transform,center+low_offsets[k],center+high_offsets[k]);
		gsl_vector_set(y,k,transform[rangefix(maxima_x[k])]);
	}

	//Fit to the maxima
	quadfit->fit(y,maxima_x,dtime);
	cout<<"Dtime: "<<dtime<<endl;

	//Note that the fit actaully gives the index of the array and not the time shift
	//I still need to convert it to the actual time

	return 0;
}

//Private function that calculates the index of the maximum value in an array
int time_diff::get_array_max(double* array, int start, int end)
{
	maxindex=start;
	temp=array[rangefix(start)];
	for(int k=start+1; k<=end; k++)
	{
		if(array[rangefix(k)]>temp)
		{
			temp=array[rangefix(k)];
			maxindex=k;
		}
	}

	return maxindex;
}

//Puts the index inside of the array bounds
int time_diff::rangefix(int index)
{
	if(index>size-1)
		return index-size;
	else if(index<0)
		return index+size;
	else
		return index;
}

}//sonar
}//ram
