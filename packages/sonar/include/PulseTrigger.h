/**
 * @file PulseTrigger.h
 * Pulse-base triggering.  Return true if a signal exceeds a given threshold
 * value for a given period of time, false if otherwise.
 *
 * @author Leo Singer
 * @author Copyright 2007 Robotics@Maryland. All rights reserved.
 * 
 */

#ifndef _RAM_SONAR_PULSETRIGGER_H
#define _RAM_SONAR_PULSETRIGGER_H

namespace ram { namespace sonar {

template<class T, int N, int nChannels>
class PulseTrigger
{
private:
	T buf[N][nChannels];
	T threshold[nChannels];
	size_t countAboveThresholds[nChannels];
	int idx;
public:
	PulseTrigger() { purge(); }
	
	void purge()
	{
		bzero(buf, sizeof(T) * N * nChannels);
		bzero(countAboveThresholds, sizeof(size_t) * nChannels);
		idx = 0;
	}
	
	void update(const T sample[nChannels])
	{
		++idx;
		if (idx >= N)
			idx = 0;
		
		T *bufPtr = buf[idx];
		memcpy(bufPtr, sample, sizeof(T) * nChannels);
		
		for (int channel = 0 ; channel < nChannels ; channel ++)
		{
			T &signal = bufPtr[channel];
			size_t &count = countAboveThresholds[channel];
			if (signal >= threshold[channel]) {
				if (count < N)
					++count;
			} else {
				count = 0;
			}
		}
	}
	
	void setThresholds(const T &thresh)
	{
		for (int channel = 0 ; channel < nChannels ; channel ++)
			setThreshold(channel, thresh);
	}
	
	void setThreshold(int channel, const T &thresh)
	{
		threshold[channel] = thresh;
	}
	
	bool operator () (int channel) const
	{
		return countAboveThresholds[channel] >= N;
	}
};
	
}} // namespace ram::sonar

#endif
