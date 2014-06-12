#! /bin/sh

# This script opens the starboard torpedo launchers for loading
# It currently works around bugs in MotorBoard r3.

# DO NOT USE WHILE THE VEHICLE IS RUNNING

echo "Opening Starboard ..."
lcdshow -srvpwroff
lcdshow -srvsetpos 6 4000
lcdshow -srvenable 64
lcdshow -srvpwron
sleep 2
lcdshow -srvpwroff

echo "Load Starboard Torpedo!"
