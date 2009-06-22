#include <p30fxxxx.h>
#include <string.h>
#include "i2c_master.c"

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
void uartRXwait(void);
byte uartRX(void);

byte AckI2C(void);
unsigned int getI2C(void);
byte StartI2C(void);
unsigned int RestartI2C(void);
unsigned int StopI2C(void);
unsigned int WriteI2C(unsigned char);
unsigned int IdleI2C(void);

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
    initI2C(0x16);

    /* Initialize the UART module */
    /* We set the baud to 9600 */
    initUART(0x0F);

    /* Set up port E as being a bank of outputs */
    TRISE= 0x0000;

    /* Now that we're done with all the setup, we're just going to do
     * a simple loop which takes input on the UART and stores it on PORTE */
    LATE= 0x0002;
    while(1) {
        uartRXwait();
        i= uartRX();
        U1TXREG= i;
        
        LATE= 0x0000;
        IdleI2C();                        /* Wait for the i2c bus to be idle */
        StartI2C();                       /* Generate a start condition */
        Write((0x52 << 1) | I2C_WRITE);   /* Send a packet to address 0x52, informing it of a write */
        IdleI2C();                        /* Wait for the i2c bus to be idle */
        if(I2CSTATbits.ACKSTAT == NACK) { /* If we've gotten a NACK we're done, light up the red LED */
            LATE= 0x0004;
            continue;
        }

    }

    return 0;
}

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

    /* Now we will initialise the I2C peripheral for Master Mode, No Slew Rate
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
 * output as the master.
|**********************************|
\**********************************/

/* This function aatempts to ACK  */
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
    I2CCONbits.RCEN = 1;           //Enable Master receive
    Nop();

    long timeout = 0;
    while(!I2CSTATbits.RBF)
    {
        if(timeout++ == I2C_TIMEOUT)
            return 255;
    }
    return(I2CRCV);                //Return data in buffer
}


byte StartI2C(void)
{
    //This function generates an I2C start condition and returns status
    //of the Start.
    long timeout=0;
    I2CCONbits.SEN = 1;        //Generate Start COndition
    while(I2CCONbits.SEN)
        if(timeout++ == I2C_TIMEOUT)
            return 255;

    return 0;

    //return(I2C1STATbits.S);   //Optionally return status
}

unsigned int RestartI2C(void)
{
    //This function generates an I2C Restart condition and returns status
    //of the Restart.
    long timeout=0;
    I2CCONbits.RSEN = 1;       //Generate Restart

    while(I2CCONbits.RSEN)
        if(timeout++ == I2C_TIMEOUT)
            return 255;

    return 0;

    //return(I2C1STATbits.S);   //Optional - return status
}

unsigned int StopI2C(void)
{
    //This function generates an I2C stop condition and returns status
    //of the Stop.

    I2CCONbits.PEN = 1;        //Generate Stop Condition
    long timeout=0;
    while(I2CCONbits.PEN)
        if(timeout++ == I2C_TIMEOUT)
            return 255;
    return 0;

    //return(I2C1STATbits.P);   //Optional - return status
}

unsigned int WriteI2C(unsigned char b)
{
    // This function transmits the byte passed to the function
    I2CTRN = b;                 // Load byte to I2C1 Transmit buffer
    long timeout=0;
    while(I2CSTATbits.TBF)
        if(timeout++ == I2C_TIMEOUT)
            return 255;
    return 0;
}

unsigned int IdleI2C(void)
{
    long timeout=0;
    while(I2CSTATbits.TRSTAT)
    {
        if(timeout++ == I2C_TIMEOUT)
        {
            return 255;
        }
    }
    return 0;
}
