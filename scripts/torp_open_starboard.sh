# This script opens the starboard torpedo launcher for loading
# It currentlt works around bugs in MotorBoard r3
# DO NOT USE WHILE THE VEHICLE IS RUNNING

echo Wait to load (Starboard) ...
lcdshow -srvpwroff
lcdshow -sersetpos 7 4000
lcdshow -srvenable 255
lcdshow -srvpwron
sleep 2
lcdshow -srvpwroff
echo Load starboard torpedo!