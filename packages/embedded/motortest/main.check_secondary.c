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
void writeStr(char *);
int writeSPI(unsigned char);
int readSPI(void);

byte getNumChar(byte num) {
    num= num & 0x0F;
    if(num < 10)
        return '0' + num;
    return 'A' - 10 + num;
}

void pWord(unsigned int toPrint) {
    writeUart(getNumChar(toPrint >> 12));
    writeUart(getNumChar(toPrint >> 8));
    writeUart(getNumChar(toPrint >> 4));
    writeUart(getNumChar(toPrint));
}

/* The main function sets everything up then loops */
int main(void) {
    unsigned char temp, temp2;

    /* Set up the Oscillator */
    initOSC();

    /* Set up the ADCs*/
    initADC();

    // Initialize the SPI bus with the a clock frequency of
    // about 5MHz (give it a secondary prescaler of 2:1)
    initSPI(0x001F);

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

    //////////////////////////////////////
    //////////////////////////////////////
    //////////////////////////////////////

    LATE= 0x0002;
    writeStr("OK\n");

    // Pause for a moment!
    temp= 0;
    while(++temp)
        ;

    while(1) {
        temp= 0;
        temp2= 1;
        while(temp != temp2) {
            uartRXwait();
            temp= uartRX();

            uartRXwait();
            temp2= uartRX();

            if(temp == temp2) {
                writeStr("0x");
                writeUart(getNumChar(temp >> 4));
                writeUart(getNumChar(temp));
                writeStr(" OK\n");
            } else {
                writeStr("N\n");
            }
        }

        LATB^= 0x0002;
        _SPI1IF= 0;
        writeSPI(temp);
        // Wait for the cycle to finish
        while(!_SPI1IF)
            ;

        pWord(readSPI() & 0xFF); // Read the RXB to prevent overflow

        LATB^= 0x0002;
    }

    return 0;
}

/* This function initializes the SPI bus with a given clock freq. */
void initSPI(byte thing) {
    // Disable the module
    SPI1STATbits.SPIEN= 0;

    // Clear any flags which may be set
    _SPI1IF= 0;
    SPI1STATbits.SPIROV= 0;

    // Master, not framed, active clock is high, idle clock low, 8 bit packets
    // Not Framed
    SPI1CONbits.FRMEN= 0;

    // We're the master, so ignore input on the frame sync pin
    SPI1CONbits.SPIFSD= 0;

    // SPI output pin is controlled by SPI module
    SPI1CONbits.DISSDO= 0;

    // 8 bit wide transmissions
    SPI1CONbits.MODE16= 0;

    // We sample at the middle of the clock cycle
    SPI1CONbits.SMP= 0;

    // We change our output data when we transition from an active clock to an idle clock
    SPI1CONbits.CKE= 0;

    // We're not in framed mode so we won't used !SS1
    SPI1CONbits.SSEN= 0;

    // Idle clock is low, active clock is high
    SPI1CONbits.CKP= 0;

    // We're the SPI bus master
    SPI1CONbits.MSTEN= 1;

    SPI1CON |= thing;

    // Turn the module on
    SPI1STATbits.SPIEN= 1;

    return;
}

/* This function initializes the UART with the given baud */
void initUART(byte baud_rate) {
    // Disable the UART before we mess with it
    U1MODEbits.UARTEN= 0;

    // Set the baud rate
    U1BRG= 0x0000 | baud_rate;

    // Set up the UART settings: 8 bits, 1 stop bit, no parity, alternate IO
    U1MODE= 0x0C00;

    // Everything that we need is set up, so go ahead and activate the UART
    U1MODEbits.UARTEN= 1;

    // Enable Transmission. This MUST BE DONE **AFTER** enabling the UART
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
    // In case it isn't already off, kill the ADC module
    ADCON1bits.ADON= 0;

    // Disable the ADCs for now. This sets all ADC pins as
    // digital pins.
    ADPCFG = 0xFFFF;
}

/* This function sits and wait for there to be a byte in the recieve buffer */
void uartRXwait() {
    // Loop waiting for there to be a byte
    while(!U1STAbits.URXDA)
        ;
}

/* This function grabs a byte off the recieve buffer and returns it*/
byte uartRX() {
    return U1RXREG;
}

/* This function *safely* writes a packet to the Uart1 output */
void writeUart(byte packet) {
    // Wait for space to be available
    while(U1STAbits.UTXBF)
        ;

    // Send the packet!
    U1TXREG= packet;
}

void writeStr(char *str) {
    int i= 0;
    while(str[i] != '\0') {
        writeUart(str[i]);
        i++;
    }
}


/* Does a safe write to the SPI tx buffer */
int writeSPI(unsigned char writeme) {
    // Make sure the TX buffer is empty
    if(SPI1STATbits.SPITBF)
        return 0xFF;

    SPI1BUF= writeme;

    return 0;
}

/* Just reads SPI1BUF */
int readSPI(void) {
    return SPI1BUF;
}
