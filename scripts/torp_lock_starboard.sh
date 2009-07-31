# This script locks the starboard torpedo launcher for loading
# It currentlt works around bugs in MotorBoard r3
# DO NOT USE WHILE THE VEHICLE IS RUNNING

echo Locking Starboard launcher...
lcdshow -srvpwroff
lcdshow -sersetpos 7 256
lcdshow -srvenable 255
lcdshow -srvpwron
sleep 2
lcdshow -srvpwroff
echo Insert starboard launcher pin!