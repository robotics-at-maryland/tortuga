import os
import sys

sys.path.insert(0, os.environ['RAM_SVN_DIR'] + '/build/lib')

#print sys.path
from ext.core import ConfigNode
from ext.pattern import CachedObserver
import ext.vehicle as Vehicle
import ext.vehicle_device as Device
#import ext.vehicle.vehicle as Vehicle

veh = Vehicle.Vehicle()
cfg = ConfigNode.fromString(str({'name' : 'Star', 'address' : '01',
                                       'calibration_factor' : 0.4}))
star = Device.Thruster.construct(veh, cfg)
#star.test = "TestValue"
veh._addDevice(star)

print "Trying pointers"
print "Py:",star
print "Veh:",veh.getDevice("Star")


class ThrusterObserver(CachedObserver):
    def __init__(self):
        CachedObserver.__init__(self, True)

    def cachedUpdate(self, subj, flag):
        if flag == Device.Thruster.FORCE_UPDATE:
            print 'Force update from',subj

# Test Observers
obs = ThrusterObserver()
star.addObserver(obs)

print 'Sending commands'
star.setForce(4)

print 'Release updates'
obs.releaseAllUpdates()

print 'Trying again'
obs.releaseAllUpdates()

#print "Trying props"
#print star.test
#print veh.getDevice("Starboard").test
