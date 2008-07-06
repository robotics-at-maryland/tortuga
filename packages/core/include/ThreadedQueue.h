/*
 * Copyright (C) 2007 Robotics at Maryland
 * All rights reserved.
 *
 * Original Author: Paul Bridger <paulbridger.net>
 * Brought in by: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/ThreadedQueue.h
 */

#ifndef RAM_CORE_THREADEDQUEUE_H_06_18_2007
#define RAM_CORE_THREADEDQUEUE_H_06_18_2007

// STD Includes
#include <queue>

// Library Includes
#include <boost/utility.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/xtime.hpp>

namespace ram {
namespace core {

namespace details {
    boost::xtime add_xtime(const boost::xtime& a, const boost::xtime& b);
}
    
template <typename T>
class ThreadedQueue : boost::noncopyable
{
public:
    void push(const T& newData)
    {
        boost::mutex::scoped_lock lock(m_monitorMutex);
        m_queue.push(newData);
        m_itemAvailable.notify_one();
    }

    /** Copies data into given parameter if there is data
        @param data
            If there is data, the value poped off the queue will be copied into
            the given variable.  Otherwise it will be unchanged.
        
        @return true if there is data, false is there isn't
    */
    bool popNoWait(T& data)
    {
        boost::mutex::scoped_lock lock(m_monitorMutex);

        if(m_queue.empty())
        {
            return false;
        }

        // Grab the next item off the queue
        data = m_queue.front();
        m_queue.pop();
        
        return true;
    }
    
    /** Waits until new data is queue and returns that item when its pushed in */
    T popWait()
    {
        boost::mutex::scoped_lock lock(m_monitorMutex);

        if(m_queue.empty())
        {
            m_itemAvailable.wait(lock);
        }

        T temp(m_queue.front());
        m_queue.pop();
        return temp;
    }

    /** Waits until new data is in the queue or the timer runs out

    The data paramater and return time are handled the same as popNoWait
    */
    bool popTimedWait(const boost::xtime &timeout, T& data)
    {
        boost::mutex::scoped_lock lock(m_monitorMutex);
        bool success = true;

        // Boost uses and absolute timeout, so determine when we want to wake
        // up based on the current time and how long the timeout is
        boost::xtime now;
        boost::xtime_get(&now, boost::TIME_UTC);
        boost::xtime wakeUp = details::add_xtime(now, timeout);
        
        if(m_queue.empty())
        {
            success = m_itemAvailable.timed_wait(lock, wakeUp);
        }

        if (success)
        {
            data = m_queue.front();
            m_queue.pop();
            return true;
        }
        
        return false;
    }

private:
    std::queue<T> m_queue;

    boost::mutex m_monitorMutex;
    boost::condition m_itemAvailable;
};
    
} // namespace core
} // namespace ram

#endif // RAM_CORE_THREADEDQUEUE_H_06_18_2007
