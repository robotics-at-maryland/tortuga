/*
 *  (c) Copyright 2006
 *	Space Systems Laboratory, University of Maryland, College Park, MD 20742
 */

#ifndef RAM_PATTERN_OBSERVER_H_05_30_2007
#define RAM_PATTERN_OBSERVER_H_05_30_2007

// Fonudation includes (gets forward declaration of Subject)
#include "pattern/include/common.h"

namespace ram {
namespace pattern {

/*! A class can implement the Observer interface when it wants to be informed
    of changes in Subject objects. Thi9 class is pure abstract.
*/
class Observer
{
public:
    virtual ~Observer() {};
    /*! This method is called whenever the observed object is changed.
        @param o 
            The object being observed
        @param data
            A void pointer carry data for the update.
        @param flag
            A flag to help observer know what kind of data they are getting.
    */
    virtual void update(Subject *o, void* data = 0, long flag = 0) = 0;
    
    /*! Comparison function
        @remarks
            This is needed because the boost ptr_containers, don't  forward
            overloaded operators yet.  It is comming in the next version
    */
    bool equals(const Observer& o) const { return (this == &o);};
};

} // namespace pattern
} // namespace ram

#endif // RAM_PATTERN_OBSERVER_H_05_30_2007
