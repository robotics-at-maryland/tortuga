#include <stdio.h>

int main(int argc, char ** argv)
{
    unsigned long addr = 0;
    unsigned long count = 0;
    unsigned long i;

    unsigned short val;

    if(argc != 3)
    {
        printf("speedtest addr count\n");
        printf("all values are in hex. addr is 32bit. count is 32bit\n");
        return -1;
    }

    addr = strtol(argv[1], NULL, 16);
    count = strtol(argv[2], NULL, 16);

    printf("addr is 0x%08x\n", addr);
    printf("count is 0x%08x\n", count);

    for(i=0; i<count; i++)
    {
        val = *((unsigned short *) (addr));
    }

    printf("done\n");
    return 0; // hi joe
}
