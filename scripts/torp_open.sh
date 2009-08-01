#! /bin/sh

# This script opens the starboard & port torpedo launchers for loading
# It currently works around bugs in MotorBoard r3.

# DO NOT USE WHILE THE VEHICLE IS RUNNING

echo "Opening Starboard ..."
lcdshow -srvpwroff
lcdshow -srvsetpos 6 4000
lcdshow -srvenable 64
lcdshow -srvpwron
sleep 2
lcdshow -srvpwroff

echo "Opening Port ..."
lcdshow -srvpwroff
lcdshow -srvsetpos 5 256
lcdshow -srvenable 32
lcdshow -srvpwron
sleep 2
lcdshow -srvpwroff

echo "Load torpedos!"
