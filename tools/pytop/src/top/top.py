# Copyright (C) 2008 Maryland Robotics Club
# Copyright (C) 2008 Joseph Lisee <jlisee@umd.edu>
# All rights reserved.
#
# Author: Joseph Lisee <jlisee@umd.edu>
# sTop - A simple TOP program implement in python using /proc/stat
#
# Modified by: Jonathan Sternberg <jsternbe@umd.edu>
# Averages this data and saves it to the disk


import sys
import time
import signal

# Here we load the first line of the proc file, which is in the following
# format:
#    name| user |nice| sys | idle |iowt.|irq |sirq|
#    cpu  177897 4821 31738 594897 43928 2178 1004 0
#
# We essentially record these values, sleep and gather the next set.  You can
# compare the total difference between the two sets, with the fractional part
# each section makes up to see where the CPU spent its time while you slept.
#
# The next several lines of the stat file contain CPU specific data, so you can
# check each CPU if desired

def AnalyzeComputer(file, size, type_locs):
    def numToCpuType(num):
        return { 0 : 'User', 1 : 'Nice', 2 : 'Sys', 3 : 'Idle',
                 4 : 'IOWait', 5 : 'IRQ', 6 : 'SIRQ' }.get(num, 'Invalid')

    # class Data(object):
    #     def __init__(self, file, size):
    #         self._size = size
    #         self._file = file
    #         self._data = []

    #         # Only setup the public functions if there is a file to write to
    #         if file is not None:
    #             self.appendData = self._appendData
    #             self.flushData = self._flushData
    #         else:
    #             self.appendData = self._pass
    #             self.flushData = self._pass

    #     def _pass(self, *args, **kwargs):
    #         pass

    #     def _appendData(self, user, sys, idle):
    #         self._data.append((user, sys, idle))

    #     def _averageStats(self):
    #         userAvg, sysAvg, idleAvg = 0, 0, 0
    #         size = len(self._data)
    #         if size == 0:
    #             return userAvg, sysAvg, idleAvg

    #         for (u, s, i) in self._data:
    #             userAvg, sysAvg, idleAvg = userAvg + u,sysAvg + s,idleAvg + i
    #         return (userAvg / size, sysAvg / size, idleAvg / size)

    #     # Flushes data to disk if the amount of data is greater than the size
    #     def _flushData(self, force = False):
    #         if len(self._data) >= self._size or force:
    #             self._file.write('User: %%%5.2f Syst: %%%5.2f Idle: %%%5.2f\n' % self._averageStats())
    #             self._data = []
                
    print 'PyTop, a Simple python written in Top (Crtl+C to exit)'
    #data = Data(file, size)

    while True:
        f = open('/proc/stat')
        start_values = f.readline().split()[1:]
        start_time = time.time()
        f.close()

        # Finishing things off
        try:
            time.sleep(1)
        except KeyboardInterrupt:
            # The user pressed Ctrl+C, lets stop running
            break

        # Read our final set of stats
        f = open('/proc/stat')
        end_values = f.readline().split()[1:]
        end_time = time.time()
        f.close()

        # Determine the difference in ticks between two valeus
        start_sum = 0
        for i in start_values:
            start_sum += int(i)
            
        end_sum = 0
        for i in end_values:
            end_sum += int(i)
    
        total_diff = float(end_sum - start_sum)

        # Find the fractional difference of each section
        
        # Order of elements: user nice system idle
        # user: normal processes executing in user mode
        # nice: niced processes executing in user mode
        # system: processes executing in kernel mode
        # idle: twiddling thumbs 

        output = ''
        for num in type_locs:
            diff = (int(end_values[num]) - int(start_values[num])) * 100
            perc = diff / total_diff
            output += '%s: %%%5.2f ' % (numToCpuType(num), perc)
        
        # Record data
        #data.appendData(userPer, sysPer, idlePer)
        #data.flushData()

        # Move the start of the line, overwrite the old output, and
        # flush stdout so its displayed
        print '\r', output,
        sys.stdout.flush()

    #data.flushData(True)
