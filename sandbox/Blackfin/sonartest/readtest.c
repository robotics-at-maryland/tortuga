#include <stdio.h>
#include <stdlib.h>
#include "spartan.h"

int main(int argc, char ** argv)
{
    signed short val;
    unsigned long addr;
    unsigned long count;
    register int i;

    initADC();


    signed short * samples1 = NULL;
    signed short * samples2 = NULL;
    signed short * samples3 = NULL;
    signed short * samples4 = NULL;
    signed short * samples5 = NULL;
    signed short * samples6 = NULL;
    signed short * samples7 = NULL;
    signed short * samples8 = NULL;



//     printf("alloc 1\n");
    samples1 = malloc(1280 * 1024 * 1);
//     printf("alloc 2\n");
    samples2 = malloc(1280 * 1024 * 1);
//     printf("alloc 3\n");
    samples3 = malloc(1280 * 1024 * 1);
//     printf("alloc 4\n");
    samples4 = malloc(1280 * 1024 * 1);
/*
    printf("alloc 5\n");
    samples5 = malloc(1280 * 1024 * 1);
    printf("alloc 6\n");
    samples6 = malloc(1280 * 1024 * 1);
    printf("alloc 7\n");
    samples7 = malloc(1280 * 1024 * 1);
    printf("alloc 8\n");
    samples8 = malloc(1280 * 1024 * 1);


*/
    if(samples1 == NULL || samples2 == NULL || samples3 == NULL || samples4 == NULL)
    {
        printf("couldn't malloc\n");
        return -1;
    }
/*
    if(samples5 == NULL || samples6 == NULL || samples7 == NULL || samples8 == NULL)
    {
        printf("couldn't malloc\n");
        return -1;
    }
*/

    if(argc < 2)
    {
        printf("readtest addr count\n");
        printf("all values are in hex. addr is 32bit. count is 32bit\n");
        return -1;
    }

    addr = strtol(argv[1], NULL, 16);
    count = strtol(argv[2], NULL, 16);

//     printf("reading for %d iterations from address 0x%08x\n", count, addr);

    register unsigned short lastCount, sampleCount;


    REG(ADDR_LED) = 0x02;

    int sd = 0;

    for(i=0; i<count; i++)
    {
        samples1[i] = REG(ADDR_FIFO_OUT1A); //*((volatile unsigned short *) addr);
        samples2[i] = REG(ADDR_FIFO_OUT1B);
        samples3[i] = REG(ADDR_FIFO_OUT1S); //REG(ADDR_ADC4);
        samples4[i] = REG(ADDR_FIFO_COUNT1A); //REG(ADDR_ADC6);

        while(REG(ADDR_FIFO_EMPTY1A) != 0);
    }

    REG(ADDR_LED) = 0x01;


    if(argc == 3)
    {
        for(i=0; i<count; i++)
            printf("A%d\n", samples1[i]);
        for(i=0; i<count; i++)
            printf("B%d\n", samples2[i]);
        for(i=0; i<count; i++)
            printf("C%d\n", samples3[i]);
        for(i=0; i<count; i++)
            printf("D%d\n", samples4[i]);
    }

//     printf("samples dropped: %d\n", sd);

    REG(ADDR_LED) = 0x00;

    return 0; // hi joe

}
