/**
 * @file src/livecat.c
 * Dump sonar waveforms live over TCP/IP, a la Netcat
 *
 * @author Leo Singer
 *
 */

#include "addresses.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>

#define BLOCKSIZE 128
#define SAMPLE_RATE 500000 /* Hertz */
#define FOUR_SECONDS_OF_DATA (4*SAMPLE_RATE/BLOCKSIZE)

int16_t block[BLOCKSIZE][4];

int main(int argc, char** argv)
{
    if (argc < 4)
    {
        fprintf(stderr, "usage: livecat <address> <port> <seconds>\n");
        return EXIT_FAILURE;
    }
    
    struct hostent* he = gethostbyname(argv[1]);
    if (!he)
    {
        perror("gethostbyname");
        return EXIT_FAILURE;
    }
    
    errno = 0;
    int port = atoi(argv[2]);
    if (port <= 0)
    {
        if (errno == 0)
            fprintf(stderr, "negative port\n");
        else
            perror("bad port");
        return EXIT_FAILURE;
    }
    
    errno = 0;
    double seconds = atof(argv[3]);
    if (seconds <= 0.0)
    {
        if (errno == 0)
            fprintf(stderr, "negative duration\n");
        else
            perror("bad duration");
        return EXIT_FAILURE;
    }
    
    unsigned long nBlocks = (unsigned long)(SAMPLE_RATE*seconds/BLOCKSIZE);
    
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr = *((struct in_addr*)he->h_addr);
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("socket");
        return EXIT_FAILURE;
    }
    
    fprintf(stderr, "About to collect %lu blocks of %d samples.\n", nBlocks, BLOCKSIZE);
    fprintf(stderr, "Waiting for connection...");
    fflush(stderr);
    while (connect(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr)) < 0)
        ; // Busy wait until connected
    
    fprintf(stderr, "CONNECTED.\nSending data...");
    fflush(stderr);
    
    // Initialize ADC
    REG(ADDR_ADCONFIG) = 0x8000;
    REG(ADDR_ADPRESCALER) = 0x0000;
    REG(ADDR_ADCONFIG) = 0x4044;
    
    int j;
    for (j = 0 ; j < nBlocks ; j ++)
    {
        int i;
        for (i = 0 ; i < BLOCKSIZE ; i ++)
        {
            while(REG(ADDR_FIFO_COUNT1A) < BLOCKSIZE)
                ; // Busy wait until samples available
            
            block[i][0] = REG(ADDR_FIFO_OUT1A);
            block[i][1] = REG(ADDR_FIFO_OUT1B);
            block[i][2] = REG(ADDR_FIFO_OUT2A);
            block[i][3] = REG(ADDR_FIFO_OUT2B);
        }
        
        send(sockfd, *block, sizeof(block), 0);
    }
    
    close(sockfd);
    
    fprintf(stderr,"DONE.\n");
    
    return 0;
}
