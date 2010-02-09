#!/bin/bash

if [ $# != 1 ];
then
    echo "Incorrect number of arguments."
    echo "Must pass in an integer 1-6."
    exit 1
fi

if [ $1 == "1" ];
then
    echo "Testing port thruster"
    lcdshow -unsafe 1
    lcdshow -setspeed 300 0 0 0 0 0
    exit 0
fi

if [ $1 == "2" ];
then
    echo "Testing starboard thruster"
    lcdshow -unsafe 2
    lcdshow -setspeed 0 300 0 0 0 0
    exit 0
fi

if [ $1 == "3" ];
then
    echo "Testing top thruster"
    lcdshow -unsafe 3
    lcdshow -setspeed 0 0 300 0 0 0
    exit 0
fi

if [ $1 == "4" ];
then
    echo "Testing fore thruster"
    lcdshow -unsafe 4
    lcdshow -setspeed 0 0 0 300 0 0
    exit 0
fi

if [ $1 == "5" ];
then
    echo "Testing bottom thruster"
    lcdshow -unsafe 5
    lcdshow -setspeed 0 0 0 0 300 0
    exit 0
fi

if [ $1 == "6" ];
then
    echo "Testing aft thruster"
    lcdshow -unsafe 6
    lcdshow -setspeed 0 0 0 0 0 300
    exit 0
fi

echo "Incorrect argument. Must be an integer 1-6."