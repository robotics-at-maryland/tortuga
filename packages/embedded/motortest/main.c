#include <p30fxxxx.h>
#include <string.h>

_FOSC( CSW_FSCM_OFF & ECIO & PWRT_64);
// Turn off the watchdog
_FWDT ( WDT_OFF );

#define TRIS_OUT 0
#define TRIS_IN  1
#define byte unsigned char
#define BYTE byte

/* This sets up the i2c peripheral */
void initI2C(byte baud_rate) {
    // First set the i2c pins as inputs
    // The family reference manual says the module doesn't care, but I do.
    TRISFbits.TRISF2 = TRIS_IN;
    TRISFbits.TRISF3 = TRIS_IN;

    /* Turn i2c off */
    I2CCONbits.I2CEN= 0;

    // Set the baud rate. 
    I2CBRG= 0x0000 | baud_rate;

    //Now we will initialise the I2C peripheral for Master Mode, No Slew Rate
    //Control, and leave the peripheral switched off.
    I2CCON= 0x1200;
    I2CRCV= 0x0000;
    I2CTRN= 0x0000;
    //Now we can enable the peripheral

    I2CCON= 0x9200;
    return 0;   
}

void initUART() {
}

int main()
{
    byte i;
    ADPCFG = 0xFFFF;

    initI2C();
}
