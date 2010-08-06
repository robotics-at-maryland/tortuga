#! /bin/sh

# First remove all the modules
sudo rmmod raw1394 video1394 ohci1394 ieee1394

# Then load them all back
sudo modprobe ieee1394
sudo modprobe ohci1394
sudo modprobe video1394
sudo modprobe raw1394