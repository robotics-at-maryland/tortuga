#!/bin/bash

x= 1
while [ $x -eq $x ] 
do
    nc $1 $2 -q 0 -n -vv -u < send_fade
    sleep 1
done
