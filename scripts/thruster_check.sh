#!/bin/bash

echo "Testing port thruster"
lcdshow -unsafe 1
lcdshow -setspeed 300 0 0 0 0 0

echo "Testing starboard thruster"
lcdshow -unsafe 2
lcdshow -setspeed 0 300 0 0 0 0

echo "Testing top thruster"
lcdshow -unsafe 3
lcdshow -setspeed 0 0 300 0 0 0

echo "Testing fore thruster"
lcdshow -unsafe 4
lcdshow -setspeed 0 0 0 300 0 0

echo "Testing bottom thruster"
lcdshow -unsafe 5
lcdshow -setspeed 0 0 0 0 300 0

echo "Testing aft thruster"
lcdshow -unsafe 6
lcdshow -setspeed 0 0 0 0 0 300
