//Driver to test the quadfit algorithm
// g++ -lgsl -lgslcblas quadfit_test.cpp sonar_quadfit.cpp -o quadfit_test

#include <iostream>
#include "sonar_quadfit.h"
#include <gsl/gsl_vector.h>
#include <cmath>

using namespace std;
using namespace ram::sonar;

int main()
{
	int n=7; //needs to be odd!

        int* x[5];
	int x0[]={-3, 0, 1, 6, 7, 10, 14};
        int x1[]={13, 18, 20, 23, 25, 28, 32};
        int x2[]={-13, -10, -6, -3, -1, 3, 5};
        int x3[]={1, 6, 9, 11, 14, 16, 20};
        int x4[]={3, 6, 9, 13, 16, 18, 21};
	int* y[5];

        int y0[]={-6,-96,-150,-600,-726,-1176,-1944};
        int y1[]={-4, -9, -25, -64, -100, -169, -289};
        int y2[]={-32, -200, -648, -1152, -1568, -2592, -3200};
        int y3[]={-128, -8, -128, -288, -648, -968, -1800};
        int y4[]={-10, -160, -490, -1210, -1960, -2560, -3610};
        x[0]=x0;
        x[1]=x1;
        x[2]=x2;
        x[3]=x3;
        x[4]=x4;
        y[0]=y0;
        y[1]=y1;
        y[2]=y2;
        y[3]=y3;
        y[4]=y4;

	double xresult[]={-4.0000, 15.0000, -15.0000, 5.0000, 2.0000};
        double result;

	gsl_vector* yvec=gsl_vector_alloc(n);
        sonar_quadfit* myfit=new sonar_quadfit(n);
        for(int j=0; j<5; j++)
        {
	    for(int k=0;k<n;k++)
		gsl_vector_set(yvec,k,y[j][k]);

	    myfit->fit(yvec,x[j],result);
            //require two digits of precision
            if((int)round(result*100) != (int)round(xresult[j]*100))
                cout<<"Failed: "<<result<<", expected "<<xresult[j]<<endl;
            else
                cout<<"Passed\n";
        }

	delete myfit;
	gsl_vector_free(yvec);
}
