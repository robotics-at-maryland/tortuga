#include <p30fxxxx.h>

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
#define BUF_SIZE 100

/**********************************************/
/* These are the prototypes for the functions */
/**********************************************/
void initSPI(byte);
void initUART(byte);
void initOSC(void);
void initADC(void);

void uartRXwait(void);
byte uartRX(void);
void writeUart(byte);


byte getNumChar(byte num) {
    if(num < 10)
        return '0' + num;
    return 'A' + num;
}

/* The main function sets everything up then loops */
int main(void)
{
    byte i, j, complete_packet, chksum;
    byte buff[BUF_SIZE];

    int temp_send;
 
    packetSize= 0;

    /* Set up the Oscillator */
    initOSC();

    /* Set up the ADCs*/
    initADC();

    // Initialize the SPI bus with the a clock frequency of
    // about 5MHz (give it a secondary prescaler of 2:1)
    initSPI(0x0B);

    /* Initialize the UART module */
    /* We set the baud to 9600 */
    initUART(0x0F);

    /* Set up port E as being a bank of outputs */
    TRISE= 0x0000;

    // Set up port B as being a bank of outputs and
    // output high on all of them. This should prevent any
    // of the SPI members from thinking we love them.
    TRISB= 0x0000;
    LATB= 0xFFFF;

    /* Now that we're done with all the setup, we're just going to do
     * a simple loop which takes input on the UART and stores it on PORTE */
    LATE= 0x0002;
    writeUart('O');
    writeUart('K');
    writeUart('\n');
    while(1) {
        complete_packet= 0;

        while(!complete_packet) {
            uartRXwait();
            buff[0]= i= uartRX();

            writeUart(getNumChar(i));

            j= 0;
            while(j++ < i) {
                uartRXwait();
                buff[j]= uartRX();
            }

            chksum= j= 0;
            while(j < i) {
                chksum+= buff[j];
                j++;
            }

            if(chksum == buff[i]) {
                writeUart('K');
                complete_packet= 0x01;
            } else {
                writeUart('N');
            }
        }

        /* Put what to do with the packet here. */

        // Trigger the Latch which corrosponds to the appropriate chip
        LATB | buff[1];

        temp_send= (((int)buff[2]) << 8) | buff[3];

    }

    return 0;
}

/* This function initializes the SPI bus with a given clock freq. */
void initSPI(byte thing) {
    SPI1CON= 0x0520 | thing;

    return;
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

/* This function initializes the Oscillator
   Currently written under the assumption we're using a dsPIC30F4011 */
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

/* This function *safely* writes a packet to the Uart1 output */
void writeUart(byte packet) {
    /* Wait for space to be available */
    while(U1STAbits.UTXBF)
        ;

    /* Send the packet! */
    U1TXREG= packet;
}
