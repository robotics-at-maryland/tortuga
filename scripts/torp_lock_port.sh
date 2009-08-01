#! /bin/sh

# This script locks both launchers after they have been fully loaded
# This looks very wierd but bugs in MotorBoard r3 cause this specific
# order of commands.

# DO NOT USE THIS SCRIPT WHILE THE MAIN SOFTWARE IS RUNNING

echo "Locking Port launcher..."
lcdshow -srvpwroff
lcdshow -srvsetpos 5 4000
lcdshow -srvenable 32
lcdshow -srvpwron
sleep 2
lcdshow -srvpwroff

echo "Insert port launcher pin!"
