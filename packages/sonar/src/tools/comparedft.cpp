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
#include "spectrum/FFTWSpectrum.h"
#include "Sonar.h"
#include "fixed/fixed.h"

using namespace ram::sonar;
using namespace std;
namespace po = boost::program_options;

static const int N = 512;

typedef adc<16> myadc;

int main(int argc, char *argv[])
{
	long skip, begin, end;
	try
	{
		po::options_description desc("Allowed options");
		
		desc.add_options()
		("help", "Produce help message")
		("skip,s", po::value<long>(&skip)->default_value(1), "Skip every s samples")
		("begin,b", po::value<long>(&begin)->default_value(0), "Begin at sample b")
		("end,e", po::value<long>(&end)->default_value(1000), "End at sample e)")
		
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
	
	SDFTSpectrum<myadc, N, 1> sdftSpectrum;
	FFTWSpectrum<myadc, N, 1> fftwSpectrum;
	
	for (long i = 0 ; i < end ; i ++)
	{
		myadc::SIGNED sample = (myadc::SIGNED)((double)rand() / RAND_MAX * myadc::SIGNED_MAX);
		sdftSpectrum.update(&sample);
		fftwSpectrum.update(&sample);
		
		myadc::QUADRUPLE_WIDE::SIGNED diffs[N];
		for (int k = 0 ; k < N ; k ++)
		{
			const complex<myadc::DOUBLE_WIDE::SIGNED> anomaly =
				sdftSpectrum.getAmplitude(k, 0) - fftwSpectrum.getAmplitude(k, 0);
			diffs[k] = std::max(fixed::abs(anomaly.real()), fixed::abs(anomaly.imag()));
		}
		if (i >= begin && i < end && (i % skip == 0))
			fwrite(diffs, sizeof(*diffs), N, stdout);
	}
	return 0;
}
