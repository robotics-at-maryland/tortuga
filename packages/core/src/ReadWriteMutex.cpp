/*
 * Copyright (C) 2007 Robotics at Maryland
 * All rights reserved.
 *
 * Original Author: Paul Bridger <paulbridger.net>
 * Brought in by: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/core/include/ReadWriteMutex.h
 */

// Project Includes
#include "core/include/ReadWriteMutex.h"

namespace ram {
namespace core {

ReadWriteMutex::ReadWriteMutex() :
    m_readers(0),
    m_pendingWriters(0),
    m_currentWriter(false)
{}

ReadWriteMutex::~ReadWriteMutex()
{
}

void ReadWriteMutex::acquireReadLock()
{
    boost::mutex::scoped_lock lock(m_mutex);
    
    // require a while loop here, since when the writerFinished condition is notified
    // we should not allow readers to lock if there is a writer waiting
    // if there is a writer waiting, we continue waiting
    while(m_pendingWriters != 0 || m_currentWriter)
    {
        m_writerFinished.wait(lock);
    }
    ++m_readers;
}

void ReadWriteMutex::releaseReadLock()
{
    boost::mutex::scoped_lock lock(m_mutex);
    --m_readers;
    
    if(m_readers == 0)
    {
            // must notify_all here, since if there are multiple waiting writers
            // they should all be woken (they continue to acquire the lock exclusively though)
        m_noReaders.notify_all();
    }
}

// this function is currently not exception-safe:
// if the wait calls throw, m_pendingWriter can be left in an inconsistent state
void ReadWriteMutex::acquireWriteLock()
{
    boost::mutex::scoped_lock lock(m_mutex);
    
    // ensure subsequent readers block
    ++m_pendingWriters;
        
    // ensure all reader locks are released
    while(m_readers > 0)
    {
        m_noReaders.wait(lock);
    }
    
    // only continue when the current writer has finished 
    // and another writer has not been woken first
    while(m_currentWriter)
    {
        m_writerFinished.wait(lock);
    }
    --m_pendingWriters;
    m_currentWriter = true;
}

void ReadWriteMutex::releaseWriteLock()
{        
    boost::mutex::scoped_lock lock(m_mutex);
    m_currentWriter = false;
    m_writerFinished.notify_all();
}

// Scoped Read Lock
ReadWriteMutex::ScopedReadLock::ScopedReadLock(ReadWriteMutex& rwLock) :
    m_rwLock(rwLock)
{
    m_rwLock.acquireReadLock();
}

ReadWriteMutex::ScopedReadLock::~ScopedReadLock()
{
    m_rwLock.releaseReadLock();
}

// Scoped Write Lock
ReadWriteMutex::ScopedWriteLock::ScopedWriteLock(ReadWriteMutex& rwLock) :
    m_rwLock(rwLock)
{
    m_rwLock.acquireWriteLock();
}

ReadWriteMutex::ScopedWriteLock::~ScopedWriteLock()
{
    m_rwLock.releaseWriteLock();
}

} // namespace core
} // namespace ram
