#!/bin/bash
lcdshow -c
#lcdshow -noblink
#lcdshow -setbars 12
echo "Running: SONAR only mission"
lcdshow -s
lcdshow -t "Get ready..."
echo "Starting mission"
#lcdshow -redgreen
sleep 2
#lcdshow -noblink
#lcdshow -setbars 240
lcdshow -t "Running..."

scripts/reload_cameras.sh
lcdshow -unsafe
source scripts/setenv
python tools/acs/src/main.py -c data/config/trans_qual2.yml -s ram.ai.course.Gate



#lcdshow -noblink
#lcdshow -setbars 12
lcdshow -t "Stopped"
