# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File:  packages/vision/python/convert.py
#
# Requirements:
#   - Must have a stock folder within the specified folder
#   - Within this folder, it must have another folder with name FORMAT
#   - Must be in a Unix type system (for commands module)

import commands
import os
import sys

def main():
    if len(sys.argv) < 3:
        print "Usage: python convert.py FOLDER FORMAT"
        sys.exit(1)

    dirname = sys.argv[1]
    format = sys.argv[2]
    stock = os.path.join(dirname, 'stock')
    files = os.listdir(stock)
    for f in files:
        command = 'convert ' + os.path.join(stock, f) + ' ' + \
            os.path.join(dirname, format, os.path.splitext(f)[0] + '.' + format)
        print command,
        if commands.getstatusoutput(command)[0] == 0:
            print 'OK!'
        else:
            print 'Failed'

if __name__ == '__main__':
    main()
