//Michael Levashov
//Performs a quadratic fit on the peak maximums and returns maximum which is closest to the fit center

#ifndef RAM_SONAR_QUADFIT
#define RAM_SONAR_QUADFIT

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_fit.h>

namespace ram {
namespace sonar {

class sonar_quadfit
{
	int n; //the length of the array. needs to be odd
	gsl_multifit_linear_workspace* workspace;
	gsl_matrix* X;
	gsl_matrix* cov;
	gsl_vector* c;
	double* chisq; //Not used, but is returned by gsl

	int status;
	int temp;
	int exp_maxx; //expected x position of the maximum
	int size; //length of the array
	double center;

	public:
	sonar_quadfit(int n);
	~sonar_quadfit();
	int fit(gsl_vector* y, int* maximax, double &result);
};

}//sonar
}//ram
#endif
