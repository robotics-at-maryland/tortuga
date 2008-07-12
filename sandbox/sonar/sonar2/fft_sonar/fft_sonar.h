//Michael Levashov
//The class fft_sonar does the convolution of the signals for calculating the time delay of arrival
//Each time new data comes is, call update_sample with a pointer to the new data[ping_num*size]
//Then, just call the fft_sonar with the pair of pingers you want (from 0 to ping_num-1) and it will calculate the convolution
//05/13/08

#ifndef _FFT_SONAR_
#define _FFT_SONAR_

namespace ram {
namespace sonar {
class fft_sonar
{
	double *transforms;
	int status;
	int size;
	int hydr_num;
	int k;

	public:
	fft_sonar(int h_num, int length);
	~fft_sonar();
	int update_sample(int** data);
	int convolve(int hydr1, int hydr2, double* result);
};

}//sonar
}//ram
#endif
