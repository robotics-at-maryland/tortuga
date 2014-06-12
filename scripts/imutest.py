#!/usr/bin/env python

import serial
import struct

#Uncomment out one of these to check that IMU
#ser = serial.Serial('/dev/imu', 115200)
ser = serial.Serial('/dev/magboom', 115200)

fmt = '=BBBHxxxxhhhhhhhhhhhhB'

print 'GyroX\tGyroY\tGyroZ\tAccelX\tAccelY\tAccelZ\tMagnetX\tMagnetY\tMagnetZ\tTempX\tTempY\tTempZ'

try:
    syncbytes = 0
    while True:
        #Synchronize with the 4 0xFF bytes
        out = ser.read()
        if out == 'ff'.decode('hex'):
            syncbytes += 1
        else:
            syncbytes = 0

        #Once it's synced, get the payload
        if syncbytes == 4:
            syncbytes = 0

            out = ser.read(34)

            #Do a checksum test with the final bit
            chksum = 4*255
            for i in range(33):
                chksum += ord(out[i])

            if chksum % 256 != ord(out[33]):
                print 'Bad checksum!'
            
            #Otherwise, unpack the print out the data and print it out
            else:
                st = struct.unpack(fmt, out)
                
                #OUPUT FORMAT:
                #Gyro X,Y,Z
                #Accelerometer X,Y,Z
                #Magnetometer X,Y,Z
                #Temperature X,Y,Z

                print str(st[4]) + '\t' + str(st[5]) + '\t' + str(st[6]) + '\t' \
                    + str(st[7]) + '\t' + str(st[8]) + '\t' + str(st[9]) + '\t' \
                    + str(st[10]) + '\t' + str(st[11]) + '\t' + str(st[12]) + '\t' \
                    + str(st[13]) + '\t' + str(st[14]) + '\t' + str(st[15])

except KeyboardInterrupt:
    pass

ser.close()
