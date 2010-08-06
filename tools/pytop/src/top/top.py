# Copyright (C) 2010 Maryland Robotics Club
# Copyright (C) 2010 Jonathan Sternberg <jsternbe@umd.edu>
# All rights reserved.
#
# Author: Jonathan Sternberg <jsternbe@umd.edu>
# pyTop - A simple TOP program implement in python using /proc/stat

# STD Imports
import sys
import time
import signal

# Project Imports
import ext.core as core
import ram.monitor

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

def AnalyzeComputer(log_name, suppress, size, type_locs):
    data = [0]*7
    type_index = type_locs

    def numToCpuType(num):
        return { 0 : 'User', 1 : 'Nice', 2 : 'Sys', 3 : 'Idle',
                 4 : 'IOWait', 5 : 'IRQ', 6 : 'SIRQ' }.get(num, 'Invalid')

    def _onUser(event):
        data[0] = event.number

    def _onNice(event):
        data[1] = event.number

    def _onSys(event):
        data[2] = event.number

    def _onIdle(event):
        data[3] = event.number

    def _onIowt(event):
        data[4] = event.number

    def _onIrq(event):
        data[5] = event.number

    def _onSirq(event):
        data[6] = event.number

    def rewrite():
        output = ''
        for l in type_index:
            output += numToCpuType(l) + ': %%%5.2f ' % data[l]
        print '\r', output,
        sys.stdout.flush()

    print 'PyTop, a CPU analysis program (Crtl+C to exit)'

    # Create subsystems
    eventHub = core.EventHub()
    qeventHub = core.QueuedEventHub(eventHub)

    cfg = {
        'CpuMonitor' : {
            'type' : 'CpuMonitor',
            'depends_on' : '[ "QueuedEventHub" ]',
            'log_results' : suppress,
            'bufferSize' : size
            }
        }
    if log_name is not None:
        cfg['CpuMonitor']['log_name'] = log_name
    monitor = ram.monitor.CpuMonitor(cfg = cfg.get('CpuMonitor'),
                                     deps = [ qeventHub ])

    # Register events we are listening to
    connections = []
    eventTypes = [ ram.monitor.CpuMonitor.USER_UPDATE,
                   ram.monitor.CpuMonitor.NICE_UPDATE,
                   ram.monitor.CpuMonitor.SYS_UPDATE,
                   ram.monitor.CpuMonitor.IDLE_UPDATE,
                   ram.monitor.CpuMonitor.IOWT_UPDATE,
                   ram.monitor.CpuMonitor.IRQ_UPDATE,
                   ram.monitor.CpuMonitor.SIRQ_UPDATE ]
    eventFuncs = [ _onUser, _onNice, _onSys, _onIdle,
                   _onIowt, _onIrq, _onSirq ]

    for index in type_index:
        conn = qeventHub.subscribeToType(eventTypes[index], eventFuncs[index])
        connections.append(conn)

    start = time.time()
    while True:
        try:
            # Allow cpu time to pass
            time.sleep(1.0 / size)
            end = time.time()

            # Update monitor and publish the events
            monitor.update(end - start)
            qeventHub.publishEvents()
            rewrite()
            start = end
        except KeyboardInterrupt:
            break

    # Unbackground monitor and close connections
    monitor.unbackground()

    for conn in connections:
        conn.disconnect()
