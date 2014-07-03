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
