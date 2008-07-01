#include "spartan.h"
#include "dataset.h"

#define ADDR_ADCONFIG ADDR_ADConfig1
#define ADDR_ADPRESCALER ADDR_ADConfig2


int initADC()
{
    REG(ADDR_ADCONFIG) = 0x8000;
    //REG(ADDR_ADConfig1) = 0x4000;
    REG(ADDR_ADPRESCALER) = 0x0000;
    REG(ADDR_ADCONFIG) = 0x4044;
}

int captureSamples(struct dataset * s)
{
    int i=0, unit=0, offset=0;

    for(i=0; i < s->size; i++)
    {
        *(s->data[unit][0]+offset) = REG(ADDR_FIFO_OUT1A);
        *(s->data[unit][1]+offset) = REG(ADDR_FIFO_OUT1B);
        *(s->data[unit][2]+offset) = REG(ADDR_FIFO_OUT2A);
        *(s->data[unit][3]+offset) = REG(ADDR_FIFO_OUT2B);
        offset++;
        if(offset == ALLOC_UNIT_SIZE)
        {
            unit++;
            offset = 0;
        }
    }
    while(REG(ADDR_FIFO_EMPTY1A) != 0);
}
