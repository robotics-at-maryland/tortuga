#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv)
{
    signed short val;
    unsigned long addr;
    unsigned long count;
    unsigned long i;

    signed short * samples = NULL;


    samples = malloc(1024 * 1024 * 10); /* 10 megs */

    if(samples == NULL)
    {
        printf("couldn't malloc\n");
        return -1;
    }


    if(argc < 2)
    {
        printf("readtest addr count\n");
        printf("all values are in hex. addr is 32bit. count is 32bit\n");
        return -1;
    }

    addr = strtol(argv[1], NULL, 16);
    count = strtol(argv[2], NULL, 16);

    printf("reading for %d iterations from address 0x%08x\n", count, addr);

    for(i=0; i<count; i++)
        samples[i] = *((signed short *) (addr));

    if(argc == 3)
    {
        for(i=0; i<count; i++)
            printf("%d\n", samples[i]);
    }

    return 0; // hi joe

}
