class state_machine:
        
    def __init__(self,environment):
        print "Initializing the state machine"
        self.state = "initializing"             #set the start state
        self.define_state_table()
        self.environment = environment
        self.vehicle = self.environment.vehicle
    def vehicle(self):
        return self.environment.vehicle
    def time(self):
        return self.environment.timer.time()
        
    '''
    Defines the state table, where state names are mapped to operation functions.
    '''
    def define_state_table(self):
        self.state_table = {"initializing":self.initializing,"starting":self.starting,"halting":self.halting,"operating":self.operating}
    
    def operate(self):
        function = self.state_table[self.state]
        function()
        return self.state
            
    def change_state(self,new_state):
        self.state = new_state
        
    def initializing(self):
        print "still initializing..."
        self.change_state("starting")
        
    def starting(self):
        print "starting the robot..."
        error = self.vehicle.start_vision_system()
        if error:
            self.change_state("halting")
        else:
            self.change_state("operating")

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
        self.change_state("finished")