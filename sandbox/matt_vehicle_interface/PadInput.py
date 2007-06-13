import curses,motor_communicator,os

W = 119
S = 115
A = 97
D = 100

I = 105
K = 107
J = 108
L = 106

ESC = 27

def set_powers(controller,m1,m2,m3,m4):
    controller.set_power(1, m1*1024)
    controller.set_power(2, m2*1024)
    controller.set_power(3, m3*1024)
    controller.set_power(4, m4*1024)
    
increment = 0.05

file = open("/Users/matbak87/matt.txt",'w')
controller = motor_communicator.communicator()
controller.set_amps_limit()

stdscr = curses.initscr()
curses.echo()
curses.cbreak()
stdscr.keypad(1)

m1 = 0
m2 = 0
m3 = 0
m4 = 0

quit = 0

while(quit != 1):
    c = stdscr.getch()
    print "\n"
    if c == W:
        m1 += increment
    if c == S:
        m1 -= increment
    if c == D:
        m2 -= increment
    if c == A:
        m2 += increment
    if c == I:
        m3 += increment
    if c == K:
        m3 -= increment
    if c == J:
        m4 -= increment
    if c == L:
        m4 += increment
    if c == ESC:
        controller.close()
        quit = 1
        
    
    if m1 > 1.0:
        m1 = 1.0
    if m1 < -1.0:
        m1 = -1.0
    if m2 > 1.0:
        m2 = 1.0
    if m2 < -1.0:
        m2 = -1.0
    if m3 > 1.0:
        m3 = 1.0
    if m3 < -1.0:
        m3 = -1.0
    if m4 > 1.0:
        m4 = 1.0
    if m4 < -1.0:
        m4 = -1.0
        
    set_powers(controller,m1,m2,m3,m4)
    
curses.nocbreak()
stdscr.keypad(0)
curses.echo()
curses.endwin()


    