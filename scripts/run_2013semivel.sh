#!/bin/bash

echo "Running GATE -> SONAR"
scripts/reload_cameras.sh
source scripts/setenv
lcdshow -unsafe

lcdshow -c
#lcdshow -noblink
#lcdshow -setbars 12
lcdshow -s
lcdshow -t "Get ready..."
#lcdshow -redgreen
sleep 2
#lcdshow -noblink
#lcdshow -setbars 240
lcdshow -t "Running..."

python tools/acs/src/main.py -c data/config/Mao2013_practice.yml -s ram.ai.course.GateVel

#lcdshow -noblink
#lcdshow -setbars 12
lcdshow -t "Stopped"
