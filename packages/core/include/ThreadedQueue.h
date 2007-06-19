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

namespace ram {
namespace core {

template <typename DataT>
class ThreadedQueue : boost::noncopyable
{
public:
    void push(const DataT& newData)
    {
        boost::mutex::scoped_lock lock(m_monitorMutex);
        m_queue.push(newData);
        m_itemAvailable.notify_one();
    }

    /** Waits until new data is queue */
    DataT popWait()
    {
        boost::mutex::scoped_lock lock(m_monitorMutex);

        if(m_queue.empty())
        {
            m_itemAvailable.wait(lock);
        }

        DataT temp(m_queue.front());
        m_queue.pop();
        return temp;
    }

    /** Waits until new data is in the queue or the timer runs out */
    DataT popTimedWait(const boost::xtime &xt)
    {
        boost::mutex::scoped_lock lock(m_monitorMutex);

        if(m_queue.empty())
        {
            m_itemAvailable.timed_wait(lock, xt);
        }

        DataT temp(m_queue.front());
        m_queue.pop();
        return temp;
    }

private:
    std::queue<DataT> m_queue;

    boost::mutex m_monitorMutex;
    boost::condition m_itemAvailable;
};

} // namespace core
} // namespace ram

#endif // RAM_CORE_THREADEDQUEUE_H_06_18_2007
