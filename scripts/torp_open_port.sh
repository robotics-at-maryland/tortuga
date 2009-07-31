# This script opens the port torpedo launcher for loading
# It currentlt works around bugs in MotorBoard r3
# DO NOT USE WHILE THE VEHICLE IS RUNNING

echo Wait to load (Port) ...
lcdshow -srvpwroff
lcdshow -sersetpos 7 256
lcdshow -srvenable 192
lcdshow -srvpwron
sleep 2
lcdshow -srvpwroff
echo Load port torpedo!