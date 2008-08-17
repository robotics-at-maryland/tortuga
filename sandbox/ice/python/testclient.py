#!/usr/bin/env python

import sys, traceback, Ice
import ram

status = 0
ic = None
try:
	ic = Ice.initialize(sys.argv)
	base = ic.stringToProxy("factory:default -p 10000")
	factory = ram.vehicle.IVehicleFactoryPrx.checkedCast(base)
	if not factory:
		raise RuntimeError("Invalid proxy")
	
	try:
		vehicle1 = factory.getVehicleByName("The Yellow Submarine")
	except:
		traceback.print_exc()
	try:
		vehicle2 = factory.getVehicleByName("The Blue Submarine")
	except:
		traceback.print_exc()
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
