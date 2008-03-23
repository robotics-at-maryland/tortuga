#!/usr/bin/env python

import sys, traceback, Ice
import ram

status = 0
ic = None
try:
	ic = Ice.initialize(sys.argv)
	base = ic.stringToProxy("MockController:default -p 10000")
	ctrl = ram.control.ControllerPrx.checkedCast(base)
	if not ctrl:
		raise RuntimeError("Invalid proxy")
	
	for i in range(1000):
		print "Setting speed to", i
		ctrl.setSpeed(float(i))
		print "Speed is now", ctrl.getSpeed()
except:
	traceback.print_exc()
	status = 1

if ic:
	# Clean up
	try:
		ic.destroy()
	except:
		traceback.print_exc()
		status = 1

sys.exit(status)
