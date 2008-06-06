#include <stdio.h>

/* Where are we reading samples from? */
#define ADC_ADDR 0x202F0000

int main(int argc, char ** argv)
{
    signed short val;
    while(1)
    {
        val = *((signed short *) (ADC_ADDR));
	printf("%d\n", val);
	fflush(stdout);
    }
    return 0; // hi joe
}
