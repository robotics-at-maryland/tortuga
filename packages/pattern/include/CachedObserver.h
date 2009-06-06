/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  pattern/include/CachedObserver.h
 */

#ifndef RAM_PATTERN_CACHEDOBSERVER_H_05_30_2007
#define RAM_PATTERN_CACHEDOBSERVER_H_05_30_2007

// STD Includes
#include <set>
#include <list>

// Library Includes
#include "boost/tuple/tuple.hpp"
#include "boost/tuple/tuple_comparison.hpp"
#include "boost/thread/mutex.hpp"

// Project includes (gets forward declaration of Subject)
#include "pattern/include/Observer.h"

// Must Be Included last
#include "pattern/include/Export.h"

namespace ram {
namespace pattern {

class RAM_EXPORT CachedObserver;

typedef boost::tuple<Subject*, void*, long> UpdateItem;
typedef std::list<UpdateItem> UpdateList;
typedef UpdateList::iterator UpdateListIter;
typedef std::set<CachedObserver*> CachedObserverSet;
typedef CachedObserverSet::iterator CachedObserverSetIter;

/** Implements the observer interface but stores updates for later release.
 *
 *  It uses a static cache that allows global release of all updates
 *  or only for the current observer certain object.  This <b>is</b> thread
 *  safe.
 */
class RAM_EXPORT CachedObserver : public Observer
{
public:
    CachedObserver(bool ignoreData = false);
    
    virtual ~CachedObserver();

    /** Starts caching of updates for this observer */
    void cacheUpdates(bool yes = true, bool release = true);

    /** Releases all updates for this observer */
    void releaseUpdates();

    /** Releases all updates for al observers */
    static void releaseAllUpdates();

    /** The default behavior of the method is to cache the call
     *
     *  To actually get the resutls, call releaseUpdates, and the
     *  cachedUpdates method will be called.
     */
    virtual void update(Subject *o, void* data = 0, long flag = 0);

    /** Called when the updates are releaesd  */
    virtual void cachedUpdate(Subject* o, void* data, long flag) = 0;

    /** Called when we don't care about the data */
    virtual void cachedUpdate(Subject* o, long flag) = 0;
    
private:
    static void registerCachedObserver(CachedObserver* observer);
    static void unregisterCachedObserver(CachedObserver* observer);
    
    /** Whether or not to cache updates */
    bool m_cached;

    /** Whether or not to pass the data onto the subject */
    bool m_ignoreData;
    
    /** Protects access to cache */
    boost::mutex m_cacheMutex;
    /** Holds cached updates*/
    UpdateList m_cache;
    
    /** Sync's access to the list of observers */
    static boost::mutex s_listMutex;
    /** Static list of all cached observser */
    static CachedObserverSet s_cachedObservers;
};

} // namespace pattern
} // namespace ram

#endif // RAM_PATTERN_OBSERVER_H_05_30_2007
