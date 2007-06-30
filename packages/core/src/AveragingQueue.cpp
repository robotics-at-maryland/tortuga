  
/*
 * Copyright (C) 2007 Robotics at Maryland
 * All rights reserved.
 *
 * Daniel Hakim
 * File:  packages/core/src/AveragingQueue.cpp
 */
 
 #include "core/include/AveragingQueue.h"
 #include <stdlib.h>
 namespace ram {
 namespace core {
 
	AveragingQueue::AveragingQueue(int s)
	{
		size=0;
		maxSize=s;
		start=0;
		total=0;
		array=(double*)calloc(s,sizeof(double));
	}
	
	void AveragingQueue::addValue(double newValue)
	{
		total-=array[start];
		array[start]=newValue;
		total+=newValue;
		++start;
		++size;
		if (start==maxSize)
			start=0;
	}
	
	double AveragingQueue::getAverage()
	{
		if (size<maxSize)
			return total/size;
		else
			return total/maxSize;
	}
	
	AveragingQueue::~AveragingQueue()
	{
		free(array);
	}
 }//End core namespace
 }//End ram namespace