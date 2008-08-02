#!/bin/bash
killall python
killall lcdshow
killall screen

lcdshow -noblink
lcdshow -setbars 12
lcdshow -t "Stopped"

killall lcdshow
