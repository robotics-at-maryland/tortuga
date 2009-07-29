#include <stdio.h>
#include <stdlib.h>
#include "spartan.h"

#define BLOCK_SIZE 0xA0000


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


    count = strtol(argv[1], NULL, 16);

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

    if(count > BLOCK_SIZE)
    {
        fprintf(stderr, "alloc 5\n");
        samples5 = malloc(BLOCK_SIZE * 2);
        fprintf(stderr, "alloc 6\n");
        samples6 = malloc(BLOCK_SIZE * 2);
        fprintf(stderr, "alloc 7\n");
        samples7 = malloc(BLOCK_SIZE * 2);
        fprintf(stderr, "alloc 8\n");
        samples8 = malloc(BLOCK_SIZE * 2);

        if(samples5 == NULL || samples6 == NULL || samples7 == NULL || samples8 == NULL)
        {
            printf("couldn't malloc\n");
            return -1;
        }
    }


    REG(ADDR_LED) = 0x02;

    for(i=0; i<count; i++)
    {
        if(i < BLOCK_SIZE)
        {
            samples1[i] = REG(ADDR_FIFO_OUT1A);
            samples2[i] = REG(ADDR_FIFO_OUT1B);
            samples3[i] = REG(ADDR_FIFO_OUT2A);
            samples4[i] = REG(ADDR_FIFO_OUT2B);
        } else
        {
            samples5[i-BLOCK_SIZE] = REG(ADDR_FIFO_OUT1A);
            samples6[i-BLOCK_SIZE] = REG(ADDR_FIFO_OUT1B);
            samples7[i-BLOCK_SIZE] = REG(ADDR_FIFO_OUT2A);
            samples8[i-BLOCK_SIZE] = REG(ADDR_FIFO_OUT2B);
        }
        while(REG(ADDR_FIFO_EMPTY1A) != 0);
    }

    REG(ADDR_LED) = 0x01;

    int t;
    if(count > BLOCK_SIZE)
        t = BLOCK_SIZE;
    else
        t = count;

    if(argc == 2)
    {
        fprintf(stderr, "Sending as ASCII\n");

        for(i=0; i<t; i++)
            printf("A %d\n", samples1[i]);
        for(i=0; i<t; i++)
            printf("B %d\n", samples2[i]);
        for(i=0; i<t; i++)
            printf("C %d\n", samples3[i]);
        for(i=0; i<t; i++)
            printf("D %d\n", samples4[i]);

        if(count > BLOCK_SIZE)
        {
            t = count - BLOCK_SIZE;
            for(i=0; i<t; i++)
                printf("A %d\n", samples5[i]);
            for(i=0; i<t; i++)
                printf("B %d\n", samples6[i]);
            for(i=0; i<t; i++)
                printf("C %d\n", samples7[i]);
            for(i=0; i<t; i++)
                printf("D %d\n", samples8[i]);
        }
    } else
    {
        fprintf(stderr, "Sending as binary\n");
        for(i=0; i<t; i++)
        {
            putchar(samples1[i] & 0xFF);
            putchar((samples1[i] >> 8) & 0xFF);
            putchar(samples2[i] & 0xFF);
            putchar((samples2[i] >> 8) & 0xFF);
            putchar(samples3[i] & 0xFF);
            putchar((samples3[i] >> 8) & 0xFF);
            putchar(samples4[i] & 0xFF);
            putchar((samples4[i] >> 8) & 0xFF);
        }

        if(count > BLOCK_SIZE)
        {
            t = count - BLOCK_SIZE;
            for(i=0; i<t; i++)
            {
                putchar(samples5[i] & 0xFF);
                putchar((samples5[i] >> 8) & 0xFF);
                putchar(samples6[i] & 0xFF);
                putchar((samples6[i] >> 8) & 0xFF);
                putchar(samples7[i] & 0xFF);
                putchar((samples7[i] >> 8) & 0xFF);
                putchar(samples8[i] & 0xFF);
                putchar((samples8[i] >> 8) & 0xFF);
            }
        }

    }


//     printf("samples dropped: %d\n", sd);

    REG(ADDR_LED) = 0x00;

    return 0; // hi joe // hi steve, still watching svn?
}
