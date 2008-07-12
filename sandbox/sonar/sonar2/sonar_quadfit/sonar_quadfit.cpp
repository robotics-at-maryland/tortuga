/* Michael Levashov
 * Performs a quadratic fit on the peak maximums and returns the center of the peak
 * Could be modified to return the maximum closest to the center
 * NOTE: this builds on gsl for doing fitting, but it doesn't have to.  It would be reasonably easy to write our own fitting routine, which is just matrix manipulation
 */

#include <iostream>
#include "sonar_quadfit.h"
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_fit.h>

using namespace std;

/* Start off by allocating the workspaces and creating the necessary vectors.
 * This has to be done only once, but the vectors will be re-written often
 */

namespace ram {
namespace sonar {

sonar_quadfit::sonar_quadfit(int nn)
{
	n=nn; //n needs to be odd!!!
	if(n%2 == 0)  //if not odd,  at least reduce it to something better
        {
            cerr<<"n = "<<n<<", which is not odd in quadfit!";
		n=n-1;
        }

	workspace = gsl_multifit_linear_alloc(n, 3);
	X = gsl_matrix_alloc(n, 3);
	for(int k=0;k<n; k++)
		gsl_matrix_set(X,k,0,1); //it is 1 for all cases
	c = gsl_vector_alloc(3);
	cov = gsl_matrix_alloc(3, 3);
	chisq = new double;
}

sonar_quadfit::~sonar_quadfit()
{
	gsl_multifit_linear_free(workspace);
	gsl_matrix_free(X);
	gsl_matrix_free(cov);
	gsl_vector_free(c);
	delete chisq;
}

//Fits the maxima y and maxima x position to a parabola, returns result
int sonar_quadfit::fit(gsl_vector* y, int* maxima_x, double &result)
{
	//generate matricies X, y
	for(int k=0; k<n; k++)
	{
		temp=maxima_x[k];

	        gsl_matrix_set(X,k,1,temp);
	        gsl_matrix_set(X,k,2,temp*temp);
	}

	//fit a parabola to the values
	status=gsl_multifit_linear(X, y, c, cov, chisq, workspace);

	//find the parabola center
	center=-gsl_vector_get(c,1)/(2*gsl_vector_get(c,2));

	//Optional: implement an algorithm that will try to find the peak closest to the center.  But it seems unnecessary for now, since it will work out just as well fir multiple data points
	
	result=center;

	return status;
}

}//sonar
}//ram
