#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv)
{
    signed short val;
    unsigned long addr;
    unsigned long count;
    unsigned long i;


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
//     samples2 = malloc(1280 * 1024 * 1);
//     printf("alloc 3\n");
//     samples3 = malloc(1280 * 1024 * 1);
//     printf("alloc 4\n");
//     samples4 = malloc(1280 * 1024 * 1);
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
/*    if(samples1 == NULL || samples2 == NULL || samples3 == NULL || samples4 == NULL)
    {
        printf("couldn't malloc\n");
        return -1;
    }*/
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

    unsigned short lastCount;

    for(i=0; i<count; i++)
    {
        samples1[i] = *((signed short *) (addr));

        while(lastCount == *((unsigned short *)0x202F0020));

//         if(lastCount+1 != *((unsigned short *)0x202F0020))
//             printf("sample dropped!\n");

        lastCount = *((unsigned short *)0x202F0020);
    }


    if(argc == 3)
    {
        for(i=0; i<count; i++)
            printf("%d\n", samples1[i]);
    }

    return 0; // hi joe

}
