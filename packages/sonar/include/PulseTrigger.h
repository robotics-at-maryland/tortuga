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
	
	PulseTrigger(T threshold)
	{
		purge();
		setThresholds(threshold);
	}
	
	/**
	 * Place this class in the state that the constructor leaves it in.
	 */
	void purge()
	{
		bzero(buf, sizeof(T) * N * nChannels);
		bzero(countAboveThresholds, sizeof(size_t) * nChannels);
		idx = 0;
	}
	
	void update(const T sample[nChannels])
	{
		//	Update index in circular buffer
		++idx;
		if (idx >= N)
			idx = 0;
		
		//	Get a pointer to the current set of samples
		T *bufPtr = buf[idx];
		
		//	Copy new samples into the buffer
		memcpy(bufPtr, sample, sizeof(T) * nChannels);
                
                // Examine each channel with respect to its threshold,
                // increment countAboveThresholds as needed
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

	/** Returns true if the given channel had N samples above the threshold */
	bool operator () (int channel) const
	{ return countAboveThresholds[channel] >= N; }
};
	
}} // namespace ram::sonar

#endif
