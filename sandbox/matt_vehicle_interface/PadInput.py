import curses,motor_communicator,os

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

u1 = 119
d1 = 115
u2 = 97
d2 = 100
u3 = 105
d3 = 107
u4 = 106
d4 = 108

m1 = 0
m2 = 0
m3 = 0
m4 = 0

quit = 0

while(quit != 1):
    c = stdscr.getch()
    print "\n"
    
    if c == 119:
        m1 += increment
    if c == 115:
        m1 -= increment
    if c == 97:
        m2 -= increment
    if c == 100:
        m2 += increment
    if c == 105:
        m3 += increment
    if c == 107:
        m3 -= increment
    if c == 106:
        m4 -= increment
    if c == 108:
        m4 += increment
    if c == 27:
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


    