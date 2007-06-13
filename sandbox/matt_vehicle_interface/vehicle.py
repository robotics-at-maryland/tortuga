import devices

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
    def set_thruster_power(self,thruster,power):
        thruster.power = power
        print "Thruster " + str(thruster.address) + ": " + str(power)
        self.motor_comm.set_power(thruster)
    
    #updates the thruster with whatever its settings are in real life
    def update_thruster(self,thruster):
	self.motor_comm.set_power(thruster)

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
    
    def shutdown(self):
        self.motor_comm.close(self.thrusters)
    