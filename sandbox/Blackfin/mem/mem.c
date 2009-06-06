#include <stdio.h>

int main(int argc, char ** argv)
{
    unsigned long addr = 0;

    if(argc == 1)
    {
        printf("mem [r|w|r4|w4|r8|w8] addr [val]\n");
        printf("all values are in hex. addr is 32bit. val is 16bit\n");
        return -1;
    }

//     printf("short is %d bytes\nint is %d bytes\nlong is %d bytes\nlong long is %d bytes\n",
//             sizeof(short), sizeof(int), sizeof(long), sizeof(long long));

    addr = strtol(argv[2], NULL, 16);

    printf("addr is 0x%08x\n", addr);

    if(strcmp(argv[1], "r") == 0)
    {
        unsigned short val = 0;
        val = *((unsigned short *) (addr));
        printf("read: 0x%04x\n", val);
    }

    if(strcmp(argv[1], "w") == 0)
    {
        unsigned short val = 0;
        val = strtol(argv[3], NULL, 16);
        printf("writing: 0x%04x\n", val);
        *((unsigned short *) (addr)) = val;
    }


    if(strcmp(argv[1], "r4") == 0)
    {
        unsigned int val = 0;
        val = *((unsigned int *) (addr));
        printf("read: 0x%08x\n", val);
    }

    if(strcmp(argv[1], "w4") == 0)
    {
        unsigned int val = 0;
        val = strtol(argv[3], NULL, 16);
        printf("writing: 0x%08x\n", val);
        *((unsigned int *) (addr)) = val;
    }

    if(strcmp(argv[1], "r8") == 0)
    {
        unsigned long long val = 0;

        val = *((unsigned long long *) (addr));
        printf("read: 0x%016x\n", val);
    }

    if(strcmp(argv[1], "w8") == 0)
    {
        unsigned long long val = 0;
        val = strtol(argv[3], NULL, 16);
        printf("writing: 0x%016x\n", val);
        *((unsigned long long *) (addr)) = val;
    }


    printf("done\n");
    return 0; // hi joe
}
