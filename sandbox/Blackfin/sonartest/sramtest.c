#include <stdio.h>


#define SRAM_BASE 0x20300000
#define SRAM_SIZE 0x100000


int main(int argc, char ** argv)
{
    unsigned long i, t;
    int errCount=0;


    printf("Writing words...\n");
    for(i=0; i<SRAM_SIZE; i+=2)
    {
//         printf("Writing to %08x\n", (i+SRAM_BASE));
        *((unsigned short *) (i + SRAM_BASE)) = i;
    }

    printf("Reading words...\n");
    for(i=0; i<SRAM_SIZE; i+=2)
    {
        t = *((unsigned short *) (i + SRAM_BASE));
        if(t != (i & 0xFFFF))
        {
            printf("Mismatch at address %08x: expected %04x, got %04x\n", i, (i&0xFFFF), t);
            errCount++;
        }
    }

    printf("Writing bytes...\n");
    for(i=0; i<SRAM_SIZE; i++)
    {
        *((unsigned char *) (i + SRAM_BASE)) = i;
    }

    printf("Reading bytes...\n");
    for(i=0; i<SRAM_SIZE; i++)
    {
        t = *((unsigned char *) (i + SRAM_BASE));
        if(t != (i & 0xFF))
        {
            printf("Mismatch at address %08x: expected %02x, got %02x\n", i, (i&0xFF), t);
            errCount++;
        }
    }

    printf("Test completed.\n%d errors.\n", errCount);

    return 0; // hi joe
}
