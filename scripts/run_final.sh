#!/bin/bash

echo "Running GATE -> PIPEGATE -> LIGHT -> BIN -> TARGET -> SONAR"
scripts/reload_cameras.sh
lcdshow -unsafe
source scripts/setenv

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

python tools/acs/src/main.py -c data/config/transdec_final_competition.yml -s ram.ai.course.Gate

#lcdshow -noblink
#lcdshow -setbars 12
lcdshow -t "Stopped"