#! /bin/sh

# This script locks the starboard torpedo launcher for loading
# It currentlt works around bugs in MotorBoard r3
# DO NOT USE WHILE THE VEHICLE IS RUNNING

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
echo "Insert starboard launcher pin!"
