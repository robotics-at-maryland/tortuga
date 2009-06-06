import os
import sys
import warnings

sys.path.insert(0, os.environ['RAM_SVN_DIR'] + '/build/lib')

#print sys.path
import vehicle.vehicle as Vehicle
import vehicle.devices as Device
from ext.core import ConfigNode
from ext.pattern import CachedObserver
from module import ModuleManager

# Create singleton module manager
ModuleManager()

veh = Vehicle.Vehicle({'name' : 'Vehicle'})
cfg = {'name' : 'Star',
       'address' : '01',
       'calibration_factor' : 0.4}
star = Device.Thruster.construct(veh, ConfigNode.fromString(str(cfg)))

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
star.setForce(8)
print 'Count',star.getMotorCount()

print 'Release updates'
obs.releaseAllUpdates()

print 'Trying again'
obs.releaseAllUpdates()

print 'Grabing device'
star = veh.getDevice('Star')
star.setForce(4)

#print "Trying props"
#print star.test
#print veh.getDevice("Starboard").test
