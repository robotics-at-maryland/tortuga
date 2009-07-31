#! /bin/sh

# This script opens the port torpedo launcher for loading
# It currentlt works around bugs in MotorBoard r3
# DO NOT USE WHILE THE VEHICLE IS RUNNING

echo "Wait to load (Port) ..."
lcdshow -srvpwroff
sleep 1
lcdshow -srvsetpos 7 256
sleep 1
lcdshow -srvenable 192
sleep 1
lcdshow -srvpwron
sleep 2
lcdshow -srvpwroff
echo "Load port torpedo!"
