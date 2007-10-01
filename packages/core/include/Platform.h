/*
 * Copyright (C) 2007 Robotics at Maryland
 * Copyright (C) 2007 Joseph Lisee <jlisee@umd.edu>
 * All rights reserved.
 *
 * Author: Joseph Lisee jlisee@umd.edu>
 * File:  packages/core/include/Platform.h
 */
 
#ifndef RAM_PLATFORM_H_09_29_2007
#define RAM_PLATFORM_H_09_29_2007

// Must be placed infront of classes and  functions that you wish to be 
// excesible outside of there DLL.
#if defined(RAM_WINDOWS) && !defined ( __MINGW32__ )
#   define RAM_DLL_EXPORT __declspec( dllexport ) // When building
#   define RAM_DLL_IMPORT __declspec( dllimport ) // For linking
#else
#   define RAM_DLL_EXPORT
#   define RAM_DLL_IMPORT
#endif

// Handle MSVC Compiler warnings
#if defined(RAM_WINDOWS) && !defined ( __MINGW32__ )
#pragma warning( disable : 4514 ) // Uncalled inline function removed
#pragma warning( disable : 4820 ) // Padding added to struct
#pragma warning( disable : 4710 ) // Function not inlined after request
#endif
 
#endif // RAM_PLATFORM_H_09_29_2007
