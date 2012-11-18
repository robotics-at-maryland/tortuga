#!/bin/bash
lcdshow -c
lcdshow -s
lcdshow -t "Get ready..."
lcdshow -redgreen
sleep 5
lcdshow -t "Running..."


lcdshow -unsafe
source scripts/setenv
python tools/acs/src/main.py -c data/config/transdec_final_competition.yml -s ram.ai.course.Gate



lcdshow -t "Stopped"
