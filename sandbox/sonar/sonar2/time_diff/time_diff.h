using namespace std;

#ifndef _TIME_DIFF_
#define _TIME_DIFF_

namespace ram {
namespace sonar {

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
	time_diff(int hydr_num, int size, int sampfreq, int maxfreq, int minfreq); //number of pingers, number of data points, sampling frequency, maximum pinger freq, minimum pinger freq
	~time_diff();
	int calc_time_diff(int hydr1, int hydr2, double &dtime); //the number of the hydrophones to calculate the difference between, the actual time difference
	int update(int** data); //data passed into the funstion as an array of arrays data[hydr_num][point_num]
};

}//sonar
}//ram
#endif
