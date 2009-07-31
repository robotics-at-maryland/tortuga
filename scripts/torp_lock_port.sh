# This script locks the port torpedo launcher for loading
# It currentlt works around bugs in MotorBoard r3
# DO NOT USE WHILE THE VEHICLE IS RUNNING

echo Locking Port launcher...
lcdshow -srvpwroff
lcdshow -sersetpos 7 4000
lcdshow -srvenable 192
lcdshow -srvpwron
sleep 2
lcdshow -srvpwroff
echo Insert port launcher pin!