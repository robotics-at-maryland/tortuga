 
/*
 * Copyright (C) 2007 Robotics at Maryland
 * All rights reserved.
 *
 * Daniel Hakim
 * File:  packages/core/include/AveragingQueue.h
 */

#ifndef RAM_CORE_AVERAGINGQUEUE_H_06_30_2007
#define RAM_CORE_AVERAGINGQUEUE_H_06_30_2007

namespace ram {
namespace core {

class AveragingQueue
{
	public:
		AveragingQueue(int maxSize);
		~AveragingQueue();
		void addValue(double newValue);
		double getAverage();
	
	private:
		int size;
		int maxSize;
		int start;
		double total;
		double* array;
};

}
}

#endif // RAM_CORE_AVERAGINGQUEUE_H_06_30_2007
