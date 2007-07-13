#list of objects that all have an update method that takes a time step

import time

class updatableInformation:
    def __init__(self,object,updateInterval):
        self.object = object
        self.updateInterval = updateInterval
        self.lastUpdated = time.time()
                
class scheduler:
    def __init__(self,objects):
        self.objects = objects
        self.startTime = time.time()
        
    def operate(self):
        while True:
            currTime = time.time()
            for element in self.objects:
                lastUpdated = element.lastUpdated
                if currTime - lastUpdated >= element.updateInterval:
                    #print "Sytem Time:",currTime - self.startTime
                    element.object.update(currTime - lastUpdated)
                    element.lastUpdated = currTime
