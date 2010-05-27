# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File:  packages/vision/python/window.py
#
# Requirements:
#   - Must have opencv python bindings
#      - Package list:
#         - python-opencv
#         - python-yaml
#   - Must have a ~/stock_images/window folder
#   - Within this folder, it must have the folders
#      - ppm
#      - output
#      - processed
#   - Inside the ppm folder must be the images to be tested
#     in ppm format
#   - Must be in a Unix type system (for commands module)

# Use true division everywhere
from __future__ import division

import commands
import math
import os
import sys
import yaml

from opencv.cv import *
from opencv.highgui import *

class Blob(object):
    def __init__(self, centerx, centery, width, height):
        self._centerx, self._centery, self._width, self._height = \
            centerx, centery, width, height

    def __cmp__(self, other):
        mysize = self._width * self._height
        othersize = other._width * other._height

        return mysize - othersize

    def __str__(self):
        return '%d %d %d %d' % (self._centerx, self._centery,
                                self._width, self._height)

def analyze(filename, cfg, output_cfg):
    found, result = False, [0, 0, 0, 0]

    # Load the original image
    original = cvLoadImage(filename)

    # Use color segmentation algorithm using cmd line executable
    output = output_cfg.setdefault(os.path.basename(filename), {})
    outputpath = os.path.join(os.path.dirname(filename), '..', 'output',
                          os.path.basename(filename))
    commands.getstatusoutput(cfg['command'] + ' ' + filename + ' ' + outputpath)

    img = cvLoadImage(outputpath)
    debug = cvCreateImage(cvGetSize(img), 8, 3)
    cvCopy(img, debug)

    blobList = []

    # Find the blobs

    # Sort the list into best matches first
    blobList.sort(reverse = True)
    if len(blobList) > 0:
        # Choose the largest square as the found window
        blob = blobList[0]
        result[0] = blobList._centerx
        result[1] = blobList._centery
        result[2] = blobList._width
        result[3] = blobList._height

    # Save these results
    for i, k in enumerate(blobList):
        output[i] = str(k)

    output['found'] = found

    # Redraw the chosen circle as the found color (unknown is gray)
    if found:
        centerx, centery, width, height = result
        UL = (centerx - cvRound(width/2), centery - cvRound(height/2))
        LR = (centerx + cvRound(width/2), centery + cvRound(height/2))

        # Draw bounding box
        cvRectange(debug, UL, LR, cvScalar(0,0,255))
        # Draw the center point
        cvCircle(debug, (centerx, centery), 3, cvScalar(0,255,0), -1)

    processed = os.path.join(os.path.dirname(filename), '..', 'processed',
                             os.path.basename(filename))
    cvSaveImage(processed, debug)

    return (found, result)

def main():
    if len(sys.argv) < 2:
        raise Exception('Usage: python process.py CONFIG')

    spos, sneg, accuracy, size_acc, fpos, fneg, ptotal, ntotal, correct_color =\
        0, 0, 0, 0, 0, 0, 0, 0, 0

    source = os.path.expanduser(os.path.join('~', 'stock_images', 'window', 'ppm'))
    config = yaml.load(file(sys.argv[1]))
    output = {}

    if not config.has_key('command'):
        raise Exception('No command given in the configuration file')

    files = [os.path.join(source, x) for x in os.listdir(source)]
    files.sort()
    for i, f in enumerate(files):
        print '\r%d / %d' % (i+1, len(files)),
        sys.stdout.flush()
        #print '\rProgress:', i, '/', len(files),
        found, result = analyze(f, config, output)
        basename = os.path.basename(f)
        if config.has_key(basename) and found:
            # Average the distance
            real = config[basename]
            #ssd = (((result[0]*real['x'])**2)/(result[0]+real['x']) + \
            #            ((result[1]*real['y'])**2)/(result[1]+real['y']) + \
            #            ((result[2]*real['r'])**2)/(result[2]+real['r'])) * .5
            ssd = math.sqrt((result[0]-real['x'])**2 + \
                                (result[1]-real['y'])**2)
            size_ssd = math.sqrt((result[2]-real['width'])**2 + \
                                     (result[3]-real['height'])**2)
            accuracy += ssd
            size_acc += size_ssd
            # Success
            spos += 1
            ptotal += 1

            # Check if the correct color was returned
            if real['color'] == result[3]:
                correct_color += 1

        elif config.has_key(basename) and not found:
            # Didn't find the point even though we should have, false negative
            fneg += 1
            ptotal += 1
        elif found and not config.has_key(f):
            # Found something we shouldn't have, false positive
            fpos += 1
            ntotal += 1
        else:
            # No window, no result, success
            sneg += 1
            ntotal += 1

    if ptotal > 0:
        print '\n\nSuccess Positive:',spos,'/',ptotal,\
            '- %.2f %%' % (spos/ptotal*100)
        print 'False Negative:',fneg,'/',ptotal,\
            '- %.2f %%' % (fneg/ptotal*100)
    else:
        print '\n\nSuccess Positive:',spos,'/',ptotal,\
            '- %.2f %%' % 0
        print 'False Negative:',fneg,'/',ptotal,\
            '- %.2f %%' % 0

    if ntotal > 0:
        print 'Success Negative:',sneg,'/',ntotal,\
            '- %.2f %%' % (sneg/ntotal*100)
        print 'False Positive:', fpos, '/', ntotal,\
            '- %.2f %%' % (fpos/ntotal*100)
    else:
        print 'Success Negative:',sneg,'/',ntotal,\
            '- %.2f %%' % 0
        print 'False Positive:', fpos, '/', ntotal,\
            '- %.2f %%' % 0

    if spos > 0:
        print 'Accuracy: %e' % (accuracy / spos)
        print 'Size Accuracy: %e' % (size_acc / spos)
    else:
        print 'Accuracy: %e' % 0
        print 'Size Accuracy: %e' % 0

    with open(config.get('output', 'results.yml'), 'w') as fd:
        yaml.dump(output, fd, indent=4, default_flow_style=False)

if __name__ == '__main__':
    main()
