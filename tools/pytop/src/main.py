# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# File: tools/pytop/src/main.py

# STD Imports
from optparse import OptionParser

# Project Imports
import top.top as top

def vprint(verbose):
    if verbose:
        def handler(text):
            print text
        return handler
    else:
        def handler(text):
            pass
        return handler

# Returns a corresponding number for a cpu type
def parse_name(name):
    if name == 'user':
        return 0
    elif name == 'nice':
        return 1
    elif name == 'sys':
        return 2
    elif name == 'idle':
        return 3
    elif name == 'iowait':
        return 4
    elif name == 'irq':
        return 5
    elif name == 'sirq':
        return 6
    else:
        return -1

def main():
    # Parse options
    parser = OptionParser()
    parser.add_option('-f', dest='filename', default=None, type='string',
                      help='write report to FILE', metavar='FILE')
    parser.add_option('-v', dest='verbose', action='store_true',
                      help='verbose mode')
    parser.add_option('-s', '--size', dest='size', default=1, type='int',
                      help='size of the averaging filter SIZE '
                      '[default: %default]', metavar='SIZE')
    parser.add_option('-t', '--types', dest='types', default='user,sys,idle',
                      help='options: user, nice, sys, idle, iowait, irq, sirq '
                      '[default: %default]', type='string', metavar='TYPES')

    (options, args) = parser.parse_args()

    # Setting print mode
    printfunc = vprint(options.verbose)
    printfunc("Setting print mode to verbose")

    printfunc("Monitoring cpu types %s" % options.types)
    type_locs = set()
    types = options.types.split(',')
    for t in types:
        num = parse_name(t)
        if num < 0:
            print "ERROR: Not a valid cpu type %s" % t
            return
        else:
            type_locs.add(num)

    file = None
    size = options.size
    if options.filename is not None:
        printfunc("Opening file %s in write mode" % options.filename)
        file = open(options.filename, 'w')
        printfunc("Recording data buffer is size %d" % size)
        printfunc("Averaged data will be flushed to the disk")
    else:
        printfunc("No file specified. Data will not be saved to disc.")

    printfunc("Starting main program")
    
    top.AnalyzeComputer(file = file, size = size, type_locs = type_locs)

    if file is not None:
        printfunc("Closing file")
        file.close()

if __name__ == '__main__':
    main()
