#!/bin/bash
lcdshow -c
#lcdshow -noblink
#lcdshow -setbars 12
echo "Running: Pipe -> Light -> SONAR mission"
lcdshow -s
lcdshow -t "Get ready..."
#lcdshow -redgreen
sleep 2
#lcdshow -noblink
#lcdshow -setbars 240
lcdshow -t "Running..."

scripts/reload_cameras.sh
lcdshow -unsafe
source scripts/setenv
python tools/acs/src/main.py -c data/config/trans_qual.yml -s ram.ai.course.Gate



#lcdshow -noblink
#lcdshow -setbars 12
lcdshow -t "Stopped"
