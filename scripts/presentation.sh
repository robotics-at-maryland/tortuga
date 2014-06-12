#!/bin/bash
lcdshow -c
#lcdshow -noblink
#lcdshow -setbars 12
echo "Running: pre-qual" #Gate -> Travel1 -> Travel2 -> Sonar -> Vase -> Octagon -> FakeGate -> Bin"
lcdshow -s
lcdshow -t "Starting"
echo "Get ready..."

source scripts/setenv


lcdshow -extgrabber

sleep 12

lcdshow -marker 1

lcdshow -marker 2



#lcdshow -noblink
#lcdshow -setbars 12
lcdshow -t "Stopped"
