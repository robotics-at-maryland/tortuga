
import sys
import os
import time
import yaml

from opencv.cv import *
from opencv.highgui import *

def main():
    if len(sys.argv) < 2:
        print 'Usage:', sys.argv[0], 'RESULTS_FILE'
        sys.exit(0)

    results = yaml.load(file(sys.argv[1]))
    hist = [0]*256

    for k in results.itervalues():
        if k['found']:
            hist[k['hist'][0]] += 1

    # Display histogram
    max_value = max(hist)
    print 'Max value:', max_value
    img = cvCreateImage(cvSize(256, 256), 8, 1)
    cvSetZero(img)
    for x in xrange(256):
        scale = hist[x]*255 / max_value
        cvLine(img, (x, 255), (x, 255-scale), cvScalar(255))

    cvStartWindowThread()
    cvNamedWindow('Histogram')
    cvShowImage('Histogram', img)
    cvWaitKey(0)

    cvDestroyWindow('Histogram')

if __name__ == '__main__':
    main()
