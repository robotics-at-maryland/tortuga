/*
 * Copyright (C) 2009 Robotics at Maryland
 * Copyright (C) 2009 Kit Sczudlo <kitsczud@umd.edu>
 * All rights reserved.
 *
 * Author: Kit Sczudlo <kitsczud@umd.edu>
 * File:  packages/dvl/include/dvlapi.h
 */

#ifndef RAM_DVLAPI_H_12_02_2009
#define RAM_DVLAPI_H_12_02_2009

// If we are compiling as C++ code we need to use extern "C" linkage
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/* We should abort trying to sync if it takes more than a
   couple packets worth of bytes, or more than 1 second. */
#define SYNC_FAIL_BYTECOUNT 2000
#define SYNC_FAIL_MILLISEC 1000
#define SYNC_FAIL_SECONDS (SYNC_FAIL_MILLISEC/1000)

/* These are error messages */
#define ERR_NOSYNC            0x0001
#define ERR_TOOBIG            0x0002
#define ERR_BADFIXEDLEADER    0x0003
#define ERR_BADVARIABLELEADER 0x0004
#define ERR_BADBTID           0x0005
#define ERR_CHKSUM            0x0006

/* DVL Header information */
typedef struct _DVLHeaderData
{
    /* Together, these two should be 0x7F7F */
    unsigned char HeaderID,
                  DataSourceID;
    
    unsigned int PacketSize;

    /* 1 empty byte here */

    unsigned char num_datatypes;

    unsigned int *offsets;
} DVLHeaderData;

/* DVL Fixed Leader contains all the info which shouldn't change */
/* That being said, this info will essentially be useless */
typedef struct _DVLFixedLeaderData
{
    /* This should be 0x0000 */
    short FixedLeaderID;

    unsigned char CPU_Firmware_Version, CPU_Firmware_Revision;

    unsigned short System_Config;

    unsigned char Real_Sim_flag,
                  Lag_Length,
                  Num_Beams,
                  Num_Cells;

    unsigned int pings_per_packet,
                 depth_cell_length,
                 blank_after_transmit;

    unsigned char prof_mode,
                  low_corr_thresh,
                  num_code_reps;
    
    unsigned char prcnt_good_min;

    int err_vel_max;

    unsigned char min_btwn_ping,
                  sec_btwn_ping,
                  hundredths_btwn_ping;

    unsigned char coord_transform;

    unsigned int head_align,
                 head_bias;

    unsigned char sen_source,
                  sen_avail;

    unsigned int bin1_dist,
                 xmit_pulse_length,
                 ref_lyr_avg;

    unsigned char false_trgt_thresh;

    /* Empty Byte! */

    unsigned int lagdist;

    /* Bytes 42 - 49 are all empty */

    unsigned int sys_bwidth;

    /* Bytes 52 and 53 are empty */

    unsigned char serial[4];
} DVLFixedLeaderData;

/* A leader for all the information which should change */
/* on a per ensemble basis. This contains useful info. */
typedef struct _DVLVariableLeaderData
{
    /* The variable leader ID should be 0x8000 */
    unsigned short varleaderID;
    unsigned int ensemblenum;

    unsigned char RTC_year,
                  RTC_month,
                  RTC_day,
                  RTC_hour,
                  RTC_minute,
                  RTC_second,
                  RTC_hundredths;

    unsigned int ensemble_num_msb;

    unsigned int bit_result,
                 sound_speed,
                 transducer_depth,
                 heading,
                 pitch,
                 roll,
                 salinity,
                 temp;

    unsigned char MPT_minutes,
                  MPT_seconds,
                  MPT_hundredths;

    unsigned char HeadingStdDev,
                  PitchStdDev,
                  RollStdDev;

    unsigned char dvl_adc[8];

    unsigned int errorstatus;

    /* Bytes 46 and 47 are empty */

    unsigned int pressure,
                 pressure_variance;

    /* Bytes 56 through 59 are empty! */
} DVLVariableLeaderData;

/* This is a struct which will hold the absolute in-plane */
/* velocity relative to the bottom of the tank. */
typedef struct _DVLBottomTrackData
{
    /* This should be 0x0600 */
    unsigned short BottomTrackID;

    unsigned int bt_pings_per_ensemble,
                 bt_delay_before_reaquire;

    unsigned char bt_corr_mag_min,
                  bt_eval_amp_min,
                  bt_prcnt_good_min,
                  bt_mode;

    unsigned int bt_err_vel;

    /* Bytes 12 through 15 are empty! */

    unsigned int bt_range[4];

    unsigned int bt_vel[4];

    unsigned char bt_beam_corr[4];

    unsigned char bt_eval_amp[4];

    unsigned char bt_prcnt_good[4];

    unsigned int ref_lyr_min,
                 ref_lyr_near,
                 ref_lyr_far;

    unsigned int ref_layer_vel[4];

    unsigned char ref_corr[4];

    unsigned char ref_int[4];

    unsigned char ref_prcnt_good[4];

    unsigned int bt_max_depth;

    unsigned char rssi_amp[4];

    unsigned char gain;

    unsigned char bt_range_msb[4];
} DVLBottomTrackData;

/* This will hold *ALL* of the data from the DVL */
/* It should NOT be passed every time the sensor is polled */
typedef struct _CompleteDVLPacket
{
    int fixedleaderset;

    DVLHeaderData header;
    DVLFixedLeaderData fixedleader;
    DVLVariableLeaderData variableleader;
    DVLBottomTrackData btdata;

    unsigned short checksum;
} CompleteDVLPacket;

/* This is the info that will actually get passed to and from
   the API. */
typedef struct _RawDVLData
{
    /* Useful information in this structure? */
    /* Non-zero implies invalid data. */
    unsigned int valid;

    /* vvvvv PUT DATA HERE vvvvv */

    unsigned int bt_velocity[4];

    unsigned int ensemblenum;
    
    unsigned int year, month, day,
                 hour, min, sec, hundredth;

    /* ^^^^^ PUT DATA HERE ^^^^^ */

    CompleteDVLPacket *privDbgInf;
} RawDVLData;

/** Opens a serial channel to the imu using the given devices
 *
 *  @param  devName  Device filename
 *
 *  @return  The file descriptor of the device file, -1 on error.
 */
int openDVL(const char * devName);

/** Read the latest DVL measurements into the given structure
 *
 *  @param fd  The device file returned by openDVL
 */
int readDVLData(int fd, RawDVLData * imu);

// If we are compiling as C++ code we need to use extern "C" linkage
#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
    
#endif // RAM_DVLAPI_H_12_02_2009
