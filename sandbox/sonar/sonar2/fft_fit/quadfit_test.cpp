//Driver to test the quadfit algorithm
// g++ -lgsl -lgslcblas quadfit_test.cpp sonar_quadfit.cpp -o quadfit_test

#include <iostream>
#include "sonar_quadfit.h"
#include <gsl/gsl_vector.h>

using namespace std;

int main()
{
	int n=13; //needs to be odd!

	int x[]={-4,-2,-1,1,3,4,6,7,9,11,12,14,16};
	int y[]={26,61,75,98,113,117,120,128,119,99,80,51,14};
	double result;

	gsl_vector* yvec=gsl_vector_alloc(n);
	for(int k=0;k<n;k++)
		gsl_vector_set(yvec,k,y[k]);

	sonar_quadfit* myfit=new sonar_quadfit(n);

	myfit->fit(yvec,x,result);

	delete myfit;
	cout<<"Best fit x: "<<result<<endl;
	gsl_vector_free(yvec);
}
