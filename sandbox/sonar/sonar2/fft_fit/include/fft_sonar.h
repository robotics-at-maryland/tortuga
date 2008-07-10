//Michael Levashov
//The class fft_sonar does the convolution of the signals for calculating the time delay of arrival
//Each time new data comes is, call update_sample with a pointer to the new data[ping_num*size]
//Then, just call the fft_sonar with the pair of pingers you want (from 0 to ping_num-1) and it will calculate the convolution
//05/13/08
#include "gsl/gsl_fft_real.h"
#include "gsl/gsl_fft_halfcomplex.h"
#include "gsl/gsl_errno.h"

#ifndef _FFT_SONAR_
#define _FFT_SONAR_

class fft_sonar
{
	double *transforms;
	int status;
	int size;
	int ping_num;
	int k;

	public:
	fft_sonar(int p_num, int length);
	~fft_sonar();
	int update_sample(int** data);
	int convolve(int pinger1, int pinger2, double* result);
};
#endif
