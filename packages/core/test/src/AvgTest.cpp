/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Daniel Hakim  <dhakim@umd.edu>
 * All rights reserved.
 *
 * Author: Daniel Hakim <dhakim@umd.edu>
 * File:  packages/core/test/src/AvgTest.cpp
 */

// STD Includes
#include <iostream>

// Project Includes
#include "core/include/AveragingFilter.h"

using namespace std;
using namespace ram::core;

int main()
{
    int total = 0;
    AveragingFilter<double, 10> q;
    for (int i = 1; i < 25; i++)
    {
        total += i;
        cout << "adding:" << i << endl;
        q.addValue(i);
        cout << "Average (Total: " << "):" << q.getValue() << endl;
    }
    return 0;
}
