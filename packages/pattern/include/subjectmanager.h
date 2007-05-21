/*!
$Id: subjectmanager.h 1142 2006-09-22 21:54:02Z jlisee $
 
    (c) Copyright 2006
	Space Systems Laboratory, University of Maryland, College Park, MD 20742
 
	@file 
        Contains the delcation for the SubjectManager class.
 
	HISTORY
    20-Jul-2006 J Lisee     Imported into ssl::foundation::pattern library.
    25-Fed-2006 J Lisee     Created.
*/

#ifndef __SUBJECTMANAGER_H
#define __SUBJECTMANAGER_H

// Boost Includes
#include <boost/utility.hpp>

// Fonudation includes (gets forward declaration of Subject)
#include "rtsx/foundation/pattern/h/common.h"

namespace ssl {
namespace foundation {
namespace pattern {

class Subject;
    
/*! This class allows a set of obersable object to be update all at once witout
    the users haveing to be aware of every object that is needs to notify.
 */
class SubjectManager : private boost::noncopyable
{
public:
    /*! Constructor */
    SubjectManager();
    
    /*! Desctructor 
        @remarks
            This releases ownership off all objects held in the ptr_container, 
            so they will not be automatically destroyed with the object.
    */
    ~SubjectManager();
    
    /*! Adds an subject to the list of Subject for this object
    @remarks 
        You can not have the same Subject twice in one set, each subject
        is sorted by its location in memory, ie its pointer. This class is not
        reponsible for deleting the subjects.
    @param 
        subject The object to observe
     */
    void addSubject(Subject *subject);
    
    /*! Returns the number of Subject's of this object manages.
    @return 
        The number of Subject's this object manages.
    */
    int countSubjects();
    
    /*!  Deletes an Subject from the set of Subjects of this object.
    @remarks 
        We determine the object to delete by comparing pointers
     */
    void removeSubject(Subject *subject);
    
    /*! Clears the Subject list for this object
     */
    void removeSubjects();
    
    /*! Calls notifyObservers on all the Subjects contained by this object
    @remarks
        Each Subject will only update its observers if it has changed state.
     */
    void notifySubjects();
    
protected:
    /*! List of Subjects to notify */
    SubjectList mSubjects;
};
    
} // namespace pattern
} // namespace foundation
} // namespace ssl

#endif // __SUBJECTMANAGER_H
