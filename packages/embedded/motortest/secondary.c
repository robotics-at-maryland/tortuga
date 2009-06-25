#include <p30fxxxx.h>
#include <string.h>

/* Turn on the oscillator in XT mode so that it runs at the clock on
 * OSC1 and OSC2 */
_FOSC( CSW_FSCM_OFF & XT );

/* 64ms Power-up timer.  Or maybe micro seconds. Not important */
_FBORPOR( PWRT_64 );

/* Turn off the watchdog timer. */
_FWDT ( WDT_OFF );


/* Some defines which will help keep us sane. */
#define TRIS_OUT 0
#define TRIS_IN  1
#define byte unsigned char
#define BYTE byte

/* So the R/W bit is low for a write, and high for a read */
/* In other words, if the master sends data, use I2C_WRITE, */
/* if the master is receiving data use I2C_READ */
#define I2C_TIMEOUT 100000
#define I2C_READ 0x0001
#define I2C_WRITE 0x0000
#define NACK 1
#define ACK 0

/**********************************************/
/* These are the prototypes for the functions */
/**********************************************/
void initI2C(byte);
void initUART(byte);
void initOSC(void);
void initADC(void);
void uartRXwait(void);
byte uartRX(void);

byte AckI2C(void);
unsigned int getI2C(void);
unsigned int WriteI2C(byte);
unsigned int IdleI2C(void);
unsigned int WaitAck(void);
byte wasAck(void);
void waitAddr(void);
byte waitRX(void);

/* The main function sets everything up then loops */
int main()
{
    byte i;

    /* Set up the Oscillator */
    initOSC();

    /* Set up the ADCs*/
    initADC();

    /* The value of the equation given by the formula on the reference sheet is
     * 21.75 for a 10MHz clock (so a 2.5MHz FCY) running on a 100kHz i2c port.
     * Thus we set the Baud Rate Generator to 0x16 (22 in decimal) */
    initI2C(0x52);

    /* Initialize the UART module */
    /* We set the baud to 9600 */
    initUART(0x0F);

    /* Set up port E as being a bank of outputs */
    TRISE= 0x0000;

    /* Now that we're done with all the setup, we're just going to do
     * a simple loop which takes input on the UART and stores it on PORTE */
    LATE= 0x0002;
    while(1) {
        IFS0bits.SI2CIF= 0;
        while(!IFS0bits.SI2CIF)
            ;
        if(waitRX()) {    /* Get the data off the bus */
            LATE= 0x0004;
            continue;
        }
        i= I2CRCV;
        U1TXREG= i;  /* Chuck the byte to the serial port. */
    }

    return 0;
}

/* This sets up the i2c peripheral */
void initI2C(byte address) {
    // First set the i2c pins as inputs
    // The family reference manual says the module doesn't care, but I do.
    TRISFbits.TRISF2 = TRIS_IN;
    TRISFbits.TRISF3 = TRIS_IN;

    /* Turn i2c off */
    I2CCONbits.I2CEN= 0;

    /* Set the device address*/
    I2CADD= address;

    /* Now we will initialise the I2C peripheral for Slave Mode, No Slew Rate
     * Control, and leave the peripheral switched off. */
    I2CCON= 0x1200;
    I2CRCV= 0x0000;
    I2CTRN= 0x0000;

    /* Now we can enable the peripheral */
    I2CCON= 0x9200;
}

/* This function initializes the UART with the given baud */
void initUART(byte baud_rate) {
    /* Disable the UART before we mess with it */
    U1MODEbits.UARTEN= 0;

    /* Set the baud rate */
    U1BRG= 0x0000 | baud_rate;

    /* Set up the UART settings: 8 bits, 1 stop bit, no parity, alternate IO */
    U1MODE= 0x0C00;

    /* Everything that we need is set up, so go ahead and activate the UART */
    U1MODEbits.UARTEN= 1;

    /* Enable Transmission. This MUST BE DONE **AFTER** enabling the UART */
    U1STAbits.UTXEN= 1;
}

/* This function initializes the Oscillator */
/* Currently written under the assumption we're using a dsPIC30F4011 */
void initOSC() {
    /* Looking into it, the default settings are fine, so we're not going to
     * mess with the oscillator.  But I'll leave the function as a
     * placeholder */
}

/* This initializes the ADCs */
void initADC() {
    /* In case it isn't already off, kill the ADC module */
    ADCON1bits.ADON= 0;
    
    /* Disable the ADCs for now. This sets all ADC pins as
     * digital pins. */
    ADPCFG = 0xFFFF;
}

/* This function sits and wait for there to be a byte in the recieve buffer */
void uartRXwait() {
    /* Loop waiting for there to be a byte */
    while(!U1STAbits.URXDA)
        ;
}

/* This function grabs a byte off the recieve buffer and returns it*/
byte uartRX() {
    return U1RXREG;
}


/**********************************\
|**********************************|
 * This section begins the I2C
 * functions for basic input and
 * output as the slave.
|**********************************|
\**********************************/

/* This function atempts to ACK  */
byte AckI2C(void)
{
    I2CCONbits.ACKDT = 0;          /* Set for Ack */
    I2CCONbits.ACKEN = 1;          /* Enable the Ack */

    /* Now we have a little timeout loop while we wait for the 
     * Ack to be accepted */
    long timeout = 0;
    while(I2CCONbits.ACKEN)
        if(timeout++ == I2C_TIMEOUT)
            return 255;
    return 0;
}

/* This function grabs a single byte of the i2c bus */
unsigned int getI2C(void)
{
    long timeout = 0;
    while(!I2CSTATbits.RBF) {
        if(timeout++ == I2C_TIMEOUT)
            return 255;
    }
    return(I2CRCV);                //Return data in buffer
}

/* This function transmits the byte passed to it over the i2c bus */
unsigned int WriteI2C(byte b)
{
    long timeout=0;

    /* So make sure there's space in the transmit buffer before we stick
     * anything in there */
    while(I2CSTATbits.TBF) {
        if(timeout++ == I2C_TIMEOUT) {
            return 255;
        }
    }

    /* Jam the byte in the transmit buffer! */
    I2CTRN = b;
    return 0;
}

/* This is technically an inaccurate name.  This is not waiting for 
 * an Idle condition, it waits until we're not transmitting. */
unsigned int IdleI2C(void)
{
    long timeout= 0;

    /* Now we just loop until */
    while(I2CSTATbits.TRSTAT) {
        if(timeout++ == I2C_TIMEOUT) {
            return 255;
        }
    }
    return 0;
}

/* This function should oly be called after the master has done something
 * which should be ACK'd or NACK'd */
unsigned int WaitAck(void) {
    long timeout= 0;

    /* This function waits on the Master i2c interrupt flag because I couldn't
     * find anything else which was triggered by a NACK or ACK being written.
     * Anyone who can find a better method should feel free to implement it */

    /* We clear the interrupt flag we're waiting on, because a bunch of stuff
     * *could* have triggered it. */
    IFS0bits.MI2CIF= 0;
    while(!IFS0bits.MI2CIF) {
        if(timeout++ == I2C_TIMEOUT) {
            return 255;
        }
    }

    return 0;
}

/* This byte returns whether the previous byte was ACK'd 
 * returns 0 if the previous sent byte was NACK'd, non-0 otherwise */
byte wasAck(void) {
    return (I2CSTATbits.ACKSTAT == ACK);
}

/* This function waits for the master to send an address which matches */
void waitAddr(void) {
    while(!I2CSTATbits.D_A)
        ;
}

/* This function waits for a byte to get into the buffer */
byte waitRX(void) {
    long timeout= 0;
    while(!I2CSTATbits.RBF) {
        if(timeout++ == I2C_TIMEOUT)
            return 255;
    }

    return 0;
}
