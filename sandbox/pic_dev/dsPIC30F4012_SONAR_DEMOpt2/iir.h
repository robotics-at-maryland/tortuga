/*header file for iir.c 
 *Scott Watson April 18, 2008
 *include this file whenever using iir.c functions in another file
 */

float biquadFilter(float input, float A0, float A1, float A2, float B1, float B2, float s[]);
float biquadFilter2(float input, float A[], float B[], float s[]);

float fixedBiquadFilter1(float input, float s[]);

float secondOrderFIR(float input, float A[], float s[]);

int fastAverageFIR1stOrder(int input, int s[]);
int fastAverageFIR2ndOrder(int input, int s[]);
int fastAverageFIR3rdOrder(int input, int s[]);


