#!/usr/bin/env python -i

import sys, os
sys.path.append('build')
sys.path.append('/opt/local/lib/python2.6/site-packages')
import traceback, Ice, ram
import atexit

if len(sys.argv) != 2:
	print 'Usage: client.py hostname'
	os._exit(1)


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
    base = ic.stringToProxy("SensorBoard:default -h %s -p 10000" % sys.argv[1])
    sb = ram.tortuga.SensorBoardPrx.checkedCast(base)
    if not sb:
        raise RuntimeError("Invalid proxy")
except:
    traceback.print_exc()
    status = 1
    print('Failed to load Ice client runtime.  Quitting now.')
    os._exit(status)
print "A SensorBoard is now available as the global variable 'sb'."
