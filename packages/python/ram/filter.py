# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# File:  filter.py


class RingBuffer(object):
    def __init__(self, size):
        self.data = [None for i in xrange(size)]

    def append(self, x):
        self.data.pop(0)
        self.data.append(x)

    def get(self):
        return self.data
    
class MovingAverageFilter(RingBuffer):
    def __init__(self, size, default = 0.0):
        RingBuffer.__init__(self, size)
        
        # Fill buffer with default values
        for i in range(0, size):
            self.append(default)
            
    def getAverage(self):
        data = self.get()
        return sum(data) / float(len(data))
