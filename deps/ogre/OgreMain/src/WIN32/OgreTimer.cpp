/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2006 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.

You may alternatively use this source under the terms of a specific version of
the OGRE Unrestricted License provided you have obtained such a license from
Torus Knot Software Ltd.
-----------------------------------------------------------------------------
*/
#include "OgreStableHeaders.h"
#include "OgreTimer.h"

using namespace Ogre;

//-------------------------------------------------------------------------
Timer::Timer()
{
	reset();
}

//-------------------------------------------------------------------------
Timer::~Timer()
{
}

//-------------------------------------------------------------------------
void Timer::reset()
{
	zeroClock = clock();

    QueryPerformanceFrequency(&mFrequency);
    QueryPerformanceCounter(&mStartTime);
    mStartTick = GetTickCount();
    mLastTime = 0;
	mQueryCount = 0;

    // Save the current process
    HANDLE mProc = GetCurrentProcess();

    // Get the current Affinity
    GetProcessAffinityMask(mProc, &mProcMask, &mSysMask);

    mThread = GetCurrentThread();
}

//-------------------------------------------------------------------------
unsigned long Timer::getMilliseconds()
{
    LARGE_INTEGER curTime;

    // Set affinity to the first core
    SetThreadAffinityMask(mThread, 1);

    // Query the timer
    QueryPerformanceCounter(&curTime);

    // Reset affinity
    SetThreadAffinityMask(mThread, mProcMask);

	// Resample the frequency
    mQueryCount++;
    if(mQueryCount == FREQUENCY_RESAMPLE_RATE)
    {
        mQueryCount = 0;
        QueryPerformanceFrequency(&mFrequency);
    }

    LONGLONG newTime = curTime.QuadPart - mStartTime.QuadPart;
    
    // scale by 1000 for milliseconds
    unsigned long newTicks = (unsigned long) (1000 * newTime / mFrequency.QuadPart);

    // detect and compensate for performance counter leaps
    // (surprisingly common, see Microsoft KB: Q274323)
    unsigned long check = GetTickCount() - mStartTick;
    signed long msecOff = (signed long)(newTicks - check);
    if (msecOff < -100 || msecOff > 100)
    {
        // We must keep the timer running forward :)
        LONGLONG adjust = (std::min)(msecOff * mFrequency.QuadPart / 1000, newTime - mLastTime);
        mStartTime.QuadPart += adjust;
        newTime -= adjust;

        // Re-calculate milliseconds
        newTicks = (unsigned long) (1000 * newTime / mFrequency.QuadPart);
    }

    // Record last time for adjust
    mLastTime = newTime;

    return newTicks;
}

//-------------------------------------------------------------------------
unsigned long Timer::getMicroseconds()
{
    LARGE_INTEGER curTime;
    QueryPerformanceCounter(&curTime);
    LONGLONG newTime = curTime.QuadPart - mStartTime.QuadPart;
    
    // get milliseconds to check against GetTickCount
    unsigned long newTicks = (unsigned long) (1000 * newTime / mFrequency.QuadPart);
    
    // detect and compensate for performance counter leaps
    // (surprisingly common, see Microsoft KB: Q274323)
    unsigned long check = GetTickCount() - mStartTick;
    signed long msecOff = (signed long)(newTicks - check);
    if (msecOff < -100 || msecOff > 100)
    {
        // We must keep the timer running forward :)
        LONGLONG adjust = (std::min)(msecOff * mFrequency.QuadPart / 1000, newTime - mLastTime);
        mStartTime.QuadPart += adjust;
        newTime -= adjust;
    }

    // Record last time for adjust
    mLastTime = newTime;

    // scale by 1000000 for microseconds
    unsigned long newMicro = (unsigned long) (1000000 * newTime / mFrequency.QuadPart);

    return newMicro;
}

//-------------------------------------------------------------------------
unsigned long Timer::getMillisecondsCPU()
{
	clock_t newClock = clock();
	return (unsigned long)((float)(newClock-zeroClock) / ((float)CLOCKS_PER_SEC/1000.0)) ;
}

//-------------------------------------------------------------------------
unsigned long Timer::getMicrosecondsCPU()
{
	clock_t newClock = clock();
	return (unsigned long)((float)(newClock-zeroClock) / ((float)CLOCKS_PER_SEC/1000000.0)) ;
}
