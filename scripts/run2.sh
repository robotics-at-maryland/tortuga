#!/bin/bash
lcdshow -c
lcdshow -noblink
lcdshow -setbars 12
lcdshow -s
lcdshow -t "Get ready..."
lcdshow -redgreen
sleep 5
lcdshow -noblink
lcdshow -setbars 240
lcdshow -t "Running..."


lcdshow -unsafe
source scripts/setenv
python tools/acs/src/main2.py -c data/config/aitest2.yml



lcdshow -noblink
lcdshow -setbars 12
lcdshow -t "Stopped"
