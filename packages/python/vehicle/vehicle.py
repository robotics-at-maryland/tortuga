import devices
'''
This is the vehicle abstraction. A vehicle has devices, things like thrusters and sensors.
All access to the devices occurs through the vehicle. Functions that access devices are written 
as accessors in the vehicle class (no direct access to devices from state machine)
'''


class main_vehicle:
    def __init__(self,vc,mc,sc):
        print "Creating a vehicle"
        #Set the vision Communicator
        self.vision_comm = vc
        self.motor_comm = mc
        self.sensor_comm = sc
        
        # Initialize the thruster objects
        t1 = devices.Thruster(0.0,1)
        t2 = devices.Thruster(0.0,2)
        t3 = devices.Thruster(0.0,3)
        t4 = devices.Thruster(0.0,4)
        self.thrusters = [t1,t2,t3,t4]
        
        #Create a depth sensor object
        self.depth_sensor = devices.DepthSensor(self.sensor_comm)
    
    #sets thruster values, as well as updating the motor controller with the command  
    #thruster is an actual thruster object, not just an address number
    def set_thruster_power(self,thruster,power):
        thruster.power = power
        self.motor_comm.set_power(thruster.address,thruster.power)
    
    #updates the motor communicator with the power setting for the thruster device.
    #if for some reason the "code" thruster got a power setting that the motor communicator
    #didn't send to the motor controllers, this function does the job
    def update_thruster(self,thruster):
	self.motor_comm.set_power(thruster.address,thruster.power)

    #prints out the thruster powers in a readable format
    def print_thruster_powers(self):
        for thruster in self.thruster:
            print "Thruster " + str(thruster.address) + ": " + str(thruster.power)
            
    #gets the depth from the depth sensor
    def depth(self):
        return self.depth_sensor.depth()
        
    #starts the vision system running along on a thread
    def start_vision_system(self):
        self.vision_comm.start_vision_thread()
        return self.vision_comm.error
    
    #gets the data from the vision system
    def get_vision_structure(self):
        return self.vision_comm.get_data()
    
    #this is the vehicle shutdown routine. Put code in here that stops robot operation cleanly
    def shutdown(self):
        self.motor_comm.close(self.thrusters)
    