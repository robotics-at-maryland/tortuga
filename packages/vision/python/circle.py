
import commands
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

class Display(object):
    def __init__(self, name, img):
        self._name = name
        self._img = img
        self._x, self._y, self._r = (0, 0, 0)
        # Possible states: none, move, select, mousedown
        self._state = 'none'

    def execute(self):
        self._stall = True
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
            
        cvSetMouseCallback('Image', callback)
        cvShowImage('Image', self._img)
        while self._stall:
            pass

def main():
    if len(sys.argv) < 2:
        raise Exception('Usage: python circle.py FOLDER')

    cvStartWindowThread()
    directory = os.path.join(sys.argv[1], 'ppm')
    filename = raw_input('Save results to what file? ')

    files = [os.path.join(directory, x) for x in os.listdir(directory)]
    cvNamedWindow('Image')
    for i, f in enumerate(files):
        img = cvLoadImage(f)
        display = Display(os.path.basename(f), img)
        display.execute()
        print '%d / %d' % (i+1, len(files))
    cvDestroyWindow('Image')

    fd = open(os.path.join(sys.argv[1], '..', filename), 'w')
    yaml.dump(config, fd)
    fd.close()

if __name__ == '__main__':
    main()
