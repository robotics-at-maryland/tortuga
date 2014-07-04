import time

#checks if a specified amount of time has passed
#check will return true until duration is exceeded
class timer(object):
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
        self.range = 0

    #should be overloaded to update the values in here
    def update(self):
        pass

    def isSeen(self):
        return self.seen

class ObjectInVisionRangeQuery(object):
    def __init__(self, visionObject, x_center, y_center, range_center, x_range, y_range, range_range):
        self._obj = visionObject
    
    def query(self):
        visionObject.update(self):
        return visionObject.seen && ((abs(visionObject.x - x_center) > x_range) && (math.abs(visionObject.y - y_center) > y_range) && (math.abs(visionObject.range - range_center) > range_range))
        
