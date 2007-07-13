# Copyright (C) 2007 Maryland Robotics Club
# Copyright (C) 2007 Mathew Baklar <matbak87@umd.edu>
# All rights reserved.
#
# Author: Mathew Baklar <matbak87@umd.edu>
# File:  tools/oci/cli.py

# Python Imports
import sys
import curses
import math
import curses.wrapper

# Project Imports
from core import implements
from module import Module, IModule

W = 119
S = 115
A = 97
D = 100

I = 105
K = 107
J = 108
L = 106

ESC = 27

aft = 0
fore = 1
starboard = 2
port = 3

increment = 0.05
decrement = -0.05

class CLI(Module):
    implements(IModule)
    
    def __init__(self, vehicle, config):
        #print 'CLI Created'
        #self.stdscr = curses.initscr()
        self.vehicle = vehicle
        self.thrusters = [vehicle.getDevice('ForeThruster'),
                          vehicle.getDevice('AftThruster'),
                          vehicle.getDevice('StarboardThruster'),
                          vehicle.getDevice('PortThruster')]
        
        Module.__init__(self, config)
        
           
        
    def start(self):
        #print 'Starting CLI'
        # Dont show keypresses
        #curses.noecho()
        # Allow instantaneous Key Processing    
        #curses.cbreak()
        # Enable the keypad     
        #self.stdscr.keypad(1) 
        
        Module.start(self)
        
    def print_string(self,x,y,str,mode):
        print str
        #self.stdscr.addstr(x,y,str,mode)
        
    def print_thrusters(self,thrusters):
        self.print_string(2,10,"Fore: " + str((thrusters[0].getForce()*100)),
                          curses.A_STANDOUT)
        self.print_string(22,10,"Aft: " + str((thrusters[1].getForce()*100)),
                          curses.A_STANDOUT)
        self.print_string(11,30,"Starboard: " + str((thrusters[2].getForce()*100)),
                          curses.A_STANDOUT)
        self.print_string(11,0,"Port: " + str((thrusters[3].getForce()*100)),
                          curses.A_STANDOUT)
        
    def update(self, timestep):
        #print 'Updated*************'
        quit = 0
        self.print_string(0,0,"Thruster Information",curses.A_UNDERLINE)
        self.print_thrusters(self.thrusters)
        
        #char = self.stdscr.getch()
        #quit = self.handle_input(char)
        
        #depth = self.vehicle.getDepth()
        #self.print_string(8,57,"Depth",curses.A_REVERSE)
        #self.print_string(9,60,str(depth),curses.A_BOLD)            
         
        if quit:
            self.shutdown()
                
    def handle_input(self,c):
        if c == ESC:
            return 1
#        elif c == W:
#            self.thrusters[aft].power_up(increment)
#            self.vehicle.update_thruster(self.thrusters[aft])
#        elif c == S:
#            self.thrusters[aft].power_up(decrement)
#            self.vehicle.update_thruster(self.thrusters[aft])
#        elif c == A:
#            self.thrusters[port].power_up(increment)
#            self.vehicle.update_thruster(self.thrusters[port])
#        elif c == D:
#            self.thrusters[port].power_up(decrement)
#            self.vehicle.update_thruster(self.thrusters[port])
#        elif c == I:
#            self.thrusters[fore].power_up(increment)
#            self.vehicle.update_thruster(self.thrusters[fore])
#        elif c == K:
#            self.thrusters[fore].power_up(decrement)
#            self.vehicle.update_thruster(self.thrusters[fore])
#        elif c == J:
#            self.thrusters[starboard].power_up(increment)
#            self.vehicle.update_thruster(self.thrusters[starboard])
#        elif c == L:
#            self.thrusters[starboard].power_up(decrement)
#            self.vehicle.update_thruster(self.thrusters[starboard])
        return 0
            
    def shutdown(self):
        #curses.nocbreak()
        #self.stdscr.keypad(0)
        #curses.echo()
        #curses.endwin()
        
        Module.shutdown(self)