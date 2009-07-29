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
#include "Sonar.h"
#include "fixed/fixed.h"

using namespace ram::sonar;
using namespace std;
namespace po = boost::program_options;

static const int nChannels = 4;
static const int N = 512;

typedef adc<16> myadc;

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
	
	SDFTSpectrum<myadc, N, 1> spectrum;
	
	myadc::SIGNED sample[nChannels];
	myadc::DOUBLE_WIDE::SIGNED L1[N];
	int sampleCount = 0;
	while (fread(sample, sizeof(myadc::SIGNED), nChannels, stdin) == (size_t)nChannels)
	{
		++sampleCount;
		//	Update spectrogram
		spectrum.update(&sample[channel]);
		
		for (int k = 0 ; k < N ; k ++)
		{
			L1[k] = fixed::magL1(spectrum.getAmplitude(k, channel));
		}
		if (sampleCount >= begin && (sampleCount < end || end < 0) && (sampleCount % skip == 0))
			fwrite(L1, sizeof(myadc::DOUBLE_WIDE::SIGNED), N, stdout);
	}
	return 0;
}
