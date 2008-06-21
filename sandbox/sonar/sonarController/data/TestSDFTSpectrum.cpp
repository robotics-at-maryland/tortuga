#include <stdint.h>
#include <stdio.h>
#include <iostream>
#include "../include/spectrum/SDFTSpectrum.h"
#include <complex>
#include <sys/time.h>

using namespace ram::sonar;
using namespace std;

int main()
{
	static const int N = 512;
	static const size_t inCap = 60000;
	SDFTSpectrum<N, 1> spectrum;
	adcdata_t samp[inCap];
	
	int nSamples = fread(samp, sizeof(adcdata_t), inCap, stdin);
	
	timeval start, finish;
	
	gettimeofday(&start, NULL);
	for (int i = 0 ; i < nSamples ; i ++)
	{
		spectrum.update(&samp[i]);
	}
	gettimeofday(&finish, NULL);
	
	double dt = (finish.tv_sec+(finish.tv_usec/1000000.0)-(start.tv_sec+(start.tv_usec/1000000.0)));
	double fs = 500000;  // sample rate, 500 ksps
	
	double dtMs = dt * 1000;
	double msPerSample = dtMs / nSamples;
	
	cout << "Read " << nSamples << " samples at " << fs/1000 << " ksps." << endl;
	cout << "Completed processing in " << dtMs << " ms, for " << msPerSample << " ms per sample." << endl;
	cout << "It would take " << 2 * fs * msPerSample/1000 << " seconds to process 2 seconds of sound." << endl;
	/*
	while (fread(samp, 1, sizeof(adcdata_t), stdin))
	{
		spectrum.update(samp);
		for (int k = 0 ; k < N ; k ++)
		{
			complex<int64_t> amp = spectrum.getAmplitude(k, 0);
			int64_t mag = sqrt(amp.real()*amp.real() + amp.imag() * amp.imag());
			fwrite(&mag, 1, sizeof(mag), stdout);
		}
	}
	*/
	
	return 0;
}
