/**
 * @file spectrogram.cpp
 *
 * @author Leo Singer
 * Copyright 2007 Robotics@Maryland. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/lexical_cast.hpp>

#include "spectrum/SDFTSpectrum.h"

using namespace ram::sonar;
using namespace std;
namespace po = boost::program_options;

int64_t myAbs(int64_t x)
{
	if (x < 0)
		return -x;
	else
		return x;
}

static const int nChannels = 4;
static const int N = 512;

int main(int argc, char *argv[])
{
	long skip, begin, end;
	int channel;
	try
	{
		po::options_description desc("Allowed options");
		
		desc.add_options()
		("help", "Produce help message")
		("channel,c", po::value<int>(&channel)->default_value(0), "Channel number")
		("skip,s", po::value<long>(&skip)->default_value(1), "Skip every s samples")
		("begin,b", po::value<long>(&begin)->default_value(0), "Begin at sample b")
		("end,e", po::value<long>(&end)->default_value(-1), "End at sample e (-1 for end)")
		;
		
		po::variables_map vm;
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);
		
		if (vm.count("help"))
		{
			std::cerr << desc << std::endl;
			return 1;
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "error: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	
	SDFTSpectrum<16, N, 1> spectrum;
	
	adcdata_t sample[nChannels];
	int64_t L1[N];
	int sampleCount = 0;
	while (fread(sample, sizeof(adcdata_t), nChannels, stdin) == (size_t)nChannels)
	{
		++sampleCount;
		//	Update spectrogram
		spectrum.update(&sample[channel]);
		
		for (int k = 0 ; k < N ; k ++)
		{
			const complex<int64_t> &cmplx = spectrum.getAmplitude(k, channel);
			L1[k] = (myAbs(cmplx.real()) + myAbs(cmplx.imag()));
		}
		if (sampleCount >= begin && (sampleCount < end || end < 0) && (sampleCount % skip == 0))
			fwrite(L1, sizeof(int64_t), N, stdout);
	}
	return 0;
}
