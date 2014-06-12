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
        self._x1, self._y1, self._x2, self._y2 = (0, 0, 0, 0)
        # Possible states: none, move, select, mousedown
        self._state = 'none'
        self._update = False

    def execute(self, config):
        self._stall = True
        cvNamedWindow('Image')

        def callback(event, x, y, flags, param):
            if event == CV_EVENT_LBUTTONDOWN:
                if self._state == 'none':
                    self._x1 = x
                    self._y1 = y
                    self._x2 = x
                    self._y2 = y
                    dest = clone_image(self._img)
                    cvRectangle(dest, (self._x1, self._y1),
                                (self._x2, self._y2), cvScalar(0,0,255), 3)
                    cvShowImage('Image', dest)
                    self._state = 'mousedown'
                elif self._state == 'select':
                    width, height = abs(self._x2 - self._x1), \
                        abs(self._y2 - self._y1)
                    x1 = min(self._x1, self._x2)
                    y1 = min(self._y1, self._y1)
                    if x <= x1 + width and x >= x1 and \
                            y <= y1 + height and y >= y1:
                        self._state = 'move'
                        self._movex = x
                        self._movey = y
                    else:
                        self._state = 'none'
                        cvShowImage('Image', self._img)
                        
            elif event == CV_EVENT_LBUTTONUP:
                if self._state == 'mousedown':
                    self._x2, self._y2 = x, y
                    self._state = 'select'
                elif self._state == 'move':
                    transx, transy = x - self._movex, y - self._movey
                    self._x1, self._y1 = self._x1 + transx, self._y1 + transy
                    self._x2, self._y2 = self._x2 + transx, self._y2 + transy
                    self._state = 'select'
            elif event == CV_EVENT_MOUSEMOVE:
                if self._state == 'mousedown':
                    self._x2, self._y2 = x, y
                    dest = clone_image(self._img)
                    cvRectangle(dest, (self._x1, self._y1),
                                (self._x2, self._y2), cvScalar(0,0,255), 3)
                    cvCircle(dest, (self._x1 + cvRound((self._x2-self._x1)/2),
                                    self._y1 + cvRound((self._y2-self._y1)/2)),
                             3, cvScalar(0,255,0), -1)
                    cvShowImage('Image', dest)
                elif self._state == 'move':
                    transx, transy = x - self._movex, y - self._movey
                    x1, y1 = self._x1 + transx, self._y1 + transy
                    x2, y2 = self._x2 + transx, self._y2 + transy
                    dest = clone_image(self._img)
                    cvRectangle(dest, (x1, y1),
                                (x2, y2), cvScalar(0,0,255), 3)
                    cvCircle(dest, (x1 + cvRound((x2 - x1) / 2),
                                    y1 + cvRound((y2 - y1) / 2)),
                             3, cvScalar(0,255,0), -1)
                    cvShowImage('Image', dest)
            elif event == CV_EVENT_RBUTTONDOWN:
                if self._state == 'none':
                    self._stall = False
                    if config.has_key(self._name):
                        del config[self._name]

                elif self._state == 'select':
                    width = self._x2 - self._x1
                    height = self._y2 - self._y1
                    centerx = self._x1 + cvRound(width / 2)
                    centery = self._y1 + cvRound(height / 2)
                    config[self._name] = { 'x' : centerx,
                                           'y' : centery,
                                           'width' : abs(width),
                                           'height' : abs(height) }
                    self._stall = False
                    self._state = 'none'
            
        cvSetMouseCallback('Image', callback)
        cvShowImage('Image', self._img)
        while self._stall:
            pass
        cvDestroyWindow('Image')

def main():
    if len(sys.argv) < 2:
        print 'Usage: python rectangle.py FOLDER'
        sys.exit(1)

    cvStartWindowThread()
    directory = os.path.expanduser(sys.argv[1])
    filename = raw_input('Save results to what file? ')

    config = {}
    try:
        config = yaml.load(file(filename))
        print 'WARNING: Updating file', os.path.abspath(filename)
    except IOError:
        print 'Creating new file', os.path.abspath(filename)

    files = [os.path.join(directory, x) for x in os.listdir(directory)]
    #cvNamedWindow('Image', 1)
    try:
        for i, f in enumerate(files):
            img = cvLoadImage(f)
            display = Display(os.path.basename(f), img)
            display.execute(config)
            cvReleaseImage(img)
            print '\r%d / %d' % (i+1, len(files)),
            sys.stdout.flush()
    #cvDestroyWindow('Image')
    except:
        pass

    with open(os.path.expanduser(filename), 'w') as fd:
        yaml.dump(config, fd)

if __name__ == '__main__':
    main()
