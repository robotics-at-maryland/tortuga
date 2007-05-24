class state_machine:
        
    def __init__(self,environment):
        print "Initializing the state machine"
        self.state = "initializing"             #set the start state
        self.define_state_table()
        self.environemnt = environment
        
    '''
    Defines the state table, where state names are mapped to operation functions.
    '''
    def define_state_table(self):
        self.state_table = {"initializing":self.initializing,"starting":self.starting,"halting":self.halting}
    
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
        self.change_state("halting")
        
    def halting(self):
        print "halting robot operation..."
        self.change_state("finished")