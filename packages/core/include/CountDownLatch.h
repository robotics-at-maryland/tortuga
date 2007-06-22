/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/CountDownLatch.h
 */

#ifndef RAM_CORE_COUNTDOWNLATCH_H_06_22_2007
#define RAM_CORE_COUNTDOWNLATCH_H_06_22_2007

// Library Includes
#include <boost/utility.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/xtime.hpp>

namespace ram {
namespace core {

class CountDownLatch : public boost::noncopyable
{
public:
    /** */
    CountDownLatch(int count = 0);

    /** Calling threads waits till the count on the latch is zero */
    void await();

    /** Calling thread waits till the count hits zero, or the timeout is over
     *
     *  @return True if the count reached zero, false if the timeout was
     *          reached.
     */
    bool await(boost::xtime timeout);

    /** Decrements the count on the latch, releasing waiting threads if it hits zero  0.
     */
    void countDown();

    /** Returns the current latch count */
    int getCount();

    /** Reset the count to the given value, only works if count == 0 */
    void resetCount(int count);

private:
    /** Holds current count of the latch */
    int m_count;
    
    /** Protects access to the count variable */
    boost::mutex m_mutex;

    /** Condition for waiting on the count */
    boost::condition m_countAtZero;
};
    

} // namespace core
} // namespace ram

#endif // RAM_CORE_COUNTDOWNLATCH_H_06_22_2007
