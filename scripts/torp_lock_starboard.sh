#! /bin/sh

# This script locks the starboard launcher after it has been fully loaded
# This looks very wierd but bugs in MotorBoard r3 cause this specific
# order of commands (Some bugs have been fixed).
# DO NOT USE THIS SCRIPT WHILE THE MAIN SOFTWARE IS RUNNING

echo "Locking Starboard launcher..."
lcdshow -srvpwroff
lcdshow -srvsetpos 6 256
lcdshow -srvenable 64
lcdshow -srvpwron
sleep 2
lcdshow -srvpwroff
echo "Insert starboard launcher pins!"