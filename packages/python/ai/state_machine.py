import time,key_controller
import core
            
class state_machine:
    '''
    This is the state machine, the guts of the A.I of the system. A state machine has states, which are strings.
    States map to functions. First, the start state is initialized. Then, the function that
    this state maps to is executed. At the end of this function, the change_state function is called, changing
    the state to another mapped state. This state is then executed. This cycle continues until the state
    "finished" is reached.
    '''
        
    def __init__(self,vehicle):
        print "Initializing the state machine"
        self.state = "initializing"             #set the start state
        self.define_state_table()               #load the state table
 
    '''
    Defines the state table, where state names are mapped to operation functions.
    '''
    def define_state_table(self):
        self.state_table = {"initializing":self.initializing,"starting":self.starting,"halting":self.halting,"operating":self.operating,"testing thrusters":self.testing_thrusters,"curses operation":self.curses_operation}
    
    def set_states(self,states):
        self.state_table = states
    
    '''
    This is the bulk of state machine operation. Function mapped to by current state is fetched
    from the state_table dictionary. Next, the function is executed. Finally, the new state 
    (whatever was changed into) is returned.
    '''
    def operate(self):
        try:
            function = self.state_table[self.state]
            function()
        except KeyError:
            print "No such state "
            print "You were trying to run" 
            print self.state
        return self.state
            
	'''
	simple function that changes state of the state machine
	'''
    def change_state(self,new_state):
        self.state = new_state
        
    
