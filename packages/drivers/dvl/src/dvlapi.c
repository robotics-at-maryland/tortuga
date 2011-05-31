/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Kit Sczudlo <kitsczud@umd.edu>
 * All rights reserved.
 *
 * Author: Kit Sczudlo <kitsczud@umd.edu>
 * File:  packages/dvl/src/dvlapi.c
 */

// If you want to have the DVL API open a file instead of a tty,
// define this:
// #define DEBUG_DVL_OPEN_FILE

// STD Includes
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

// UNIX Includes
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

// Linux Includes (Not Sure If These Are needed)
#ifdef RAM_LINUX
  #include <bits/types.h>
  #include <linux/serial.h>
#endif // RAM_LINUX

// Project Includes
#include "dvlapi.h"

/* Takes two bytes and pops out a short */
short dvl_convert16(unsigned char msb, unsigned char lsb)
{
    return ((signed short) ((msb << 8) | lsb));
}

/* This receives a single byte */
unsigned char dvl_waitByte(int fd)
{
    unsigned char rxb;
    int temp;
    while((temp= read(fd, &rxb, 1)) != 1 && temp != EOF)
        ;

    return rxb;
}

/* This waits for the two starting bytes of 0x7f7f */
/* I have this failing after SYNC_FAIL_BYTECOUNT */
int dvl_waitSync(int fd)
{
    int fs= 0;
    int syncLen= 0;

    while(fs < 2 && syncLen < SYNC_FAIL_BYTECOUNT) {
        if(dvl_waitByte(fd) == 0x7F) {
            fs++;
         } else {
            fs= 0;
         }
        syncLen++;
    }

    if(syncLen >= SYNC_FAIL_BYTECOUNT) {
        printf("UNABLE TO SYCHRONIZE WITH DVL!\n");
        return -1;
    }

    return 0;
}

/* This reads in the data from the DVL and stores it so
   that the AI and controls guys have something to work with! */
int readDVLData(int fd, RawDVLData* dvl)
{
    /* So there's a variable number of bytes we can get:
       6  bytes for the header + (2 bytes * the 6th byte) +
       58 bytes for the fixed leader +
       60 bytes for the variable leader +
       81 bytes for the bottom track data +
       4  bytes for garbage and checksum
       = 211 bytes
       Based on that, here's a slightly oversized buffer:
       */
    unsigned char dvlData[512];

    int len, i, tempsize, offset;
    uint16_t checksum;
    CompleteDVLPacket *dbgpkt = NULL;

    if(dvl_waitSync(fd))
        return ERR_NOSYNC;

    dvl->valid= 0;

    /* This checks that we have the debugging packet setup! */
    if(dvl->privDbgInf == NULL) {
        printf("WARNING! Debug info reallocated!\n");
        dbgpkt= dvl->privDbgInf = malloc(sizeof(CompleteDVLPacket));

        /* We'll need to set up some parts of the packet... */
        dbgpkt->fixedleaderset= 0;
        dbgpkt->header.offsets= NULL;
    } else {
        dbgpkt= dvl->privDbgInf;
    }

    /* We got these in the dvl_waitSync() call */
    dvlData[0]= dvlData[1]= 0x7F;

    /* Set length based on the 0x7F7F we recieved */
    len= 2;

    /* Get the header */
    while(len < 6)
        len+= read(fd, dvlData + len, 6 - len);

    dbgpkt->header.HeaderID= dvlData[0];
    dbgpkt->header.DataSourceID= dvlData[1];
    dbgpkt->header.PacketSize= dvl_convert16(dvlData[3], dvlData[2]);
    /* dvlData[4] is an empty byte! */
    dbgpkt->header.num_datatypes= dvlData[5];

    /* We need to prevent memory leaks! Free up the old offset array! */
    if(dbgpkt->header.offsets != NULL)
        free(dbgpkt->header.offsets);

    dbgpkt->header.offsets= malloc(sizeof(int) * dvlData[5]);

    tempsize= 6 + ((int) dbgpkt->header.num_datatypes) * 2;
    while(len < tempsize)
        len += read(fd, dvlData + len, tempsize - len);

    for(i= 0;i < dbgpkt->header.num_datatypes;i++)
        dbgpkt->header.offsets[i]= dvl_convert16(dvlData[7 + i * 2],
                                             dvlData[6 + i * 2]);

    /* WOO! We've now finished reading in the header */
    /* We should probably double check that nothing is too weird... */
    if(dbgpkt->header.PacketSize > 512) {
        /* If we're here we're fucked.  We have a packet which is */
        /* too big for our buffer to hold it! */
        printf("ERROR! Packet too big!\n");
        return ERR_TOOBIG;
    }

    /*************************************************/
    /* Now we muck our way through the fixed leader! */
    /*************************************************/
    
    /* We now have an unknown offset into the buffer we'll need to */
    /* keep track of said offset. */
    offset= tempsize;

    /* The fixed leader will add an extra 58 bytes of useless info */
    tempsize+= 58;
    while(len < tempsize)
        len += read(fd, dvlData + len, tempsize - len);

    if(dbgpkt->fixedleaderset == 0) {
        dbgpkt->fixedleader.FixedLeaderID= dvl_convert16(dvlData[offset + 1],
                                                     dvlData[offset]);

        if(dbgpkt->fixedleader.FixedLeaderID != 0x0000)
            return ERR_BADFIXEDLEADER;

        dbgpkt->fixedleader.CPU_Firmware_Version= dvlData[offset + 2];
        dbgpkt->fixedleader.CPU_Firmware_Revision= dvlData[offset + 3];
        dbgpkt->fixedleader.System_Config= dvl_convert16(dvlData[offset + 5],
                                                     dvlData[offset + 4]);
        dbgpkt->fixedleader.Real_Sim_flag= dvlData[offset + 6];
        dbgpkt->fixedleader.Lag_Length= dvlData[offset + 7];
        dbgpkt->fixedleader.Num_Beams= dvlData[offset + 8];
        dbgpkt->fixedleader.Num_Cells= dvlData[offset + 9];
        dbgpkt->fixedleader.pings_per_packet= dvl_convert16(dvlData[offset + 11],
                                                        dvlData[offset + 10]);
        dbgpkt->fixedleader.depth_cell_length= dvl_convert16(dvlData[offset + 13],
                                                         dvlData[offset + 12]);
        dbgpkt->fixedleader.blank_after_transmit= dvl_convert16(dvlData[offset + 15],
                                                            dvlData[offset + 14]);
        dbgpkt->fixedleader.prof_mode= dvlData[offset + 16];
        dbgpkt->fixedleader.low_corr_thresh= dvlData[offset + 17];
        dbgpkt->fixedleader.num_code_reps= dvlData[offset + 18];
        dbgpkt->fixedleader.prcnt_good_min= dvlData[offset + 19];
        dbgpkt->fixedleader.err_vel_max= dvl_convert16(dvlData[offset + 21],
                                                   dvlData[offset + 20]);
        dbgpkt->fixedleader.min_btwn_ping= dvlData[offset + 22];
        dbgpkt->fixedleader.sec_btwn_ping= dvlData[offset + 23];
        dbgpkt->fixedleader.hundredths_btwn_ping= dvlData[offset + 24];
        dbgpkt->fixedleader.coord_transform= dvlData[offset + 25];
        dbgpkt->fixedleader.head_align= dvl_convert16(dvlData[offset + 27],
                                                  dvlData[offset + 26]);
        dbgpkt->fixedleader.head_bias= dvl_convert16(dvlData[offset + 29],
                                                 dvlData[offset + 28]);
        dbgpkt->fixedleader.sen_source= dvlData[offset + 30];
        dbgpkt->fixedleader.sen_avail= dvlData[offset + 31];
        dbgpkt->fixedleader.bin1_dist= dvl_convert16(dvlData[offset + 33],
                                                 dvlData[offset + 32]);
        dbgpkt->fixedleader.xmit_pulse_length= dvl_convert16(dvlData[offset + 35],
                                                         dvlData[offset + 34]);
        dbgpkt->fixedleader.ref_lyr_avg= dvl_convert16(dvlData[offset + 37],
                                                   dvlData[offset + 36]);
        dbgpkt->fixedleader.false_trgt_thresh= dvlData[offset + 38];
        /* Byte 39 is empty! */
        dbgpkt->fixedleader.lagdist= dvl_convert16(dvlData[offset + 41],
                                               dvlData[offset + 40]);
        /* Bytes 42 - 49 are all empty */
        dbgpkt->fixedleader.sys_bwidth= dvl_convert16(dvlData[offset + 51],
                                                  dvlData[offset + 50]);
        /* Bytes 52 and 53 are empty */
        dbgpkt->fixedleader.serial[0]= dvlData[offset + 54];
        dbgpkt->fixedleader.serial[1]= dvlData[offset + 55];
        dbgpkt->fixedleader.serial[2]= dvlData[offset + 56];
        dbgpkt->fixedleader.serial[3]= dvlData[offset + 57];
    } else {
        /* In the future this will actually contain useful checks and have   */
        /* warnings if any of the fixed leader data changes across ensembles */
    }

    /********************************************************/
    /* FUCK YOU FIXED LEADER! Now onto the Variable Leader! */
    /********************************************************/

    /* We now have an unknown offset into the buffer we'll need to */
    /* keep track of said offset. */
    offset= tempsize;

    /* The variable leader will add an extra 60 bytes of useful info */
    tempsize+= 60;
    while(len < tempsize)
        len += read(fd, dvlData + len, tempsize - len);

    dbgpkt->variableleader.varleaderID= dvl_convert16(dvlData[offset + 1],
                                                  dvlData[offset]);

    if(dbgpkt->variableleader.varleaderID != 0x0080) {
        printf("WARNING! Bad variable leader!\n");
        return ERR_BADVARIABLELEADER;
    }

    dbgpkt->variableleader.ensemblenum= dvl_convert16(dvlData[offset + 3],
                                                  dvlData[offset + 2]);
    dbgpkt->variableleader.RTC_year= dvlData[offset + 4];
    dbgpkt->variableleader.RTC_month= dvlData[offset + 5];
    dbgpkt->variableleader.RTC_day= dvlData[offset + 6];
    dbgpkt->variableleader.RTC_hour= dvlData[offset + 7];
    dbgpkt->variableleader.RTC_minute= dvlData[offset + 8];
    dbgpkt->variableleader.RTC_second= dvlData[offset + 9];
    dbgpkt->variableleader.RTC_hundredths= dvlData[offset + 10];

    dbgpkt->variableleader.ensemble_num_msb= dvlData[offset + 11];
    dbgpkt->variableleader.bit_result= dvl_convert16(dvlData[offset + 13],
                                                 dvlData[offset + 12]);
    dbgpkt->variableleader.sound_speed= dvl_convert16(dvlData[offset + 15],
                                                  dvlData[offset + 14]);
    dbgpkt->variableleader.transducer_depth= dvl_convert16(dvlData[offset + 17],
                                                       dvlData[offset + 16]);
    dbgpkt->variableleader.heading= dvl_convert16(dvlData[offset + 19],
                                              dvlData[offset + 18]);
    dbgpkt->variableleader.pitch= dvl_convert16(dvlData[offset + 21],
                                            dvlData[offset + 20]);
    dbgpkt->variableleader.roll= dvl_convert16(dvlData[offset + 23],
                                           dvlData[offset + 22]);
    dbgpkt->variableleader.salinity= dvl_convert16(dvlData[offset + 25],
                                               dvlData[offset + 24]);
    dbgpkt->variableleader.temp= dvl_convert16(dvlData[offset + 27],
                                           dvlData[offset + 26]);
    dbgpkt->variableleader.MPT_minutes= dvlData[offset + 28];
    dbgpkt->variableleader.MPT_seconds= dvlData[offset + 29];
    dbgpkt->variableleader.MPT_hundredths= dvlData[offset + 30];
    dbgpkt->variableleader.HeadingStdDev= dvlData[offset + 31];
    dbgpkt->variableleader.PitchStdDev= dvlData[offset + 32];
    dbgpkt->variableleader.RollStdDev= dvlData[offset + 33];
    dbgpkt->variableleader.dvl_adc[0]= dvlData[offset + 34];
    dbgpkt->variableleader.dvl_adc[1]= dvlData[offset + 35];
    dbgpkt->variableleader.dvl_adc[2]= dvlData[offset + 36];
    dbgpkt->variableleader.dvl_adc[3]= dvlData[offset + 37];
    dbgpkt->variableleader.dvl_adc[4]= dvlData[offset + 38];
    dbgpkt->variableleader.dvl_adc[5]= dvlData[offset + 39];
    dbgpkt->variableleader.dvl_adc[6]= dvlData[offset + 40];
    dbgpkt->variableleader.dvl_adc[7]= dvlData[offset + 41];
    dbgpkt->variableleader.errorstatus= dvlData[offset + 45];
    dbgpkt->variableleader.errorstatus= (dbgpkt->variableleader.errorstatus << 8)
                                        + dvlData[offset + 44];
    dbgpkt->variableleader.errorstatus= (dbgpkt->variableleader.errorstatus << 8)
                                        + dvlData[offset + 43];
    dbgpkt->variableleader.errorstatus= (dbgpkt->variableleader.errorstatus << 8)
                                        + dvlData[offset + 42];
    /* Bytes 46 and 47 are empty */
    dbgpkt->variableleader.pressure= dvlData[offset + 51];
    dbgpkt->variableleader.pressure= (dbgpkt->variableleader.pressure << 8)
                                     + dvlData[offset + 50];
    dbgpkt->variableleader.pressure= (dbgpkt->variableleader.pressure << 8)
                                     + dvlData[offset + 49];
    dbgpkt->variableleader.pressure= (dbgpkt->variableleader.pressure << 8)
                                     + dvlData[offset + 48];
    dbgpkt->variableleader.pressure_variance= dvlData[offset + 55];
    dbgpkt->variableleader.pressure_variance= (dbgpkt->variableleader.pressure_variance << 8)
                                              + dvlData[offset + 54];
    dbgpkt->variableleader.pressure_variance= (dbgpkt->variableleader.pressure_variance << 8)
                                              + dvlData[offset + 53];
    dbgpkt->variableleader.pressure_variance= (dbgpkt->variableleader.pressure_variance << 8)
                                              + dvlData[offset + 52];
    /* Bytes 56 through 59 are all empty */

    /*******************************************************/
    /* FUCK YOU VARIABLE LEADER! Now onto the actual data! */
    /*******************************************************/

    /* We now have an unknown offset into the buffer we'll need to */
    /* keep track of said offset. */
    offset= tempsize;

    tempsize+= 81;
    while(len < tempsize)
        len += read(fd, dvlData + len, tempsize - len);

    dbgpkt->btdata.BottomTrackID= dvl_convert16(dvlData[offset + 1],
                                            dvlData[offset]);
    if(dbgpkt->btdata.BottomTrackID != 0x0600) {
        printf("WARNING! BottomtrackID not valid!\n");
        printf("Expected %04x but got %04x\n", 0x0600, dbgpkt->btdata.BottomTrackID);
        dvl->valid= ERR_BADBTID;
        return ERR_BADBTID;
    }

    dbgpkt->btdata.bt_pings_per_ensemble=
        dvl_convert16(dvlData[offset + 3], dvlData[offset + 2]);
    dbgpkt->btdata.bt_delay_before_reaquire=
        dvl_convert16(dvlData[offset + 5], dvlData[offset + 4]);
    dbgpkt->btdata.bt_corr_mag_min= dvlData[offset + 6];
    dbgpkt->btdata.bt_eval_amp_min= dvlData[offset + 7];
    dbgpkt->btdata.bt_prcnt_good_min= dvlData[offset + 8];
    dbgpkt->btdata.bt_mode= dvlData[offset + 9];
    dbgpkt->btdata.bt_err_vel= dvl_convert16(dvlData[offset + 11],
                                         dvlData[offset + 10]);
    // Bytes 12 through 15 are empty!
    dbgpkt->btdata.bt_range[0]= dvl_convert16(dvlData[offset + 17],
                                          dvlData[offset + 16]);
    dbgpkt->btdata.bt_range[1]= dvl_convert16(dvlData[offset + 19],
                                          dvlData[offset + 18]);
    dbgpkt->btdata.bt_range[2]= dvl_convert16(dvlData[offset + 21],
                                          dvlData[offset + 20]);
    dbgpkt->btdata.bt_range[3]= dvl_convert16(dvlData[offset + 23],
                                          dvlData[offset + 22]);
    dbgpkt->btdata.bt_vel[0]= dvl_convert16(dvlData[offset + 25],
                                        dvlData[offset + 24]);
    dbgpkt->btdata.bt_vel[1]= dvl_convert16(dvlData[offset + 27],
                                        dvlData[offset + 26]);
    dbgpkt->btdata.bt_vel[2]= dvl_convert16(dvlData[offset + 29],
                                        dvlData[offset + 28]);
    dbgpkt->btdata.bt_vel[3]= dvl_convert16(dvlData[offset + 31],
                                        dvlData[offset + 30]);
    dbgpkt->btdata.bt_beam_corr[0]= dvlData[offset + 32];
    dbgpkt->btdata.bt_beam_corr[1]= dvlData[offset + 33];
    dbgpkt->btdata.bt_beam_corr[2]= dvlData[offset + 34];
    dbgpkt->btdata.bt_beam_corr[3]= dvlData[offset + 35];
    dbgpkt->btdata.bt_eval_amp[0]= dvlData[offset + 36];
    dbgpkt->btdata.bt_eval_amp[1]= dvlData[offset + 37];
    dbgpkt->btdata.bt_eval_amp[2]= dvlData[offset + 38];
    dbgpkt->btdata.bt_eval_amp[3]= dvlData[offset + 39];
    dbgpkt->btdata.bt_prcnt_good[0]= dvlData[offset + 40];
    dbgpkt->btdata.bt_prcnt_good[1]= dvlData[offset + 41];
    dbgpkt->btdata.bt_prcnt_good[2]= dvlData[offset + 42];
    dbgpkt->btdata.bt_prcnt_good[3]= dvlData[offset + 43];
    dbgpkt->btdata.ref_lyr_min= dvl_convert16(dvlData[offset + 45],
                                          dvlData[offset + 44]);
    dbgpkt->btdata.ref_lyr_near= dvl_convert16(dvlData[offset + 47],
                                           dvlData[offset + 46]);
    dbgpkt->btdata.ref_lyr_far= dvl_convert16(dvlData[offset + 49],
                                          dvlData[offset + 48]);
    dbgpkt->btdata.ref_layer_vel[0]=
        dvl_convert16(dvlData[offset + 51], dvlData[offset + 50]);
    dbgpkt->btdata.ref_layer_vel[1]=
        dvl_convert16(dvlData[offset + 53], dvlData[offset + 52]);
    dbgpkt->btdata.ref_layer_vel[2]=
        dvl_convert16(dvlData[offset + 55], dvlData[offset + 54]);
    dbgpkt->btdata.ref_layer_vel[3]=
        dvl_convert16(dvlData[offset + 57], dvlData[offset + 56]);
    dbgpkt->btdata.ref_corr[0]= dvlData[offset + 58];
    dbgpkt->btdata.ref_corr[1]= dvlData[offset + 59];
    dbgpkt->btdata.ref_corr[2]= dvlData[offset + 60];
    dbgpkt->btdata.ref_corr[3]= dvlData[offset + 61];
    dbgpkt->btdata.ref_int[0]= dvlData[offset + 62];
    dbgpkt->btdata.ref_int[1]= dvlData[offset + 63];
    dbgpkt->btdata.ref_int[2]= dvlData[offset + 64];
    dbgpkt->btdata.ref_int[3]= dvlData[offset + 65];
    dbgpkt->btdata.ref_prcnt_good[0]= dvlData[offset + 66];
    dbgpkt->btdata.ref_prcnt_good[1]= dvlData[offset + 67];
    dbgpkt->btdata.ref_prcnt_good[2]= dvlData[offset + 68];
    dbgpkt->btdata.ref_prcnt_good[3]= dvlData[offset + 69];
    dbgpkt->btdata.bt_max_depth= dvl_convert16(dvlData[offset + 71],
                                           dvlData[offset + 70]);
    dbgpkt->btdata.rssi_amp[0]= dvlData[offset + 72];
    dbgpkt->btdata.rssi_amp[1]= dvlData[offset + 73];
    dbgpkt->btdata.rssi_amp[2]= dvlData[offset + 74];
    dbgpkt->btdata.rssi_amp[3]= dvlData[offset + 75];
    dbgpkt->btdata.gain= dvlData[offset + 76];
    dbgpkt->btdata.bt_range_msb[0]= dvlData[offset + 77];
    dbgpkt->btdata.bt_range_msb[1]= dvlData[offset + 78];
    dbgpkt->btdata.bt_range_msb[2]= dvlData[offset + 79];
    dbgpkt->btdata.bt_range_msb[3]= dvlData[offset + 80];

    offset= tempsize;

    /* Now grab the checksum! */
    tempsize+= 2;

    while(len < tempsize)
        len+= read(fd, dvlData + len, tempsize - len);

    dbgpkt->checksum= dvl_convert16(dvlData[offset + 1],
                                    dvlData[offset]);

    checksum= 0;

    /* Calculate the checksum */
    for(i= 0;i < tempsize - 2;i++)
        checksum+= dvlData[i];

    if(checksum != dbgpkt->checksum) {
        printf("WARNING! Bad checksum.\n");
        printf("Expected 0x%02x but got 0x%02x\n", checksum, dbgpkt->checksum);
        dvl->valid= ERR_CHKSUM;
        return ERR_CHKSUM;
    }

    dvl->ensemblenum= (dbgpkt->variableleader.ensemblenum | (dbgpkt->variableleader.ensemble_num_msb << 16));

    dvl->year= dbgpkt->variableleader.RTC_year;
    dvl->month= dbgpkt->variableleader.RTC_month;
    dvl->day= dbgpkt->variableleader.RTC_day;
    dvl->hour= dbgpkt->variableleader.RTC_hour;
    dvl->min= dbgpkt->variableleader.RTC_minute;
    dvl->sec= dbgpkt->variableleader.RTC_second;
    dvl->hundredth= dbgpkt->variableleader.RTC_hundredths;

    for(i= 0;i < 4;i++)
        dvl->bt_velocity[i]= dbgpkt->btdata.bt_vel[i];

    return 0;
}

/* Some code from cutecom, which in turn may have come from minicom */
int openDVL(const char* devName)
{
#ifdef DEBUG_DVL_OPEN_FILE
   int fd= openDVL(devName);

   return fd;
#else
   int fd = open(devName, O_RDWR, O_ASYNC);

    if(fd == -1)
        return -1;

    struct termios newtio;
    if (tcgetattr(fd, &newtio)!=0)
        printf("\nFirst stuff failed\n");

    unsigned int _baud= B115200;
    cfsetospeed(&newtio, _baud);
    cfsetispeed(&newtio, _baud);


    newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS8;
    newtio.c_cflag |= CLOCAL | CREAD;
    newtio.c_cflag &= ~(PARENB | PARODD);

    newtio.c_cflag &= ~CRTSCTS;
    newtio.c_cflag &= ~CSTOPB;

    newtio.c_iflag=IGNBRK;
    newtio.c_iflag &= ~(IXON|IXOFF|IXANY);

    newtio.c_lflag=0;

    newtio.c_oflag=0;


    newtio.c_cc[VTIME]=1;
    newtio.c_cc[VMIN]=60;

//   tcflush(m_fd, TCIFLUSH);
    if (tcsetattr(fd, TCSANOW, &newtio)!=0)
        printf("tsetaddr1 failed!\n");

    int mcs=0;

    ioctl(fd, TIOCMGET, &mcs);
    mcs |= TIOCM_RTS;
    ioctl(fd, TIOCMSET, &mcs);

    if (tcgetattr(fd, &newtio)!=0)
        printf("tcgetattr() 4 failed\n");

    newtio.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &newtio)!=0)
      printf("tcsetattr() 2 failed\n");
    
    return fd;
#endif
}



/* We now need to trundle through lots of data, some of which we
       may not know how to parse. */
/*    while(len < dbgpkt->header.PacketSize - 4)
    {
        // The next chunk of data is two bytes which *should* identify the
        // next big chunk as being a Bottom Track packet.  Otherwise we're
        // FUCKED.
        tempsize+= 2;
        while(len < tempsize)
            len += read(fd, dvlData + len, tempsize - len);

        switch(dvl_convert16(dvlData[offset + 1], dvlData[offset]))
        {
            case(0x0600):
            {
                // This is a bottom track packet!

                // Read in all 79 bytes! (the id bytes were already read in!)
                tempsize+= 79;
                while(len < tempsize)
                    len += read(fd, dvlData + len, tempsize - len);

                dbgpkt->btdata.BottomTrackID= dvl_convert16(dvlData[offset + 1],
                                                        dvlData[offset]);
                dbgpkt->btdata.bt_pings_per_ensemble=
                    dvl_convert16(dvlData[offset + 3], dvlData[offset + 2]);
                dbgpkt->btdata.bt_delay_before_reaquire=
                    dvl_convert16(dvlData[offset + 5], dvlData[offset + 4]);
                dbgpkt->btdata.bt_corr_mag_min= dvlData[offset + 6];
                dbgpkt->btdata.bt_eval_amp_min= dvlData[offset + 7];
                dbgpkt->btdata.bt_prcnt_good_min= dvlData[offset + 8];
                dbgpkt->btdata.bt_mode= dvlData[offset + 9];
                dbgpkt->btdata.bt_err_vel= dvl_convert16(dvlData[offset + 11],
                                                     dvlData[offset + 10]);
                // Bytes 12 through 15 are empty!
                dbgpkt->btdata.bt_range[0]= dvl_convert16(dvlData[offset + 17],
                                                      dvlData[offset + 16]);
                dbgpkt->btdata.bt_range[1]= dvl_convert16(dvlData[offset + 19],
                                                      dvlData[offset + 18]);
                dbgpkt->btdata.bt_range[2]= dvl_convert16(dvlData[offset + 21],
                                                      dvlData[offset + 20]);
                dbgpkt->btdata.bt_range[3]= dvl_convert16(dvlData[offset + 23],
                                                      dvlData[offset + 22]);
                dbgpkt->btdata.bt_vel[0]= dvl_convert16(dvlData[offset + 25],
                                                    dvlData[offset + 24]);
                dbgpkt->btdata.bt_vel[1]= dvl_convert16(dvlData[offset + 27],
                                                    dvlData[offset + 26]);
                dbgpkt->btdata.bt_vel[2]= dvl_convert16(dvlData[offset + 29],
                                                    dvlData[offset + 28]);
                dbgpkt->btdata.bt_vel[3]= dvl_convert16(dvlData[offset + 31],
                                                    dvlData[offset + 30]);
                dbgpkt->btdata.bt_beam_corr[0]= dvlData[offset + 32];
                dbgpkt->btdata.bt_beam_corr[1]= dvlData[offset + 33];
                dbgpkt->btdata.bt_beam_corr[2]= dvlData[offset + 34];
                dbgpkt->btdata.bt_beam_corr[3]= dvlData[offset + 35];
                dbgpkt->btdata.bt_eval_amp[0]= dvlData[offset + 36];
                dbgpkt->btdata.bt_eval_amp[1]= dvlData[offset + 37];
                dbgpkt->btdata.bt_eval_amp[2]= dvlData[offset + 38];
                dbgpkt->btdata.bt_eval_amp[3]= dvlData[offset + 39];
                dbgpkt->btdata.bt_prcnt_good[0]= dvlData[offset + 40];
                dbgpkt->btdata.bt_prcnt_good[1]= dvlData[offset + 41];
                dbgpkt->btdata.bt_prcnt_good[2]= dvlData[offset + 42];
                dbgpkt->btdata.bt_prcnt_good[3]= dvlData[offset + 43];
                dbgpkt->btdata.ref_lyr_min= dvl_convert16(dvlData[offset + 45],
                                                      dvlData[offset + 44]);
                dbgpkt->btdata.ref_lyr_near= dvl_convert16(dvlData[offset + 47],
                                                       dvlData[offset + 46]);
                dbgpkt->btdata.ref_lyr_far= dvl_convert16(dvlData[offset + 49],
                                                      dvlData[offset + 48]);
                dbgpkt->btdata.ref_layer_vel[0]=
                    dvl_convert16(dvlData[offset + 51], dvlData[offset + 50]);
                dbgpkt->btdata.ref_layer_vel[1]=
                    dvl_convert16(dvlData[offset + 53], dvlData[offset + 52]);
                dbgpkt->btdata.ref_layer_vel[2]=
                    dvl_convert16(dvlData[offset + 55], dvlData[offset + 54]);
                dbgpkt->btdata.ref_layer_vel[3]=
                    dvl_convert16(dvlData[offset + 57], dvlData[offset + 56]);
                dbgpkt->btdata.ref_corr[0]= dvlData[offset + 58];
                dbgpkt->btdata.ref_corr[1]= dvlData[offset + 59];
                dbgpkt->btdata.ref_corr[2]= dvlData[offset + 60];
                dbgpkt->btdata.ref_corr[3]= dvlData[offset + 61];
                dbgpkt->btdata.ref_int[0]= dvlData[offset + 62];
                dbgpkt->btdata.ref_int[1]= dvlData[offset + 63];
                dbgpkt->btdata.ref_int[2]= dvlData[offset + 64];
                dbgpkt->btdata.ref_int[3]= dvlData[offset + 65];
                dbgpkt->btdata.ref_prcnt_good[0]= dvlData[offset + 66];
                dbgpkt->btdata.ref_prcnt_good[1]= dvlData[offset + 67];
                dbgpkt->btdata.ref_prcnt_good[2]= dvlData[offset + 68];
                dbgpkt->btdata.ref_prcnt_good[3]= dvlData[offset + 69];
                dbgpkt->btdata.bt_max_depth= dvl_convert16(dvlData[offset + 71],
                                                       dvlData[offset + 70]);
                dbgpkt->btdata.rssi_amp[0]= dvlData[offset + 72];
                dbgpkt->btdata.rssi_amp[1]= dvlData[offset + 73];
                dbgpkt->btdata.rssi_amp[2]= dvlData[offset + 74];
                dbgpkt->btdata.rssi_amp[3]= dvlData[offset + 75];
                dbgpkt->btdata.gain= dvlData[offset + 76];
                dbgpkt->btdata.bt_range_msb[0]= dvlData[offset + 77];
                dbgpkt->btdata.bt_range_msb[1]= dvlData[offset + 78];
                dbgpkt->btdata.bt_range_msb[2]= dvlData[offset + 79];
                dbgpkt->btdata.bt_range_msb[3]= dvlData[offset + 80];

                break;
            }

            default:
            {
                // If we're here we don't understand the data being sent!
                // we'll discard it using the offset information and move
                // along!

                printf("WARNING! Unknown Datatype!\n");

                // Find the offset
                for(i= 0;i < dbgpkt->header.num_datatypes &&
                         dbgpkt->header.offsets[i] < tempsize;i++)
                    ;

                // Make it the new size
                if(i == dbgpkt->header.num_datatypes) {
                    tempsize+= (dbgpkt->header.PacketSize - 4) -
                               (dbgpkt->header.offsets[i - 1]);
                } else {
                    tempsize+= dbgpkt->header.offsets[i] -
                               dbgpkt->header.offsets[i - 1];
                }

                // Read it into the buffer in order to eat it!
                while(len < tempsize)
                    len += read(fd, dvlData + len, tempsize - len);

                break;
            }

        }

        // Keep track of the new offset!
        offset= tempsize;
    } */
