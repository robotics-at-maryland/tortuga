#include <p30fxxxx.h>
#include <stdio.h>
#include <string.h>
#include <outcompare.h>
#include <timer.h>

/* Just use the internal oscillator */
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

#define RELAY_ON  1
#define RELAY_OFF 0

/* Pin defines! */
#define RELAY_1_TRIS  _TRISE5
#define RELAY_1       _LATE5
#define RELAY_2_TRIS  _TRISE4
#define RELAY_2       _LATE4
#define RELAY_3_TRIS  _TRISE3
#define RELAY_3       _LATE3
#define RELAY_4_TRIS  _TRISE0
#define RELAY_4       _LATE0
#define RELAY_5_TRIS  _TRISE1
#define RELAY_5       _LATE1
#define RELAY_6_TRIS  _TRISE2
#define RELAY_6       _LATE2
#define RELAY_7_TRIS  _TRISC14
#define RELAY_7       _LATC14
#define RELAY_8_TRIS  _TRISC13
#define RELAY_8       _LATC13
#define RELAY_9_TRIS  _TRISC15
#define RELAY_9       _LATC15
#define RELAY_10_TRIS _TRISF6
#define RELAY_10      _LATF6
#define RELAY_11_TRIS _TRISF1
#define RELAY_11      _LATF1
#define RELAY_12_TRIS _TRISF0
#define RELAY_12      _LATF0


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

    // Turn off all of the relays to start
    RELAY_1= RELAY_OFF;
    RELAY_2= RELAY_OFF;
    RELAY_3= RELAY_OFF;
    RELAY_4= RELAY_OFF;
    RELAY_5= RELAY_OFF;
    RELAY_6= RELAY_OFF;
    RELAY_7= RELAY_OFF;
    RELAY_8= RELAY_OFF;
    RELAY_9= RELAY_OFF;
    RELAY_10= RELAY_OFF;
    RELAY_11= RELAY_OFF;
    RELAY_12= RELAY_OFF;

    // Set up all of the pins
    RELAY_1_TRIS= TRIS_OUT;
    RELAY_2_TRIS= TRIS_OUT;
    RELAY_3_TRIS= TRIS_OUT;
    RELAY_4_TRIS= TRIS_OUT;
    RELAY_5_TRIS= TRIS_OUT;
    RELAY_6_TRIS= TRIS_OUT;
    RELAY_7_TRIS= TRIS_OUT;
    RELAY_8_TRIS= TRIS_OUT;
    RELAY_9_TRIS= TRIS_OUT;
    RELAY_10_TRIS= TRIS_OUT;
    RELAY_11_TRIS= TRIS_OUT;
    RELAY_12_TRIS= TRIS_OUT;

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
        } else {
        }
    }

    return 0;
}

/* This function initializes the UART with the given baud */
void initUART(byte baud_rate) {
    /* Disable the UART before we mess with it */
    U2MODEbits.UARTEN= 0;

    /* Set the baud rate */
    U2BRG= 0x0000 | baud_rate;

    /* Set up the UART settings: 8 bits, 1 stop bit, no parity, alternate IO */
    U2MODE= 0x0C00;

    /* Everything that we need is set up, so go ahead and activate the UART */
    U2MODEbits.UARTEN= 1;

    /* Enable Transmission. This MUST BE DONE **AFTER** enabling the UART */
    U2STAbits.UTXEN= 1;
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
    while(!U2STAbits.URXDA)
        ;
}

unsigned int uartRXCheck() {
    return U2STAbits.URXDA;
}

/* This function grabs a byte off the recieve buffer and returns it*/
byte uartRX() {
    return U2RXREG;
}

/* This function *safely* writes a packet to the Uart1 output */
void writeUart(byte packet) {
    /* Wait for space to be available */
    while(U2STAbits.UTXBF)
        ;

    /* Send the packet! */
    U2TXREG= packet;
}
