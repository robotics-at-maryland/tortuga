/*!
$Id: subject.h 1211 2006-09-28 15:36:27Z roderick $
 
	(c) Copyright 2006
	Space Systems Laboratory, University of Maryland, College Park, MD 20742
 
	@file 
        Contains the delcation for the Subject class.
    @note 
        The interface and some of the comments come from Sun's Observable class
 
	HISTORY
    20-Jul-2006 J Lisee     Imported into ssl::foundation::pattern library.
*/


#ifndef __SUBJECT_H
#define __SUBJECT_H

// Fonudation includes (gets forward declaration or Observer)
#include "foundation/pattern/h/common.h"

namespace ssl {
namespace foundation {
namespace pattern {
    
/*! This class represents an Subject object, or "data" in the model-view 
    paradigm.
    @remarks 
        It can be subclassed to represent an object that the application
        wants to have observed. @n

        A Subject object can have one or more observers. An observer may
        be any object that implements interface Observer. After a Subject
        instance changes, an application calling the Subject's
        notifyObservers method causes all of its observers to be notified of
        the change by a call to their update method. @n

        The order in which notifications will be delivered is unspecified.
        The default implementation provided in the Observerable class will
        notify Observers in the order in which they registered interest, but
        subclasses may change this order, use no guaranteed order, deliver
        notifications on separate threads, or may guarantee that their
        subclass follows this order, as they choose. @n

        Note that this notification mechanism is has nothing to do with
        threads and is completely separate from the wait and notify
        mechanism of class Object. @n

        When an Subject object is newly created, its set of observers is
        empty. Two observers are considered the same if and only if the
        equals method returns true for them. @n
 */
class Subject
{
public:
    Subject();
    virtual ~Subject();
    
    /*! Adds an observer to the list of observers for this object
        @remarks 
            You can not have the same observer twice in one set, each
            observer is sorted by its location in memory, ie its pointer.
        @param o 
            The object to observe
        */
    virtual void addObserver(Observer *o);
    
    /*! Returns the number of observers of this Subject object.
        @return 
            The number of observers for this object
    */
    virtual int countObservers();
    
    /*!  Deletes an observer from the set of observers of this object.
        @remarks 
            We determine the object to delete by comparing pointers
    */
    virtual void removeObserver(Observer *o);
    
    /*! Clears the observer list for this object */
    virtual void removeObservers();
    
    /*!  Tests if this object has changed.
        @return 
            True is the object has changed and false if it has not
    */
    virtual bool hasChanged();
    
    /*! Notifies all observers if the object is in a changed state
        @pre
            The objects hasChanged() method returns true.
        @post
            All obsevers are notified and clearChanged() hass been called.
        @param data
            A void pointer that can carry data to the observers.
        @param flag
            A flag to help observers figure out what kind of data they are 
            recieving of it can just be a message used to signal the observer
            to collect the data it needs, in that case just set data to 0.
     */
    virtual void notifyObservers(void* data = 0, long flag = 0);
    
    /*! Comparison function
        @remarks
            This is needed because the boost ptr_containers, don't  forward
            overloaded operators yet.  It is comming in the next version
    */
    bool equals(const Subject& o) const { return (this == &o);};
    
protected:
    /*! Sets the objects changed state to false
        @remarks 
            Indicates that this object has no longer changed, or that it has
            already notified all of its observers of its most recent change,
            so that the hasChanged method will now return false.
    */
    virtual void clearChanged();
    
    /*! Marks this Subject object as having been changed
        @remarks 
            The hasChanged method will now return true.
    */
    virtual void setChanged();
    
    /*! Stores the changed status of the object
        @remarks 
            If the object is listed as changed it will notify its observers
            when notifyObservers() is called.
    */
    bool mChanged;
    
    /*! List of Observer objects to notify */
    ObserverList mObservers;
};

} // namespace pattern
} // namespace foundation
} // namespace ssl
#endif // __SUBJECT_H
