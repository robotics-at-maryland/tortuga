#! /bin/sh

# This script locks the port torpedo launcher for loading
# It currentlt works around bugs in MotorBoard r3
# DO NOT USE WHILE THE VEHICLE IS RUNNING

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
echo "Insert port launcher pin!"
