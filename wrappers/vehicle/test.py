import os
import sys

sys.path.insert(0, os.environ['RAM_SVN_DIR'] + '/build/lib')

#print sys.path
import ext.vehicle as Vehicle
import ext.vehicle_device as Device
#import ext.vehicle.vehicle as Vehicle

veh = Vehicle.Vehicle()
star = Device.Thruster.construct("Starboard", "01", 0.9)
veh._addDevice(star)
