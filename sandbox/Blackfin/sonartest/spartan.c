#include "spartan.h"


#define ADDR_ADCONFIG ADDR_ADConfig1
#define ADDR_ADPRESCALER ADDR_ADConfig2


int initADC()
{
    REG(ADDR_ADCONFIG) = 0x8000;
    //REG(ADDR_ADConfig1) = 0x4000;
    REG(ADDR_ADPRESCALER) = 0x0000;
    REG(ADDR_ADCONFIG) = 0x4044;
}
