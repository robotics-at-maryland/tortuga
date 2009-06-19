#include <p30fxxxx.h>
#include <string.h>

/* Turn on the oscillator in XT mode so that it runs at the clock on
 * OSC1 and OSC2 */
_FOSC( CSW_FSCM_OFF & XT);

// Turn off the watchdog
_FWDT ( WDT_OFF );

#define TRIS_OUT 0
#define TRIS_IN  1
#define byte unsigned char
#define BYTE byte

/* So the R/W bit is low for a write, and high for a read */
/* In other words, if the master sends data, use I2C_WRITE, */
/* if the master is receiving data use I2C_READ */
#define I2C_READ 0x0001
#define I2C_WRITE 0x0000

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

/* This function initializes the UART with the given baud */
void initUART(byte baud_rate) {
}

/* This initializes the Oscillator */
/* Currently written under the assumption we're using a dsPIC30F4011 */
void initOSC() {

}

int main()
{
    byte i;

    /* Set up the Oscillator */
    initOSC();

    /* Disable the ADCs for now. */
    ADPCFG = 0xFFFF;

    /* The value of the equation given by the formula on the reference sheet is
     * 21.75 for a 10MHz clock (so a 2.5MHz FCY) running on a 100kHz i2c port.
     * Thus we set the Baud Rate Generator to 0x16 (22 in decimal) */
    initI2C(0x16);


}
