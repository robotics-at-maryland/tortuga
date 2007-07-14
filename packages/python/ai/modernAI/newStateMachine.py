class stateMachine():     
	def __init__(self,args = None,interFuncs = None,interStates = None):
	    self.changeState(self.startState,args,interFuncs,interStates)
	    self.controllingMachine = self
	    self.complete = False

	def operate(self):    
		if self.controllingMachine == self:
		    state = self.currentState()
		    state(self.stateArgs,self.interFuncs,self.interStates)
		    self.checkInterrupt(self.interFuncs,self.interStates)
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
		
	def changeState(self,newState,args = None,interFuncs = None,interStates = None):
	    self.state = newState
	    self.stateArgs = args
	    self.interFuncs = interFuncs
	    self.interStates = interStates
	    
	def checkInterrupt(self,funcs,states):
	    i = 0
	    if funcs != None:
	        for func in funcs:
	            if func():
	                self.changeState(states[i])
	            else:
	                i += 1
	            
	def exit(self):
	    self.complete = True
	    print "State machine complete"
	    
	def branch(self,machine,reentryFunc):
	    self.controllingMachine = machine
	    self.reentryFunc = reentryFunc