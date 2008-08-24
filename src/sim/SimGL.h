/**
 * @file SimGL.h
 *
 * Include OpenGL, GLU, and GLUT from the correct places, regardless of platform
 *
 * @author Leo Singer
 * @author Copyright 2008 Robotics@Maryland. All rights reserved.
 */

#if defined(__APPLE__) || defined(MACOSX)
    #include <GL/glew.h>
	#include <GLUT/glut.h>
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
#else
	#include <GL/glew.h>
	#include <GL/glut.h>
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif
