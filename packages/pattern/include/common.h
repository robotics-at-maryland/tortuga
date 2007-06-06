/*
 *  (c) Copyright 2006
 *	Space Systems Laboratory, University of Maryland, College Park, MD 20742
 */

#ifndef RAM_PATTERN_COMMON_H_05_30_2007
#define RAM_PATTERN_COMMON_H_05_30_2007

// Boost Includes
#include <boost/ptr_container/ptr_vector.hpp>

namespace ram {
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

} // namespace pattern
} // namespace ram

#endif // RAM_PATTERN_COMMON_H_05_30_2007
