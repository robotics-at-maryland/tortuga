#include "spartan.h"


int initADC()
{
    REG(ADDR_ADConfig1) = 0x8000;
    REG(ADDR_ADConfig1) = 0x4000;
    //REG(ADDR_ADConfig1) = 0x4044;
    REG(ADDR_ADConfig2) = 0x0000;
}


int resetADC(unsigned short gainMode)
{
    REG(ADDR_ADConfig1) = 0x8000;
    REG(ADDR_ADConfig1) = gainMode;
    //REG(ADDR_ADConfig1) = 0x4044;
    REG(ADDR_ADConfig2) = 0x0000;
}

int setLED(int yellow, int green)
{
    if(yellow)
        yellowOn();
    else
        yellowOff();

    if(green)
        greenOn();
    else
        greenOff();
}

void yellowOn()
{
    REG(ADDR_LED) |= 0x02;
}

void yellowOff()
{
    REG(ADDR_LED) &= 0x01;
}

void greenOn()
{
    REG(ADDR_LED) |= 0x01;
}

void greenOff()
{
    REG(ADDR_LED) &= 0x02;
}
