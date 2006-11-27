# Vehicle Creation Classes
class VehicleFactory(object):
    """
    Break out into a common base class, or meta class?
    """
    createFunc = {};

    @staticmethod    
    def create(type, args):
        return VehicleFactory.createFunc[type](args)

# Definition of the vehicle interface
class Vehicle(object):
    """
    Needed properties: Heading, Depth, possibly some accelerations,
    incorperate other vehicle models and plans here.
    
    Notice the lack of methods like "addXXX" and "setXXX" the internals of the
    vehicle will be very protected.  Generally a device will be turned "on",
    ie told to update, and it will set the proper data in the vehicle.
    """
    
    def update(self, timestep):
        """
        Update the vehicle for a certain timestep (uses non threaded 
        syncrounous operation)
        """
        pass

    def startUpdate(self):
        """
        Start automatic update (this would utilize threads and locking to keep
        everything up to date)
        """
        pass

    def getDevice(self, name):
        """
        """
        pass