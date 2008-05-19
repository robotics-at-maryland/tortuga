#include <stdio.h>

int main(int argc, char ** argv)
{
    unsigned long addr = 0;
    unsigned short val = 0;

    if(argc == 1)
    {
        printf("mem [r|w] addr [val]\n");
        printf("all values are in hex\n");
        return -1;
    }

    addr = strtol(argv[2], NULL, 16);

    printf("addr is 0x%08x\n", addr);

    if(strcmp(argv[1], "r") == 0)
    {
        val = *((unsigned short *) (addr));
        printf("read: 0x%04x\n", val);
    }

    if(strcmp(argv[1], "w") == 0)
    {
        val = strtol(argv[3], NULL, 16);
        printf("writing: 0x%04x\n", val);
        *((unsigned short *) (addr)) = val;
    }

    printf("done\n");
    return 0; // hi joe
}
