#!/bin/bash
lcdshow -c
#lcdshow -noblink
#lcdshow -setbars 12
echo "Running: Launch Torpedos"
lcdshow -s
lcdshow -t "Get ready..."
echo "Get ready..."
#lcdshow -redgreen
sleep 1
#lcdshow -noblink
#lcdshow -setbars 240
lcdshow -t "Running..."

lcdshow -firetorp 1
sleep 1
lcdshow -firetorp 2


#lcdshow -noblink
#lcdshow -setbars 12
lcdshow -t "Stopped"
