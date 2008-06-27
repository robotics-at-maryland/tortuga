/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/test/src/SubsystemMaker.cxx
 */

// Library Includes
#include <UnitTest++/UnitTest++.h>
#include <boost/assign/list_of.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <vector>


// Project Includes
#include "core/include/Process.h"
#include "core/include/Task.h"
#include "core/include/TimeVal.h"

using namespace ram;

std::vector<char> output;

class TaskCounter : public core::Task 
{
public:
	int counter;
	
	TaskCounter(long updateRate, int priority = 1) :
		Task(updateRate, priority, true), counter(0)
		{
		}
		
	virtual ~TaskCounter() {}
		
	virtual void doUpdate()
	{
		counter++;
	}
};

class TaskLetter : public core::Task 
{
public:
	char letter;
	
	TaskLetter(long updateRate, int priority, char letter) :
		Task(updateRate, priority, true), letter(letter)
		{
		}
		
	virtual ~TaskLetter() {}
		
	virtual void doUpdate()
	{
		output.push_back(letter);
	}
};


TEST(oneTask)
{
    core::Process *proc1 = new core::Process(0);
	
	TaskCounter *counter = new TaskCounter(250000); // every quarter second
	
	proc1->addTask(counter);
	
	ram::core::TimeVal start = ram::core::TimeVal::timeOfDay();
	
	proc1->start();
	
	while (1)
	{
		ram::core::TimeVal now = ram::core::TimeVal::timeOfDay();
		now -= start;
		if (now.seconds() >= 4)
			break;
	}
	
	proc1->stop();
	
	//should have run about 16 times
	assert(counter->counter >= 15 && counter->counter <= 17);
	//counter's total off time should be about 4 seconds
	assert(counter->getTotalOffTime() >= 3500000 && counter->getTotalOffTime() <= 4500000);
}

TEST(fourTasks)
{
    core::Process *proc1 = new core::Process(0);
	
	TaskCounter *counter1 = new TaskCounter(100000); // every 100 ms
	TaskCounter *counter2 = new TaskCounter(200000); // every 200 ms
	TaskCounter *counter3 = new TaskCounter(500000); // every 500 ms
	TaskCounter *counter4 = new TaskCounter(1000000); // every 1000 ms
	
	proc1->addTask(counter1);
	proc1->addTask(counter2);
	proc1->addTask(counter3);
	proc1->addTask(counter4);
	
	ram::core::TimeVal start = ram::core::TimeVal::timeOfDay();
	
	proc1->start();
	
	while (1)
	{
		ram::core::TimeVal now = ram::core::TimeVal::timeOfDay();
		now -= start;
		if (now.seconds() >= 4)
			break;
	}
	
	proc1->stop();
	
	//counter 1 should have about 40
	assert(counter1->counter >= 37 && counter1->counter <= 43);
	//counter 2 should have about 20
	assert(counter2->counter >= 18 && counter2->counter <= 22);
	//counter 3 should have about 8
	assert(counter3->counter >= 7 && counter3->counter <= 9);
	//counter 4 should have about 4
	assert(counter4->counter >= 3 && counter4->counter <= 5);
	
}

TEST(twoProcesses)
{
    core::Process *proc1 = new core::Process(0);
	core::Process *proc2 = new core::Process(0);
	
	TaskCounter *counter1 = new TaskCounter(100000); // every 100 ms
	TaskCounter *counter2 = new TaskCounter(2000000); // every 2000 ms
	TaskCounter *counter3 = new TaskCounter(500000); // every 500 ms
	TaskCounter *counter4 = new TaskCounter(1000000); // every 1000 ms
	
	proc1->addTask(counter1);
	proc1->addTask(counter2);
	proc2->addTask(counter3);
	proc2->addTask(counter4);
	
	ram::core::TimeVal start = ram::core::TimeVal::timeOfDay();
	
	proc1->start();
	proc2->start();
	
	while (1)
	{
		ram::core::TimeVal now = ram::core::TimeVal::timeOfDay();
		now -= start;
		if (now.seconds() >= 4)
			break;
	}
	
	proc1->stop();
	proc2->stop();
	
	//counter 1 should have about 40
	assert(counter1->counter >= 37 && counter1->counter <= 43);
	//counter 2 should have about 2
	assert(counter2->counter >= 1 && counter2->counter <= 3);
	//counter 3 should have about 8
	assert(counter3->counter >= 7 && counter3->counter <= 9);
	//counter 4 should have about 4
	assert(counter4->counter >= 3 && counter4->counter <= 5);
	
}

TEST(priority)
{
    core::Process *proc1 = new core::Process(0);
	
	TaskLetter *counter1 = new TaskLetter(500000, 4, 'A'); // every 500 ms
	TaskLetter *counter2 = new TaskLetter(500000, 3, 'B'); // every 500 ms
	TaskLetter *counter3 = new TaskLetter(500000, 2, 'C'); // every 500 ms
	TaskLetter *counter4 = new TaskLetter(500000, 1, 'D'); // every 500 ms
	
	proc1->addTask(counter1);
	proc1->addTask(counter2);
	proc1->addTask(counter3);
	proc1->addTask(counter4);
	
	ram::core::TimeVal start = ram::core::TimeVal::timeOfDay();
	
	proc1->start();
	
	while (1)
	{
		ram::core::TimeVal now = ram::core::TimeVal::timeOfDay();
		now -= start;
		if (now.seconds() >= 4)
			break;
	}
	
	proc1->stop();
	
	for (int i=1;i<(int)output.size();i++)
	{
		if (output[i] == 'A')
			assert(output[i-1] == 'D');
		else if (output[i] == 'B')
			assert(output[i-1] == 'A');
		else if (output[i] == 'C')
			assert(output[i-1] == 'B');
		else
			assert(output[i-1] == 'C');
	}
	
}

TEST(enabled)
{
	core::Process *proc1 = new core::Process(0);
	
	TaskCounter *counter = new TaskCounter(500000); // every half second
	
	proc1->addTask(counter);
	
	ram::core::TimeVal start = ram::core::TimeVal::timeOfDay();
	
	proc1->start();
	
	while (1)
	{
		ram::core::TimeVal now = ram::core::TimeVal::timeOfDay();
		now -= start;
		if (now.seconds() == 1)
			counter->setEnabled(false);
		else if (now.seconds() == 3)
			counter->setEnabled(true);
		else if (now.seconds() >= 4)
			break;
	}
	
	proc1->stop();
	
	//should have run about 4 times
	assert(counter->counter >= 3 && counter->counter <= 5);
}

TEST(repeat)
{
	core::Process *proc1 = new core::Process(0);
	
	TaskCounter *counter = new TaskCounter(100000); // every 100 ms
	counter->setRepeat(false);
	
	proc1->addTask(counter);
	
	ram::core::TimeVal start = ram::core::TimeVal::timeOfDay();
	
	proc1->start();
	
	while (1)
	{
		ram::core::TimeVal now = ram::core::TimeVal::timeOfDay();
		now -= start;
		if (now.seconds() == 2)
			break;
	}
	
	proc1->stop();
	
	//should have run only once
	assert(counter->counter == 1);
}
