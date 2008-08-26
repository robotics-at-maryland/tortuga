/**
 * @file SimGL.h
 *
 * Include OpenGL, GLU, and GLUT from the correct places, regardless of platform
 *
 * @author Leo Singer
 * @author Copyright 2008 Robotics@Maryland. All rights reserved.
 */

#ifndef _RAM_SIM_SIMGL_H
#define _RAM_SIM_SIMGL_H

#if defined(__APPLE__) || defined(MACOSX)
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#include <SDL_opengl.h>

#endif
