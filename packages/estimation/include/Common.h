/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee <jlisee@umd.edu>
 * File:  packages/estimation/include/Common.h
 */

#ifndef RAM_ESTIMATION_COMMON_H_08_26_2010
#define RAM_ESTIMATION_COMMON_H_08_26_2010

// Library Includes
#include <boost/shared_ptr.hpp>

namespace ram {
namespace estimation {

class IStateEstimator;
typedef boost::shared_ptr<IStateEstimator> IStateEstimatorPtr;

} // namespace estimation
} // namespace ram

#endif // RAM_ESTIMATION_COMMON_H_08_26_2010
