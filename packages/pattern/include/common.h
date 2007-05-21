/*!
$Id: common.h 747 2006-08-09 14:54:26Z jlisee $
 
	(c) Copyright 2006
	Space Systems Laboratory, University of Maryland, College Park, MD 20742
 
	@file Contains foward declarations, typedefs, defines, and enums for all 
          classes in the ssl::foundation::pattern namespace.  It also contains 
          all needed STL and/or Boost includes for those typedefs.
 
	HISTORY
	24-Jul-2006 J Lisee     Created.
 */

#ifndef __SSL_FOUNDATION_PATTERN_COMMON_H
#define __SSL_FOUNDATION_PATTERN_COMMON_H

// Boost Includes
#include <boost/ptr_container/ptr_vector.hpp>

namespace ssl {
namespace foundation {
namespace pattern {

class Subject;
class Observer;
class SubjectManager;
    
// Subject typedefs
/*! Typedef defining generic list of Observer pointers */
typedef boost::ptr_vector<Observer> ObserverList;
/*! Typedef defining a generic iterator for a list of Observer pointers */ 
typedef boost::ptr_vector<Observer>::iterator ObserverListIter;

// Subject Manager typedefs
/*! Typedef defining generic list of Subject pointers */
typedef boost::ptr_vector<Subject> SubjectList;
/*! Typedef defining a generic iterator for a list of Subject pointers */ 
typedef boost::ptr_vector<Subject>::iterator SubjectListIter;

} // namespace ssl
} // namespace foundation
} // namespace pattern

#endif // __SSL_FOUNDATION_PATTERN_COMMON_H
