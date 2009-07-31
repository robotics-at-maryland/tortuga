#! /bin/sh

# This script opens the starboard & port torpedo launchers for loading
# It currently works around bugs in MotorBoard r3, it must always do starboard
# then port
# DO NOT USE WHILE THE VEHICLE IS RUNNING

echo "Opening Starboard ..."
lcdshow -srvpwroff
sleep 1
lcdshow -srvsetpos 7 4000
sleep 1
lcdshow -srvenable 255
sleep 1
lcdshow -srvpwron
sleep 2
lcdshow -srvpwroff

echo "Opening Port ..."
lcdshow -srvpwroff
sleep 1
lcdshow -srvsetpos 7 256
sleep 1
lcdshow -srvenable 192
sleep 1
lcdshow -srvpwron
sleep 2
lcdshow -srvpwroff

echo "Load torpedos!"
