#include <cmath>
#include <gsl/gsl_vector.h>
#include "sonar_quadfit.h"
#include "fft_sonar.h"

using namespace std;

#ifndef _TIME_DIFF_
#define _TIME_DIFF_

class time_diff
{
	int* maxima_x;
	int* low_offsets;
	int* high_offsets;
	double* transform;
	int size;
	int sampfreq;
	int n;
	sonar_quadfit* quadfit;
	fft_sonar *fft;

	int center;
	double temp; 
	int maxindex;
	gsl_vector* y;
	int status;

	int get_array_max(double* array, int start, int end);
	int rangefix(int index);

	public:
	time_diff(int ping_num, int size, int sampfreq, int maxfreq, int minfreq); //number of pingers, number of data points, sampling frequency, maximum pinger freq, minimum pinger freq
	~time_diff();
	int calc_time_diff(int pinger1, int pinger2, double &dtime); //the number of the pingers to calculate the difference between, the actual time difference
	int update(int** data); //data passed into the funstion as an array of arrays data[pinger_num][point_num]
};
#endif
