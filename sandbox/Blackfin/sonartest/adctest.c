#include <stdio.h>

int main(int argc, char ** argv)
{
    signed short val;
    unsigned long addr = 0x202F0104;


    int g=0, e=0, l=0, ovr=0;
    unsigned long i=0;

    register volatile unsigned short c, p, a, b;


    p = *(volatile unsigned short *)(0x202F0124);
    int j;
    for(i=0; i<0xA0000; i++)
    {
        for(j=0; j<10; j++)
            c+= *(volatile unsigned short *)(0x202F0124);
        c = *(volatile unsigned short *)(0x202F0114);
        c = (c >> 8) & 0x00FF;


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
