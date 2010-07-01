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
import top.mem as mem

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
    parser.add_option('-q', dest='quiet', action='store_false',
                      default=True, help='suppress log file')
    parser.add_option('-f', '--file', dest='filename', default=None,
                       help='write to file FILE', metavar='FILE')
    parser.add_option('-v', dest='verbose', action='store_true',
                      help='verbose mode')
    parser.add_option('-s', '--size', dest='size', default=1, type='int',
                      help='size of the averaging filter SIZE '
                      '[default: %default]', metavar='SIZE')
    parser.add_option('-t', '--types', dest='types', default='user,sys,idle',
                      help='options: user, nice, sys, '
                      'idle, iowait, irq, sirq, all [default: %default]',
                      type='string', metavar='TYPES')
    parser.add_option('--meminfo', dest='meminfo', action='store_true',
                      default=False, help='Run memory analysis tool')

    (options, args) = parser.parse_args()

    # Setting print mode
    printfunc = vprint(options.verbose)
    printfunc("Setting print mode to verbose")

    if options.meminfo:
        printfunc('Running meminfo')
        mem.analyze()
        return

    printfunc("Monitoring cpu types %s" % options.types)
    type_locs = set()
    types = options.types.split(',')
    for t in types:
        if t == 'all':
            type_locs = set(range(7))
            break

        num = parse_name(t)
        if num < 0:
            print "ERROR: Not a valid cpu type %s" % t
            return
        else:
            type_locs.add(num)

    if options.filename is not None:
        printfunc("File will be logged to %s" % options.filename)
    else:
        printfunc("File will be logged in default location")

    size = options.size
    # options.quiet will be false when log output should be suppressed
    if not options.quiet:
        printfunc("Log file output will be suppressed")

    printfunc("Recording data buffer is size %d" % size)

    printfunc("Starting main program")
    
    top.AnalyzeComputer(log_name = options.filename,
                        suppress = options.quiet,
                        size = size,
                        type_locs = type_locs)

if __name__ == '__main__':
    main()
