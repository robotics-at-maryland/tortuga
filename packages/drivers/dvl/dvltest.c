/**
  * DVLtest: Copyright Kit Sczudlo, 2010
  * this program should take 
  */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>

#include "include/dvlapi.h"

void printHeader(CompleteDVLPacket* pkt);
void printFixedLeader(CompleteDVLPacket* pkt);
void printVariableLeader(CompleteDVLPacket* pkt);
void printBTData(CompleteDVLPacket* pkt);

void translateError(int errornum);

int main(int argc, char ** argv)
{
    int fd, i, tmp, retcode= 0;
    RawDVLData dvl;

    /* Make sure we don't forget to free anything! */
    dvl.privDbgInf= NULL;

    /* Check that we have the appropriate number of arguments. */
    if( (argc != 2 && argc != 3) || (argc == 2 && strcmp(argv[1], "-h") == 0)) {
        printf("Usage: %s <location> [number]\n", argv[0]);
        printf("Opens <location> and feeds it into dvl api, then prints [number] packet.\n");
        return -1;
    }

    /* If we got a packet number, save it here */
    if(argc == 3) {
        if( (tmp= sscanf(argv[2], "%d", &i)) == EOF || tmp == 0) {
            printf("invalid packet number\n");
            return -1;
        }
    } else {
        /* Assume we want to check the first packet if we got no number */
        i= 1;
    }

    /* Open the file handle that we recieved */
    //fd= openDVL(argv[1]);
    fd= open(argv[1], O_RDONLY);

    /* Invalid file handle? Bail! */
    if(fd == -1) {
        printf("\nCould not find device!\n");

        retcode= -1;
        goto bail;
    }

    /* Go through the program, grabbing packets one at a time
       until we grabbed the number packet the user wanted. */
    for(;i > 0;i--) {

        /* Check to make sure that we actually read something */
        if( (tmp= readDVLData(fd, &dvl)) ) {
            translateError(tmp);

            //retcode= -1;
            //goto bail;
        }
    }

    /* Now we need to print everything! WOOOOOOOOOOOO! */
    printf("Header:\n");
    printHeader(dvl.privDbgInf);
    printf("\nFixed Leader:\n");
    printFixedLeader(dvl.privDbgInf);
    printf("\nVariable Leader:\n");
    printVariableLeader(dvl.privDbgInf);
    printf("\nBottom Track Data:\n");
    printBTData(dvl.privDbgInf);

bail:
    if(dvl.privDbgInf != NULL) {
        free(dvl.privDbgInf);
    }

    close(fd);

    return retcode;
}



void translateError(int errornum) {
    printf("Error: ");

    switch(errornum){
        case(ERR_NOSYNC):
            printf("No synchronization! (Too few packets?)\n");
            break;

        case(ERR_TOOBIG):
            printf("Packet too big!\n");
            break;

        case(ERR_BADFIXEDLEADER):
            printf("Bad fixed leader data!\n");
            break;

        case(ERR_BADVARIABLELEADER):
            printf("Bad Variable Leader ID!\n");
            break;

        case(ERR_CHKSUM):
            printf("Bad checksum\n");
            break;

        case(0):
            printf("No error! (This should never be printed!)\n");
            break;

        default:
            printf("Unknown error number!\n");
            break;
    }

    return;
}

/* This function prints all useful info in the header */
void printHeader(CompleteDVLPacket* pkt) {
    int tmp, i;

    printf("Synch bytes: 0x%x, 0x%x\n", (int)pkt->header.HeaderID, (int)pkt->header.DataSourceID);
    printf("Packet size: %d\n", pkt->header.PacketSize);
    printf("Number of data types: %d\n", pkt->header.num_datatypes);

    tmp= pkt->header.num_datatypes;

    for(i= 0;i < tmp; i++) {
        printf("Offset for datatype %d: 0x%02x\n", i, pkt->header.offsets[i]);
    }

    return;
}

void printFixedLeader(CompleteDVLPacket* pkt) {
    printf("Fixed leader ID: 0x%04x\n", (int)pkt->fixedleader.FixedLeaderID);
    printf("CPU firmware version/revision: %d/r%d\n",
            (int)pkt->fixedleader.CPU_Firmware_Version,
            (int)pkt->fixedleader.CPU_Firmware_Revision);
    printf("System config bits: 0x%04x\n", (int)pkt->fixedleader.System_Config);
    printf("Real/simulated Flag: %u (0 is real data)\n", (int)pkt->fixedleader.Real_Sim_flag);
    printf("Lag length: %u\n", (int)pkt->fixedleader.Lag_Length);
    printf("Number of beams: %u\n", (int)pkt->fixedleader.Num_Beams);
    printf("Number of cells: %u\n", (int)pkt->fixedleader.Num_Cells);

    printf("Pings per packet: %u\n", pkt->fixedleader.pings_per_packet);
    printf("Depth cell length: %u\n", pkt->fixedleader.depth_cell_length);
    printf("Blank after transmit: %u\n", pkt->fixedleader.blank_after_transmit);

    printf("Profiling mode: 0x%02x\n", (int)pkt->fixedleader.prof_mode);
    printf("Low correlation threshold: %u counts\n", (int)pkt->fixedleader.low_corr_thresh);
    printf("Number of code repetitions in transmit pulse: %u\n", (int)pkt->fixedleader.num_code_reps);

    printf("Percent good minimum: %u\n", (int)pkt->fixedleader.prcnt_good_min);

    printf("Error velocity maximum: %u mm/s\n", pkt->fixedleader.err_vel_max);

    printf("Time between pings: %02u min:%02u sec:%02u hundreths\n",
            (int)pkt->fixedleader.min_btwn_ping,
            (int)pkt->fixedleader.sec_btwn_ping,
            (int)pkt->fixedleader.hundredths_btwn_ping);

    printf("Coordinate transformation: ");
    /* We need to check bits marked AA in the following byte: xxxAAxxx
       anding with 0x18 should do the trick! */
    switch(pkt->fixedleader.coord_transform & 0x18) {
        case(0x18):
            printf("Earth Coordinates");
            break;

        case(0x10):
            printf("Ship Coordinates");
            break;

        case(0x08):
            printf("Instrument Coordinates");
            break;

        case(0x00):
            printf("No Transformation (Beam Coordinates)");
            break;

        default:
            printf("Error in transformation check!");
    }
    if(pkt->fixedleader.coord_transform & 0x04)
        printf(", tilts (pitch and roll) used in ship or earth tranformation");

    if(pkt->fixedleader.coord_transform & 0x02)
        printf(", 3 beam solution used if one beam is below correlation threshold");

    if(pkt->fixedleader.coord_transform & 0x01)
        printf(", bin mapping used");

    printf("\n");

    printf("Head alignment: %f degrees\n", (float)pkt->fixedleader.head_align / 100);
    printf("Head bias: %f degrees\n", (float)pkt->fixedleader.head_bias / 100);

    printf("Sensor source:\n");
    if(pkt->fixedleader.sen_source & 0x80)
        printf("\t* Calculates EC (speed of sound) from ED, ES, and ET\n");

    if(pkt->fixedleader.sen_source & 0x40)
        printf("\t* Uses ED from depth sensor\n");

    if(pkt->fixedleader.sen_source & 0x20)
        printf("\t* Uses EH from trandsucer heading sensor\n");

    if(pkt->fixedleader.sen_source & 0x10)
        printf("\t* Uses EP from transducer pitch sensor\n");

    if(pkt->fixedleader.sen_source & 0x08)
        printf("\t* Uses ER from transducer roll sensor\n");

    if(pkt->fixedleader.sen_source & 0x04)
        printf("\t* Uses ES (salinity) from conductivity sensor\n");

    if(pkt->fixedleader.sen_source & 0x02)
        printf("\t* Uses ET from transducer temperature sensor\n");

    if(pkt->fixedleader.sen_source & 0x01)
        printf("\t* Uses EU from transducer temperature sensor\n");

    printf("Sensor Availability:\n");
    printf("\t* External speed of sound sensor ");
    if(!(pkt->fixedleader.sen_avail & 0x80))
        printf("not ");
    printf("available\n");

    printf("\t* External depth sensor ");
    if(!(pkt->fixedleader.sen_avail & 0x40))
        printf("not ");
    printf("available\n");

    printf("\t* External heading sensor ");
    if(!(pkt->fixedleader.sen_avail & 0x20))
        printf("not ");
    printf("available\n");

    printf("\t* External pitch (tilt 1) sensor ");
    if(!(pkt->fixedleader.sen_avail & 0x10))
        printf("not ");
    printf("available\n");

    printf("\t* External roll (tilt 2) sensor ");
    if(!(pkt->fixedleader.sen_avail & 0x08))
        printf("not ");
    printf("available\n");

    printf("\t* External salinity sensor ");
    if(!(pkt->fixedleader.sen_avail & 0x04))
        printf("not ");
    printf("available\n");

    printf("\t* External temperature sensor ");
    if(!(pkt->fixedleader.sen_avail & 0x02))
        printf("not ");
    printf("available\n");

    printf("Bin 1 distance: %u cm\n", pkt->fixedleader.bin1_dist);
    printf("Transmit pulse length: %u cm\n", pkt->fixedleader.xmit_pulse_length);
    printf("Reference layer average: %u depth cells\n", pkt->fixedleader.ref_lyr_avg);

    printf("False target threshold: %u counts\n", (int)pkt->fixedleader.false_trgt_thresh);

    printf("Distance between pulse repetitions: %u cm\n", pkt->fixedleader.lagdist);

    printf("System bandwidth: %u\n", pkt->fixedleader.sys_bwidth);
    printf("System serial number: 0x%02x%02x%02x%02x\n", pkt->fixedleader.serial[3],
            pkt->fixedleader.serial[2], pkt->fixedleader.serial[1], pkt->fixedleader.serial[0]);
}

void printVariableLeader(CompleteDVLPacket* pkt) {
    printf("Variable leader ID: 0x%04x\n", (int)pkt->variableleader.varleaderID);
    printf("Ensemble Number: %u\n", (int)pkt->variableleader.ensemblenum);

    printf("Real-time clock info: %02u/%02u/%02u %02u:%02u:%02u %u hundredths\n",
            (int)pkt->variableleader.RTC_month, (int)pkt->variableleader.RTC_day,
            (int)pkt->variableleader.RTC_year, (int)pkt->variableleader.RTC_hour,
            (int)pkt->variableleader.RTC_minute, (int)pkt->variableleader.RTC_second,
            (int)pkt->variableleader.RTC_hundredths);

    printf("Ensemble number MSB: %02x\n", (int)pkt->variableleader.ensemble_num_msb);

    printf("BIT result:\n");

    if(pkt->variableleader.bit_result & 0x10)
        printf("\t* DEMOD 1 Error\n");

    if(pkt->variableleader.bit_result & 0x08)
        printf("\t* DEMOD 0 Error\n");

    if(pkt->variableleader.bit_result & 0x02)
        printf("\t* Timing Card Error\n");

    if(pkt->variableleader.bit_result == 0)
        printf("\t* Success!\n");

    printf("Speed of sound: %u m/s\n", pkt->variableleader.sound_speed);
    printf("Transducer depth: %u decimeters\n", pkt->variableleader.transducer_depth);
    printf("Heading: %f degrees\n", (float)pkt->variableleader.heading / 100);
    printf("Pitch: %f degrees\n", (float)pkt->variableleader.pitch / 100);
    printf("Roll: %f degrees\n", (float)pkt->variableleader.roll / 100);
    printf("Salinity: %u parts per thousand\n", pkt->variableleader.salinity);
    printf("Temperature: %f degrees\n", (float)pkt->variableleader.temp / 100);

    printf("Minimum pre-ping wait time: %02u minutes %02u seconds %02u hundredths\n",
            (int)pkt->variableleader.MPT_minutes,
            (int)pkt->variableleader.MPT_seconds,
            (int)pkt->variableleader.MPT_hundredths);

    printf("Heading standard deviation: %u degrees\n", (int)pkt->variableleader.HeadingStdDev);
    printf("Pitch standard deviation: %f degrees\n", (float)pkt->variableleader.PitchStdDev / 100);
    printf("Roll standard deviation: %f degrees\n", (float)pkt->variableleader.RollStdDev / 100);

    printf("ADC channel 1 (xmit voltage): %u\n", pkt->variableleader.dvl_adc[1]);

    printf("Error status: %04x\n", pkt->variableleader.errorstatus);

    printf("Pressure: %u deca-pascals\n", pkt->variableleader.pressure);
    printf("Pressure Variance: %u deca-pascals\n", pkt->variableleader.pressure_variance);
}

void printBTData(CompleteDVLPacket* pkt) {
    int i;

    printf("Bottom Track ID: 0x%04x\n", (int)pkt->btdata.BottomTrackID);
    printf("Pings per ensemble: %u\n", pkt->btdata.bt_pings_per_ensemble);
    printf("Delay before reaquire: %u ensembles\n", pkt->btdata.bt_delay_before_reaquire);

    printf("Correlation magnitude minimum: %u counts\n", (int)pkt->btdata.bt_corr_mag_min);
    printf("Evaluation amplitude minimum: %u counts\n", (int)pkt->btdata.bt_eval_amp_min);
    printf("Percent good minimum: %u\n", (int)pkt->btdata.bt_prcnt_good_min);
    printf("Bottom Track Mode: 0x%02x\n", (int)pkt->btdata.bt_mode);

    printf("Error velocity maximum: %u mm/s\n", pkt->btdata.bt_err_vel);

    for(i= 0;i < 4;i++)
        printf("Beam %u range: %u cm\n", i, pkt->btdata.bt_range[i]);

    for(i= 0;i < 4;i++)
        printf("Beam %u velocity: %u mm/s\n", i, pkt->btdata.bt_vel[i]);

    for(i= 0;i < 4; i++)
        printf("Colleation magnitude for beam %u: %u\n", i,
                (int)pkt->btdata.bt_beam_corr[i]);

    for(i= 0;i < 4; i++)
        printf("Evaluation amplitude for beam %u: %u counts\n", i,
                (int)pkt->btdata.bt_eval_amp[i]);

    for(i= 0;i < 4;i++)
        printf("Percent good for beam %u: %u\n", i,
                (int)pkt->btdata.bt_prcnt_good[i]);

    printf("Reference layer min/near/far: %u/%u/%u dm\n", pkt->btdata.ref_lyr_min,
            pkt->btdata.ref_lyr_near, pkt->btdata.ref_lyr_far);

    for(i= 0;i < 4;i++)
        printf("Reference layer velocity for beam %u: %u mm/s\n", i,
                pkt->btdata.ref_layer_vel[i]);

    for(i= 0;i < 4;i++)
        printf("Ref. layer correlation magnitude for beam %u: %u\n", i,
                pkt->btdata.ref_corr[i]);

    for(i= 0;i < 4;i++)
        printf("Ref. layer echo intensity for beam %u: %u\n", i,
                (int)pkt->btdata.ref_int[i]);

    for(i= 0;i < 4;i++)
        printf("Ref. layer percent good for beam %u: %u\n", i,
                (int)pkt->btdata.ref_prcnt_good[i]);

    printf("Maximum tracking depth: %u decimeters\n", pkt->btdata.bt_max_depth);

    for(i= 0;i < 4;i++)
        printf("RSSI for beam %u: %u counts or about %f db\n", i,
                (int)pkt->btdata.rssi_amp[i],
                (float)pkt->btdata.rssi_amp[i] * 0.45);

    printf("Gain level: %u\n", (int)pkt->btdata.gain);

    for(i= 0;i < 4;i++)
        printf("Range MSB for beam %u: 0x%02x\n", i, (int)pkt->btdata.bt_range_msb[i]);
}
