/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  packages/math/include/Export.h
 */
 
// Note include guards intentially left out of this file
 
// Project Includes
#include "core/include/Platform.h"
 
// Undef RAM_EXPORT 
#ifdef RAM_EXPORT
#   undef RAM_EXPORT
#endif
 
// Redfine based on whether we are currently compiling this package
#ifdef RAM_PKG_PATTERN
#   define RAM_EXPORT RAM_DLL_EXPORT
#else
#   define RAM_EXPORT RAM_DLL_IMPORT
#endif
