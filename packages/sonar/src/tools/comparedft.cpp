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

typedef enum {
    WAVEFORM_WHITENOISE = 0, WAVEFORM_SINE = 1
} waveform_t;

typedef enum {
    ANOMALY_DIFF_MAG = 0,
    ANOMALY_MAX_DIFF = 1,
    ANOMALY_MAG = 2,
    ANOMALY_PHASE = 3
} anomaly_t;

int main(int argc, char *argv[])
{
	long skip, begin, end;
    float scale;
    waveform_t waveformType;
    anomaly_t anomalyType;
	try
	{
		po::options_description desc("Allowed options");
		
		desc.add_options()
		("help", "Produce help message")
		("skip,s", po::value<long>(&skip)->default_value(1), "Skip every s samples")
		("begin,b", po::value<long>(&begin)->default_value(0), "Begin at sample b")
		("end,e", po::value<long>(&end)->default_value(1000), "End at sample e)")
		("scale,x", po::value<float>(&scale)->default_value(1.0), "Fraction of full scale amplitude to use for generating sample waveforms")
        ("waveform,w", po::value<int>((int*)(void*)&waveformType)->default_value(WAVEFORM_WHITENOISE), "Waveform type")
        ("anomaly,a", po::value<int>((int*)(void*)&anomalyType)->default_value(ANOMALY_DIFF_MAG), "Anomaly type")
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
		myadc::SIGNED sample;
        switch (waveformType)
        {
            case WAVEFORM_SINE:
                sample = (myadc::SIGNED)(scale * std::sin(2 * M_PI * i / 24) * myadc::SIGNED_MAX);
                break;
            case WAVEFORM_WHITENOISE:
            default:
                sample = (myadc::SIGNED)(scale * rand() / RAND_MAX * myadc::SIGNED_MAX);
                break;
        }
		sdftSpectrum.update(&sample);
		fftwSpectrum.update(&sample);
		
		myadc::DOUBLE_WIDE::SIGNED diffs[N];
		for (int k = 0 ; k < N ; k ++)
		{
            const complex<myadc::DOUBLE_WIDE::SIGNED>& mine 
                = sdftSpectrum.getAmplitude(k, 0);
            const complex<myadc::DOUBLE_WIDE::SIGNED>& theirs
                = fftwSpectrum.getAmplitude(k, 0);
			const complex<myadc::DOUBLE_WIDE::SIGNED> anomaly = mine - theirs;
            switch (anomalyType)
            {
                case ANOMALY_DIFF_MAG:
                    diffs[k] = fixed::magL2(anomaly);
                    break;
                case ANOMALY_MAX_DIFF:
                    diffs[k] = std::max(fixed::abs(anomaly.real()), fixed::abs(anomaly.imag()));
                    break;
                case ANOMALY_MAG:
                    diffs[k] = fixed::abs(fixed::magL2(mine) - fixed::magL2(theirs));
                    break;
                case ANOMALY_PHASE:
                default:
                    diffs[k] = fixed::phaseBetween(mine, theirs);
                    break;
            }
		}
		if (i >= begin && i < end && (i % skip == 0))
			fwrite(diffs, sizeof(*diffs), N, stdout);
	}
	return 0;
}
