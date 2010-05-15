# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File:  packages/vision/python/circle.py
#
# Requirements:
#   - Must have opencv python bindings
#      - Package list:
#         - python-opencv
#         - python-yaml

import math
import os
import sys
import time
import yaml

from opencv.cv import *
from opencv.highgui import *

config = {}

def clone_image(img):
    dest = cvCreateImage(cvGetSize(img),
                         img.depth,
                         img.nChannels)
    cvCopy(img, dest)
    return dest

def nothing(event, x, y, flags, param):
    pass

class Display(object):
    def __init__(self, name, img):
        self._name = name
        self._img = img
        self._x, self._y, self._r = (0, 0, 0)
        # Possible states: none, move, select, mousedown
        self._state = 'none'
        self._update = False

    def execute(self):
        self._stall = True
        cvNamedWindow('Image')

        def callback(event, x, y, flags, param):
            global config
            if event == CV_EVENT_LBUTTONDOWN:
                if self._state == 'none':
                    self._x = x
                    self._y = y
                    dest = clone_image(self._img)
                    cvCircle(dest, (x, y), 2, cvScalar(0,0,255), 3)
                    cvShowImage('Image', dest)
                    self._state = 'mousedown'
                elif self._state == 'select':
                    if math.fabs(math.sqrt((self._x - x)**2 + \
                                               (self._y - y)**2)) < self._r:
                        self._state = 'move'
                    else:
                        self._state = 'none'
                        cvShowImage('Image', self._img)
                        
            elif event == CV_EVENT_LBUTTONUP:
                if self._state == 'mousedown':
                    self._r = math.sqrt((self._x - x)**2 + \
                                                 (self._y - y)**2)
                    self._state = 'select'
                elif self._state == 'move':
                    self._state = 'select'
            elif event == CV_EVENT_MOUSEMOVE:
                if self._state == 'mousedown':
                    radius = math.sqrt((self._x - x)**2 + \
                                           (self._y - y)**2)
                    dest = clone_image(self._img)
                    cvCircle(dest, (self._x, self._y), 2, cvScalar(0,0,255), 3)
                    cvCircle(dest, (self._x, self._y), cvRound(radius),
                             cvScalar(0,255,0), 5)
                    cvShowImage('Image', dest)
                elif self._state == 'move':
                    self._x, self._y = (x, y)
                    dest = clone_image(self._img)
                    cvCircle(dest, (self._x, self._y), 2, cvScalar(0,0,255), 3)
                    cvCircle(dest, (self._x, self._y), cvRound(self._r),
                             cvScalar(0,255,0), 5)
                    cvShowImage('Image', dest)
            elif event == CV_EVENT_RBUTTONDOWN:
                if self._state == 'none':
                    self._stall = False
                elif self._state == 'select':
                    config[self._name] = { 'x' : self._x,
                                           'y' : self._y,
                                           'r' : self._r }
                    self._stall = False
                    self._state = 'none'
            
        cvSetMouseCallback('Image', callback)
        cvShowImage('Image', self._img)
        while self._stall:
            pass
        cvDestroyWindow('Image')

def main():
    if len(sys.argv) < 2:
        print 'Usage: python circle.py FOLDER'
        sys.exit(1)

    cvStartWindowThread()
    directory = os.path.expanduser(sys.argv[1])
    filename = raw_input('Save results to what file? ')

    files = [os.path.join(directory, x) for x in os.listdir(directory)]
    #cvNamedWindow('Image', 1)
    for i, f in enumerate(files):
        img = cvLoadImage(f)
        display = Display(os.path.basename(f), img)
        display.execute()
        cvReleaseImage(img)
        print '%d / %d' % (i+1, len(files))
    #cvDestroyWindow('Image')

    with open(os.path.expanduser(filename), 'w') as fd:
        yaml.dump(config, fd)

if __name__ == '__main__':
    main()
