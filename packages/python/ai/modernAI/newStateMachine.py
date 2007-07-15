class stateMachine():     
	def __init__(self,args = None,interrupts = None):
	    self.changeState(self.startState,args,interFuncs,interStates)
	    self.controllingMachine = self
	    self.complete = False

	def operate(self):    
		if self.controllingMachine == self:
		    state = self.currentState()
		    state(self.stateArgs,self.interrupts)
		    self.checkInterrupt(self.interrupts)
		    return self.complete
		else:
		    complete = self.controllingMachine.operate()
		    if complete == True:
		        self.controllingMachine = self
		        self.changeState(self.reentryFunc)
		    
	def startState(self):
		raise Exception("Start state needs to be implemented")
		
	def currentState(self):
		return self.state
		
	def changeState(self,newState,args = None,interrupts = None):
	    self.state = newState
	    self.stateArgs = args
	    self.interrupts = interrupts
	    
	def setArgs(self,args):
	    self.stateArgs = args
	    
	def checkInterrupt(self,interrupts):
	    i = 0
	    funcs = interrupts.keys()
	    if funcs != None:
	        for func in funcs:
	            if func():
	                self.changeState(interrupts[func])
	            else:
	                i += 1
	            
	def exit(self):
	    self.complete = True
	    print "State machine complete"
	    
	def branch(self,machine,reentryFunc,args):
	    self.controllingMachine = machine
	    machine.setArgs(args)
	    self.reentryFunc = reentryFunc