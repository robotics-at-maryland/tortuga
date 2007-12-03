/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  wrappers/samples/include/SharedPtrVector.h
 */

#ifndef RAM_SAMPLES_SHAREDPTRVECTOR_H
#define RAM_SAMPLES_SHAREDPTRVECTOR_H

// STD Includes
#include <vector>

// Library Includes
#include <boost/shared_ptr.hpp>

namespace samples
{

class A
{
};

boost::shared_ptr<A> func();

std::vector<A> funcVector();
    
std::vector<boost::shared_ptr<A> > funcVectorShared();

}

#endif // RAM_SAMPLES_SHAREDPTRVECTOR_H
