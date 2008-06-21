#include <stdint.h>
#include <stdio.h>
#include "../include/spectrum/SDFTSpectrum.h"
#include <complex>

using namespace ram::sonar;
using namespace std;

int main()
{
	static const int N = 512;
	SDFTSpectrum<N, 1> spectrum;
	adcdata_t samp[1];
	
	while (fread(samp, 1, sizeof(adcdata_t), stdin))
	{
		spectrum.update(samp);
		for (int k = 0 ; k < N ; k ++)
		{
			adcmath_t mag = abs(spectrum.getAmplitude(k, 0));
			fwrite(&mag, 1, sizeof(mag), stdout);
		}
	}
	return 0;
}
