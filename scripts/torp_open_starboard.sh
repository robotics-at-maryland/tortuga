#! /bin/bash

# This script opens the starboard torpedo launcher for loading
# It currentlt works around bugs in MotorBoard r3
# DO NOT USE WHILE THE VEHICLE IS RUNNING

echo "Wait to load (Starboard) ..."
lcdshow -srvpwroff
sleep 1
lcdshow -srvsetpos 7 4000
sleep 1
lcdshow -srvenable 255
sleep 1
lcdshow -srvpwron
sleep 2
lcdshow -srvpwroff
echo Load starboard torpedo!
