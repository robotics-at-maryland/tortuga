import devices

class main_vehicle:
    def __init__(self,vc,mc):
        print "Creating a vehicle"
        #Set the vision Communicator
        self.vision_comm = vc
        self.motor_comm = mc
        
        # Initialize the thruster objects
        t1 = devices.Thruster(0.0,1)
        t2 = devices.Thruster(0.0,2)
        t3 = devices.Thruster(0.0,3)
        t4 = devices.Thruster(0.0,4)
        self.thrusters = [t1,t2,t3,t4]
    
    #sets thruster values, as well as updating the motor controller with the command  
    def set_thruster_power(self,thruster,power):
        thruster.power = power
        print "Thruster " + str(thruster.address) + ": " + str(power)
        self.motor_comm.set_power(thruster)
    
    #prints out the thruster powers in a readable format
    def print_thruster_powers(self):
        for thruster in self.thruster:
            print "Thruster " + str(thruster.address) + ": " + str(thruster.power)
        
    #starts the vision system running along on a thread
    def start_vision_system(self):
        self.vision_comm.start_vision_thread()
        return self.vision_comm.error
    
    #gets the data from the vision system
    def get_vision_structure(self):
        return self.vision_comm.get_data()
    
    def shutdown(self):
        self.motor_comm.close(self.thrusters)
    