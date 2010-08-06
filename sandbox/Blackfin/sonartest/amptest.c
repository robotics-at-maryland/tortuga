#include <stdio.h>
#include <stdlib.h>
#include "spartan.h"

#define BLOCK_SIZE 0xA0000


signed short * samples1 = NULL;
signed short * samples2 = NULL;
signed short * samples3 = NULL;
signed short * samples4 = NULL;

signed short getSample(int ch, int idx)
{
    if(ch<0 || ch > 3)
    {
        printf("Channel %d doesn't exist!!\n", ch);
        REG(ADDR_LED) = 0x00;
        exit(1);
    }

    if(ch == 0)
        return samples1[idx];
    if(ch == 1)
        return samples2[idx];
    if(ch == 2)
        return samples3[idx];
    if(ch == 3)
        return samples4[idx];
}


int main(int argc, char ** argv)
{
    signed short val;
    unsigned long addr;
    unsigned long count;
    register int i;

    initADC();



//     count = strtol(argv[1], NULL, 16);

    fprintf(stderr, "alloc 1\n");
    samples1 = malloc(BLOCK_SIZE * 2);
    fprintf(stderr, "alloc 2\n");
    samples2 = malloc(BLOCK_SIZE * 2);
    fprintf(stderr, "alloc 3\n");
    samples3 = malloc(BLOCK_SIZE * 2);
    fprintf(stderr, "alloc 4\n");
    samples4 = malloc(BLOCK_SIZE * 2);

    if(samples1 == NULL || samples2 == NULL || samples3 == NULL || samples4 == NULL)
    {
        fprintf(stderr, "couldn't malloc\n");
        return -1;
    }



    count = 0x10000;


    while(1)
    {
        REG(ADDR_LED) = 0x02;
        for(i=0; i<count; i++)
        {
            if(i < BLOCK_SIZE)
            {
                samples1[i] = REG(ADDR_FIFO_OUT1A);
                samples2[i] = REG(ADDR_FIFO_OUT1B);
                samples3[i] = REG(ADDR_FIFO_OUT2A);
                samples4[i] = REG(ADDR_FIFO_OUT2B);
            }
            while(REG(ADDR_FIFO_EMPTY1A) != 0);
        }
        REG(ADDR_LED) = 0x01;

        int sa=0, sb=0, sc=0, sd=0;
        int min[4], max[4];
        int j=0;

        for(j=0; j<4; j++)
            max[j] = min[j] = getSample(j, 0);

        for(i=0; i<count; i++)
        {
            for(j=0; j<4; j++)
            {
                if(getSample(j, i) > max[j])
                    max[j] = getSample(j, i);

                if(getSample(j, i) < min[j])
                    min[j] = getSample(j, i);
            }
        }

        for(j=0; j<4; j++)
        {
            printf("Ch %d:\t Min: %06d\tMax: %06d\tRange: %06d\n", j, min[j], max[j], max[j]-min[j]);
        }
        printf("\n");

    }


    REG(ADDR_LED) = 0x00;

    return 0; // hi joe
}
