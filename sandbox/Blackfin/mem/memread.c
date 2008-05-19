#include <stdio.h>

int main(int argc, char ** argv)
{
    int tmp;
    if(argc == 1)
    {
        printf("memread addr\n");
//         printf("c - continuously\n");
        return -1;
    }

    printf("address of tmp is %08x\n", &tmp);
    unsigned int addr;


    addr = atoi(argv[1]);
    printf("reading %08x\n", addr);

    int val;

    val = *((int*)(addr));

    printf("val is %08x\n", val);
    return 0; // hi joe
}
