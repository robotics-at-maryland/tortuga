# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File:  packages/vision/python/buoy.py
#
# Requirements:
#   - Must have opencv python bindings
#      - Package list:
#         - python-opencv
#
# This file is a test of finding a circle's horizontal width at
# any given radius. If you draw a line from the radius of the circle
# directly upwards, the resulting horizontal line would be approximately 0.
# Now if the radius is 5 pixels, what is the horizontal width at 2 pixels?
#
# We know that we can find another point given a distance and angle
# (r and theta) using:
#     y1 = r * sin(theta) + y0
# and
#     x1 = r * cos(theta) + x0
#
# We know y1, y0, r, and x0. y0 and x0 correspond to the center of the circle,
# r corresponds to the radius. y1 corresponds to how long our line directly
# upward is drawn (as we're trying to find the horizontal lines).
#
# We can solve the top equation for theta:
#    theta = asin( (y1 - y0) / r )
# Then solve for x1 (which is half the width of the horizontal line) using
# the above equation.
#
# Below is a proof of concept that it works for finding half the width.
# It draws 1/4 of the circle using this logic.

from __future__ import division

import math

from opencv.cv import *
from opencv.highgui import *

def main():
    # Radius of the circle
    r = 480

    # Test image for display, perfect square a little bit bigger than the circle
    # Occassionally produces a number 1 greater than the radius of the circle
    img = cvCreateImage(cvSize(r+1, r+1), 8, 3)
    cvSetZero(img)
    
    for y in xrange(r):
        # y0 is considered r for this exercies
        # (we want the bottom left to be the center)
        theta = math.asin( (y - r) / r )
        # x0 is zero for this exercise
        x1 = cvRound(r * math.cos(theta) + 0)

        # Record on the image
        img[y, x1] = [255, 0, 0]

    cvStartWindowThread()
    cvNamedWindow('Image')
    cvShowImage('Image', img)
    cvWaitKey(0)

    cvDestroyWindow('Image')

if __name__ == '__main__':
    main()
