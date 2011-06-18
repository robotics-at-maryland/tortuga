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

void translateError(int errornum);

int main(int argc, char **argv)
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
    fd= openDVL(argv[1]);
    //fd= open(argv[1], O_RDONLY);

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

            retcode= -1;
            goto bail;
        }
    }

    printf("X-velocity: %d\n", dvl.xvel_btm);
    printf("Y-velocity: %d\n", dvl.yvel_btm);
    printf("Z-velocity: %d\n", dvl.zvel_btm);
    printf("E-velocity: %d\n", dvl.evel_btm);

    printf("beam1-range: %u\n", dvl.beam1_range);
    printf("beam2-range: %u\n", dvl.beam2_range);
    printf("beam3-range: %u\n", dvl.beam3_range);
    printf("beam4-range: %u\n", dvl.beam4_range);

    printf("TOFP: %d\n", dvl.TOFP_hundreths);

bail:
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
