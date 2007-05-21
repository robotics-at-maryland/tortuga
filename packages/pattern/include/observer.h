/*!
$Id: observer.h 1211 2006-09-28 15:36:27Z roderick $
 
	(c) Copyright 2006
	Space Systems Laboratory, University of Maryland, College Park, MD 20742
 
	@file 
        Contains the delcation for the Subject class.
     @note 
        The interface and some of the comments come from Sun's Observer class
 
	HISTORY
    20-Jul-2006 J Lisee     Imported into ssl::foundation::pattern library.
*/

#ifndef __OBSERVER_H
#define __OBSERVER_H

// Fonudation includes (gets forward declaration of Subject)
#include "foundation/pattern/h/common.h"

namespace ssl {
namespace foundation {
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
} // namespace foundation
} // namespace ssl
#endif // __OBSERVER_H
