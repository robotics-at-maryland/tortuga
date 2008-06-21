#include "spartan.h"


int initADC()
{
    REG(ADDR_ADConfig1) = 0x8000;
    REG(ADDR_ADConfig1) = 0x4000;
    //REG(ADDR_ADConfig1) = 0x4044;
    REG(ADDR_ADConfig2) = 0x0000;
}
