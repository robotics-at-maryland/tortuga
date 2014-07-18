import time
import ext.vision as vision
import subprocess as subprocess

#checks if a specified amount of time has passed
#check will return true until duration is exceeded
class Timer(object):
    def __init__(self, duration):
        self.reset()
        self._duration = duration

    def reset(self):
        self._start = time.time()
    
    def check(self):
        return (self._duration > (time.time() - self._start))

class VisionObject(object):
    def __init__(self):
        self.seen = False
        self.x = 0
        self.y = 0
        self.angle = 0
        self.range = 0

    #should be overloaded to update the values in here
    def update(self):
        pass

    def isSeen(self):
        return self.seen

#hack vision object that  tracks a Red buoy in the old simulator
class OldSimulatorHackVisionObject(VisionObject):
    def __init__(self,oldStatePtr):
        super(OldSimulatorHackVisionObject,self).__init__()
        oldStatePtr.queuedEventHub.subscribeToType(vision.EventType.BUOY_FOUND,self.callback)
        oldStatePtr.queuedEventHub.subscribeToType(vision.EventType.BUOY_LOST,self.seeit)


    def callback(self,event):
        if(event.color == vision.Color.RED):
            self.seen = True
            self.x = event.x
            self.y = event.y
            self.range = 1.0/event.range
            
    def seeit(self,event):
        if(event.color == vision.Color.RED):
            self.seen = False

#hack vision object that  tracks a pipe in the old simulator
class OldSimulatorHackPipe(VisionObject):
    def __init__(self,oldStatePtr):
        super(OldSimulatorHackPipe,self).__init__()
        oldStatePtr.queuedEventHub.subscribeToType(vision.EventType.PIPE_FOUND,self.callback)
        oldStatePtr.queuedEventHub.subscribeToType(vision.EventType.PIPE_LOST,self.seeit)


    def callback(self,event):
        self.seen = True
        self.x = event.x
        self.y = event.y
        self.angle = event.angle.valueDegrees()
            
    def seeit(self,event):
        self.seen = False
        
#checks if a vision object is in the range specified
class ObjectInVisionRangeQuery(object):
    def __init__(self, visionObject, x_center, y_center, range_center, x_range, y_range, range_range):
        self._obj = visionObject
        self._x_center = x_center
        self._y_center = y_center
        self._range_center = range_center
        self._x_range = x_range
        self._y_range = y_range
        self._range_range = range_range
    
    def query(self):
        self._obj.update()
        obj = self._obj
        return self._obj.seen and ((abs(obj.x - self._x_center) <= self._x_range) and (abs(obj.y - self._y_center) <= self._y_range) and (abs(obj.range - self._range_center) <= self._range_range))
        

#this class transforms a query into a  query which only becomes false if the the query has only returned false under a certain amount of time(such that all queries made in that time returned true, does not account for queries that weren't actually made)
#this effectively introduces a delay into a query, it is useful for dealing with things like vision where an object might disappear for 1 or 2 frames, but then reappear immediately afterwards
#TLDR; this query returns if "query" was true in the last "timeout" seconds, as long as you call it continously
class hasQueryBeenFalse(object):
    def __init__(self, timeout, query):
        self._timer(timeout)

    def query(self):
        #if query true, reset timer and return true
        if(query()):
            self._timer.reset()
            return True
        else:
            #if query false return if the timer has triggered yet
            return self._timer.check()

#connects to the blackfin and provides control over it
class SonarSession(object):
    def __init__(self):
        self.connect()

    def __del__(self):
        self.exit()
    #telnets the subprocess into to blackfin
    #Note that blackfin must be turned on
    #returns zero if successful
    def connect(self):
        return subprocess.call('telnet 192.168.10.19')+subprocess.call('export LD_LIBRARY_PATH=/card/sonar12_2/')
    
    #this command kills the sonar
    #this is needed to change frequency
    #this kills all instances of sonard
    def killSonar(self):
        return subprocess.call('kill -15 $(pidof sonard)')
    
    #exits the telnet session
    def exit(self):
        return subprocess.call('^] close')
    
    #sets the sonar frequency, freq1 is desired, freq2 is frequency to ignore
    #will attempt to kill sonard first in case its running
    def setSonarFreq(self,freq1,freq2):
        self.killSonar()
        return subprocess.call('/card/sonar12_2/sonard'+str(freq1)+str(freq2)+ '&')

        
        
