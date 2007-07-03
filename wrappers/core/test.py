import os
import sys

sys.path.insert(0, os.environ['RAM_SVN_DIR'] + '/build/lib')

#print sys.path
import ext.vehicle as Vehicle
import ext.vehicle_device as Device
#import ext.vehicle.vehicle as Vehicle

veh = Vehicle.Vehicle()
star = Device.Thruster.pyconstruct(veh, {'name' : 'Star', 'address' : '01',
                                         'calibration_factor' : 0.4})
#star.test = "TestValue"
veh._addDevice(star)

print "Trying pointers"
print "Py:",star
print "Veh:",veh.getDevice("Starboard")

#print "Trying props"
#print star.test
#print veh.getDevice("Starboard").test
