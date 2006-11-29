############################################################################
#                                                                          #
#    iSink ROV Simulator                                                   #
#                                                                          #
#    written for Robotics@Maryland 2006                                    #
#    University of Maryland, College Park                                  #
#                                                                          #
#    Copyright (C) 2006 by Leo Singer                                      #
#    aronnax@umd.edu                                                       #
#                                                                          #
############################################################################

"""Renderer and simulation runloop"""

import physics
from time import *
from threading import *
from OpenGL.GLUT import *
from OpenGL.GLU import *
from OpenGL.GL import *


class Simulator:
	
	def __init__(self, world):
		self.world = world
		self.win = 0
		self.lastime = 0
		self.thistime = 0
	
	def go(self):
		glutInitWindowSize(500,500)
		glutInitWindowPosition(100,100)
		glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE)
		self.win = glutCreateWindow("iSink ROV Simulator")
		self.init()
		glutDisplayFunc(self.display)
		glutIdleFunc(self.doframe)
		self.thistime = self.lastime = time()
		glutMainLoop()
	
	def init(self):
		glEnable(GL_DEPTH_TEST)
		glClearColor(0,0,0,1)
		glMatrixMode(GL_PROJECTION)
		glLoadIdentity()
		gluPerspective(60,1,0.1,1000)
		gluLookAt(-10,0,0,0,0,0,0,0,1)
		glMatrixMode(GL_MODELVIEW)
		glLoadIdentity()
		glEnable(GL_COLOR_MATERIAL)
		glShadeModel(GL_SMOOTH)
		glLightfv(GL_LIGHT0,GL_POSITION,(0,0,1,0))
		glLightfv(GL_LIGHT0,GL_AMBIENT,(0.5,0.5,0.5,1))
		glLightfv(GL_LIGHT0,GL_DIFFUSE,(0.5,0.5,0.5,1))
		glEnable(GL_LIGHTING)
		glEnable(GL_LIGHT0)
	
	def display(self):
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
		self.world.display()
		glutSwapBuffers()
	
	def doframe(self):
		self.lastime = self.thistime
		self.thistime = time()
		self.world.step(self.thistime - self.lastime)
		glutSetWindow(self.win)
		glutPostRedisplay()


class ControlThread(Thread):
	"""Base class for vehicle controller"""
	
	def __init__(self, sub):
		Thread.__init__(self)
		self.setDaemon(True)
		self.sub = sub
