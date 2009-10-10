#!/usr/bin/env python -i

import sys, traceback, Ice
import atexit

sys.path.append('build')

import SensorBoard

status = 0
ic = None

def ice_cleanup():
    print('Cleaning up Ice client runtime.')
    if ic:
        # Clean up
        try:
            ic.destroy()
        except:
            traceback.print_exc()
            status = 1
atexit.register(ice_cleanup)

try:
    print 'Loading Ice client runtime...'
    ic = Ice.initialize(sys.argv)
    base = ic.stringToProxy("SensorBoard:default -p 10000")
    sb = ram.tortuga.SensorBoard.checkedCast(base)
    if not factory:
        raise RuntimeError("Invalid proxy")
except:
    traceback.print_exc()
    status = 1
    print('Failed to load Ice client runtime.  Quitting now.')
    sys.exit(status)
print "A SensorBoard is now available as the global variable 'sb'."
