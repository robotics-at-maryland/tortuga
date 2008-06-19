#include <stdio.h>

int main(int argc, char ** argv)
{
    signed short val;
    unsigned long addr = 0x202F0104;


    int g=0, e=0, l=0, ovr=0;
    unsigned long i=0;

    unsigned short c, p;


    p = *(volatile unsigned short *)(0x202F0020);

    for(i=0; i<0xA0000; i++)
    {
        c = *(volatile unsigned short *)(0x202F0020);

        if(c > p)
            g++;

        if(c == p)
            e++;

        if(c < p)
            l++;

        if(c == 0)
            ovr++;

        p = c;
    }

    printf("%d less\n%d equal\n%d greater\n%d overflow\n", l, e, g, ovr);

    return 0; // hi joe
}
