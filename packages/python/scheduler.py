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
        
    def busyOperate(self):
        while True:
            currTime = time.time()
            for element in self.objects:
                lastUpdated = element.lastUpdated
                if currTime - lastUpdated >= element.updateInterval:
                    #print "Sytem Time:",currTime - self.startTime
                    element.object.update(currTime - lastUpdated)
                    element.lastUpdated = currTime
                    
    def nextUp(self):
        currTime = time.time()
        waits = []
        for element in self.objects:
            lastUpdated = element.lastUpdated
            updateInterval = element.updateInterval
            timeTill = updateInterval - (currTime - lastUpdated)
            waitInfo = (element,timeTill)
            waits.append(waitInfo)
        minWait = 9999
        minElement = None
        for element in waits:
            obj = element[0]
            t = element[1]
            if t < minWait:
                minWait = t
                minElement = obj
        return (minElement,minWait)
    
    def sleepOperate(self):
        while True:
            next = self.nextUp()
            element = next[0]
            sleepTime = next[1]
            if sleepTime < 0:
                time.sleep(0)
            else:
                time.sleep(sleepTime)
            currTime = time.time()
            print "Sytem Time:",currTime - self.startTime
            element.object.update(currTime - element.lastUpdated)
            element.lastUpdated = currTime