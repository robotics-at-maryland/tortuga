# Device Creation Classes
class DeviceFactory(object):
    """
    Break out into a common base class, or meta class?
    """
    createFunc = {};

    @staticmethod    
    def create(type, args):
        return DeviceFactory.createFunc[type](args)
    
class Device(object):
    """
    Represents a physical object on the vehicle like thrusters, sensors,
    actuators, etc.
    """
    
    def update(self, timestep):
        """
        Update the device, the timestep value may not be needed
        """
        pass

    def startUpdate(self):
        """
        Start automatic update (this would utilize threads and locking to keep
        the vehicle up to date)
        """
        pass