/* Michael Levashov
 * 05/13/08
 * Central algorithm of the sonar fft code.
 * NOTE: This function relies on the gsl library in order to do fourier transforms. This can be substituted in the future by an equivalent library.
 *
 * The algorithm first receives the data and takes the fourier transforms for all hydrophones.
 * Then, for any two hydrophones, it can convolve the transforms, from which the time delays can be estimated.
 * It requires relatively large chuncks of the signal in order to work well
 * In addition, the maximum expected time delay must be strictly less than half the length of the sample size.  Otherwise, there is a phase ambiguity
 */

#include "gsl/gsl_fft_real.h"
#include "gsl/gsl_fft_halfcomplex.h"
#include "gsl/gsl_errno.h"

#include "Sonar.h"
#include "fft_sonar.h"

using namespace std;

namespace ram {
namespace sonar {
/* Constructor.  Stores the parameters, allocates the array for transforms
 */
fft_sonar::fft_sonar(int h_num, int length)
{
	hydr_num=h_num;
	size=length;
	transforms=new double[size*hydr_num];
}


fft_sonar::~fft_sonar()
{
	delete transforms;
}


/* Stores the data and takes the fourier transforms for all hydrophones
 * Data needs to be an 'hydr_num'-sized array of pointers to arrays of size 'size'
 */
int
fft_sonar::update_sample(adcdata_t** data)
{
	//Copy the data over
	for(k=0; k<hydr_num*size; k++)
		transforms[k]=(double) data[k/size][k%size];

	//Do an in-plase fourier transform on the data
	for(k=0; k<hydr_num; k++)
		if(gsl_fft_real_radix2_transform(&transforms[k*size], 1, size) != GSL_SUCCESS)
			return -1; //Failed doing a fourier transform

	return 0;
}


/* For the two hydrophones of interest, computes the convolution and does the inverse fourier transform.
 * Stores the outpu in result
 */
int fft_sonar::convolve(int hydr1, int hydr2, double *result)
{
	//This is done because of the way that gsl stores the data
	result[0]=transforms[size*hydr1]*transforms[size*hydr2];
	for(int j=1; j<size/2; j++)
	{
		result[j]=transforms[size*hydr1+j]*transforms[size*hydr2+j]+transforms[size*hydr1+size-j]*transforms[size*hydr2+size-j];
		//result[size-j]=data1[size-j]*data2[j]-data2[size-j]*data1[j];
		result[size-j]=transforms[size*hydr1+size-j]*transforms[size*hydr2+j]-transforms[size*hydr2+size-j]*transforms[size*hydr1+j];
	}
	result[size/2]=transforms[size*hydr1+size/2]*transforms[size*hydr2+size/2];

	if(gsl_fft_halfcomplex_radix2_backward(result,1,size) == GSL_SUCCESS)
		return 0;
	else
		return -1;
}

}//sonar
}//ram
