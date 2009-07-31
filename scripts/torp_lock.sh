#! /bin/sh

# This script locks both launchers after they have been fully loaded
# This looks very wierd but bugs in MotorBoard r3 cause this specific
# order of commands. Always starboard then port.
# DO NOT USE THIS SCRIPT WHILE THE MAIN SOFTWARE IS RUNNING

echo "Locking Starboard launcher..."
lcdshow -srvpwroff
sleep 1
lcdshow -srvsetpos 7 256
sleep 1
lcdshow -srvenable 255
sleep 1
lcdshow -srvpwron
sleep 2
lcdshow -srvpwroff

echo "Locking Port launcher..."
lcdshow -srvpwroff
sleep 1
lcdshow -srvsetpos 7 4000
sleep 1
lcdshow -srvenable 192
sleep 1
lcdshow -srvpwron
sleep 2
lcdshow -srvpwroff

echo "Insert launcher pins!"
