#!/bin/bash
killall -9 python
killall -9 lcdshow
killall -9 screen

lcdshow -noblink
lcdshow -setbars 12
lcdshow -t "Stopped"

killall -9 lcdshow
