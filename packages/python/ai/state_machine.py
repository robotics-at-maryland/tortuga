import time,key_controller




class state_machine:
    '''
    This is the state machine, the guts of the A.I of the system. A state machine has states, which are strings.
    States map to functions. First, the start state is initialized. Then, the function that
    this state maps to is executed. At the end of this function, the change_state function is called, changing
    the state to another mapped state. This state is then executed. This cycle continues until the state
    "finished" is reached.
    '''
        
    def __init__(self,environment):
        print "Initializing the state machine"
        self.state = "initializing"             #set the start state
        self.define_state_table()               #load the state table
        self.environment = environment
        self.vehicle = self.environment.vehicle
    '''
    An accessor for the vehicle, just a shortcut
    '''
    def vehicle(self):
        return self.environment.vehicle
    '''
    A simple way to get the current operating time from the clock
    '''
    def time(self):
        return self.environment.timer.time() 
    
    '''
    Defines the state table, where state names are mapped to operation functions.
    '''
    def define_state_table(self):
        self.state_table = {"initializing":self.initializing,"starting":self.starting,"halting":self.halting,"operating":self.operating,"testing thrusters":self.testing_thrusters,"curses operation":self.curses_operation}
    
    '''
    This is the bulk of state machine operation. Function mapped to by current state is fetched
    from the state_table dictionary. Next, the function is executed. Finally, the new state 
    (whatever was changed into) is returned.
    '''
    def operate(self):
        function = self.state_table[self.state]
        function()
        return self.state
            
    '''
    Simple function that 
    '''
    def change_state(self,new_state):
        self.state = new_state
        
    def initializing(self):
        print "still initializing..."
        self.change_state("starting")
        
    def starting(self):
        print "starting the robot..."
        error = self.vehicle.start_vision_system()
        if error:
            self.change_state("curses operation")
        else:
            self.change_state("operating")
            
    def curses_operation(self):
        window = key_controller.CursesController(self.vehicle)
        window.run()
        self.change_state("halting")
        
    def testing_thrusters(self):
        for thruster in self.vehicle.thrusters:
            print "Cycling Thruster " + str(thruster.address)
            for i in range(0,10,1):
                pow = i/10.0
                self.vehicle.set_thruster_power(thruster,pow)
            for i in range(10,-10,-1):
                pow = i/10.0
                self.vehicle.set_thruster_power(thruster,pow)
            for i in range(-10,0,1):
                pow = i/10.0
                self.vehicle.set_thruster_power(thruster,pow)
        self.change_state("halting")
        
    def operating(self):
        print "operating the robot..."
        vs = self.vehicle.get_vision_structure()
        print "Frame Num:",vs.frameNum
        print "Width:",vs.width
        print "Height:",vs.height
        print "Red Light Location: (",vs.redLightx,",",vs.redLighty,")"
        print "Distance From Vertical:",vs.distFromVertical
        print "Angle:",vs.angle
        print "Bin Location: (",binx,",",biny,")"
        print "Light Visible:",vs.lightVisible
        print "Pipe Visible:",vs.pipeVisible
        print "Bin Visible",vs.binVisible
        self.change_state("operating")
        
    def halting(self):
        print "halting robot operation..."
        self.vehicle.shutdown()
        self.change_state("finished")