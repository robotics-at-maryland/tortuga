/*
 * Copyright (C) 2007 Robotics at Maryland
 * All rights reserved.
 *
 * Original Author: Paul Bridger <paulbridger.net>
 * Brought in by: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/ReadWriteMutex.h
 */

#ifndef RAM_CORE_READWRITEMUTEX_H_06_18_2007
#define RAM_CORE_READWRITEMUTEX_H_06_18_2007

// Library Includes
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>

// Must Be Included last
#include "core/include/Export.h"

namespace ram {
namespace core {

/** A class that implements a working read write lock with Boost.Threads.
 *
  *  It allows multiple clients may read simultaneously but write access is
 *  exclusive, writers are favoured over readers.
 */
class RAM_EXPORT ReadWriteMutex : boost::noncopyable
{
public:
    ReadWriteMutex();
	~ReadWriteMutex();

    /** Lock for Readers, follows the Boost.Thread scoped lock pattern.
     */
    class RAM_EXPORT ScopedReadLock : boost::noncopyable
    {
    public:
        ScopedReadLock(ReadWriteMutex& rwLock);

        ~ScopedReadLock();

    private:
        ReadWriteMutex& m_rwLock;
    };

    
    /** Lock for Writers, follows the Boost.Thread scoped lock pattern.
     */
    class RAM_EXPORT ScopedWriteLock : boost::noncopyable
    {
    public:
        ScopedWriteLock(ReadWriteMutex& rwLock);
        
        ~ScopedWriteLock();

    private:
        ReadWriteMutex& m_rwLock;
    };


private: // data
    boost::mutex m_mutex;

    unsigned int m_readers;
    boost::condition m_noReaders;

    unsigned int m_pendingWriters;
    bool m_currentWriter;
    boost::condition m_writerFinished;


private: // internal locking functions
    void acquireReadLock();

    void releaseReadLock();


    /**
     * @warning  This function is currently not exception-safe:
     *           if the wait calls throw, m_pendingWriter can be left in an
     *           inconsistent state
     */
    void acquireWriteLock();
    
    void releaseWriteLock();
};

} // namespace core
} // namespace ram

#endif // RAM_CORE_READWRITEMUTEX_H_06_18_2007
