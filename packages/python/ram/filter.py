# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  filter.py


class RingBuffer(object):
    def __init__(self, size):
        self.data = []
        self._size = size

    def append(self, x):
        if len(self.data) < self._size:
            self.data.append(x)
        else:
            self.data.pop(0)
            self.data.append(x)

    def get(self):
        return self.data
    
class MovingAverageFilter(RingBuffer):
    def __init__(self, size, default = None):
        RingBuffer.__init__(self, size)
        
        # Fill buffer with default values
        if default is not None:
            for i in xrange(0, size):
                self.append(default)
            
    def getAverage(self):
        data = self.get()
        if len(data) == 0:
            return 0
        else:
            return sum(data) / float(len(data))
