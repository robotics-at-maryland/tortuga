/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  pattern/src/CachedObserver.cpp
 */

// STD Includes
#include <algorithm>

// Project Includes
#include "pattern/include/CachedObserver.h"

namespace ram {
namespace pattern {

CachedObserver::CachedObserver()
{
    CachedObserver::registerCachedObserver(this);
}
    
CachedObserver::~CachedObserver()
{
    CachedObserver::unregisterCachedObserver(this);
}


void CachedObserver::cacheUpdates(bool yes, bool release)
{   
    if (yes)
    {
        if (!m_cached)
            CachedObserver::registerCachedObserver(this);
        
        m_cached = true;
        return;
    }
    else
    {
        // Fire off all remaining update events
        if (m_cached)
        {
            CachedObserver::unregisterCachedObserver(this);

            if (release)
                releaseUpdates();                
        }

        m_cached = false;
        m_cache.clear();
    }    
}


void CachedObserver::releaseUpdates()
{
    if (m_cached)
    {
        boost::mutex::scoped_lock lock(m_cacheMutex);

        while (0 != m_cache.size())
        {
            UpdateListIter cur = m_cache.begin();
            cachedUpdate(boost::get<0>(*cur), boost::get<1>(*cur),
                         boost::get<2>(*cur));
            m_cache.erase(cur);
        }   
    }
}


void CachedObserver::releaseAllUpdates()
{
    boost::mutex::scoped_lock lock(s_listMutex);

    for(CachedObserverSetIter iter; iter != s_cachedObservers.end(); iter++)
    {
        const_cast<CachedObserver*>(*iter)->releaseUpdates();
    }
}


void CachedObserver::update(Subject *o, void* data, long flag)
{
    if (m_cached)
    {
        boost::mutex::scoped_lock lock(m_cacheMutex);
        m_cache.push_back(boost::make_tuple(o, data, flag));
    }
    else
    {
        cachedUpdate(o, data, flag);
    }
}


void CachedObserver::registerCachedObserver(CachedObserver* observer)
{
    boost::mutex::scoped_lock lock(s_listMutex);
    s_cachedObservers.insert(observer);
}

void CachedObserver::unregisterCachedObserver(CachedObserver* observer)
{
    boost::mutex::scoped_lock lock(s_listMutex);
    CachedObserverSetIter iter = s_cachedObservers.find(observer);

    if (iter != s_cachedObservers.end())
        s_cachedObservers.erase(iter);
}

} // namespace pattern
} // namespace ram
