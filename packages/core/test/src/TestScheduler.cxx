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

// Project Includes
#include "core/include/Process.h"
#include "core/include/Task.h"

using namespace ram;

class TaskA : public core::Task 
{
public:
	char letter;
	
	TaskA(long updateRate, char letter) :
		Task(updateRate, 1, true), letter(letter)
		{
		}
		
	virtual ~TaskA() {}
		
	virtual void update()
	{
		startUpdate();
		std::cout << letter << letter << letter << "\n";
		endUpdate();
	}
};


TEST(basicTest)
{
    core::Process *proc1 = new core::Process(0);
	proc1->addTask(new TaskA(3500000, 'A'));
	proc1->addTask(new TaskA(370000, 'C'));
	proc1->addTask(new TaskA(3900000, 'B'));
	
	core::Process *proc2 = new core::Process(0);
	proc2->addTask(new TaskA(1000000, 'D'));
	proc2->addTask(new TaskA(1500000, 'E'));
	proc2->addTask(new TaskA(2000000, 'F'));
	
	proc1->start();
	proc2->start();
	
	while (1)
	{
	}

}
