#include <p30fxxxx.h>
#include <stdio.h>
#include <string.h>
#include <outcompare.h>
#include <timer.h>
#include "servo.c"

/* Turn on the oscillator in XT mode so that it runs at the clock on
 * OSC1 and OSC2 */
_FOSC( CSW_FSCM_OFF & FRC );

/* 64ms Power-up timer.  Or maybe micro seconds. Not important */
_FBORPOR( PWRT_64 );

/* Turn off the watchdog timer. */
_FWDT ( WDT_OFF );

/* Turn on/off excess dumb code */
//#define DEBUG_ON

/* Some defines which will help keep us sane. */
#define TRIS_OUT 0
#define TRIS_IN  1
#define byte unsigned char
#define BYTE byte
#define BUF_SIZE 256

/**********************************************/
/* These are the prototypes for the functions */
/**********************************************/
void initUART(byte);
void initOSC(void);

/* The UART related functions */
void uartRXwait(void);
byte uartRX(void);
void writeUart(byte);

/*****************************************************************************/
/*****************************************************************************/
/* This is the end of the prototypes, defines, and other such setup nonsense */
/*****************************************************************************/
/*****************************************************************************/

/* The main function sets everything up then loops */
int main()
{
    unsigned char buff[10];
    unsigned int i, temp;

    // Set up the Oscillator
    initOSC();

    // Initialize the UART module
    // We set the baud to 9600
    //initUART(0x0F); // For 10MHz (the external osc, no PLL)
    initUART(0x0B); // For 7.37MHz (the internal frc osc)

    // Initialize the servo junk
    InitServos();

    // Turn on the LED
    _TRISB4= TRIS_OUT;
    _LATB4= 0;

    // Wait for 0xDEAFBEEF
    while(1) {
        //Check for sync packet #1
        uartRXwait();
        buff[0]= uartRX();
        if(buff[0] != 0xDE)
            continue;

        // Check for sync packet #2
        uartRXwait();
        buff[0]= uartRX();
        if(buff[0] != 0xAF)
            continue;

        // Check for sync packet #3
        uartRXwait();
        buff[0]= uartRX();
        if(buff[0] != 0xBE)
            continue;

        // Check for sync packet #4
        uartRXwait();
        buff[0]= uartRX();
        if(buff[0] != 0xEF)
            continue;

        break;
    }
    
    while(1) {
        i= 0;
        while(i < 4) {
            // Wait for a packet
            uartRXwait();
            // Buffer it
            buff[i++]= uartRX();
        }

        // Calculate checksum
        buff[4]= buff[0];
        buff[4]+= buff[1];
        buff[4]+= buff[2];

        // Check that the checksums are equal, then see if the servo number
        // makes sense or if this is an "Enable" command
        if((buff[3] != buff[4]) ||
                ( buff[0] > 3 && !(buff[0] == 'E' && buff[1] == 'N'))) {
            writeUart('N');
            continue;
        }

        writeUart('K');

        // Is this the enable packet? Or the other kind of packet?
        if(buff[0] == 'E' && buff[1] == 'N') {
            // Check the bitfield and enable and disable
            // the servos as appropriate
            if(buff[2] & 0x1) {
                EnableServo(0);
            } else {
                DisableServo(0);
            }

            if(buff[2] & 0x2) {
                EnableServo(1);
            } else {
                DisableServo(1);
            }

            if(buff[2] & 0x4) {
                EnableServo(2);
            } else {
                DisableServo(2);
            }

            if(buff[2] & 0x8) {
                EnableServo(3);
            } else {
                DisableServo(3);
            }
        } else {
            // This is a position, set the position!

            // Smash the 16 bit position into a short
            temp= buff[1];
            temp= temp << 8;
            temp|= buff[2];

            SetServo(buff[0], temp); // Actually set the servo
        }
    }

    return 0;
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

unsigned int uartRXCheck() {
    return U1STAbits.URXDA;
}

/* This function grabs a byte off the recieve buffer and returns it*/
byte uartRX() {
    return U1RXREG;
}

/* This function *safely* writes a packet to the Uart1 output */
void writeUart(byte packet) {
    /* Wait for space to be available */
    while(U1STAbits.UTXBF)
        ;

    /* Send the packet! */
    U1TXREG= packet;
}
