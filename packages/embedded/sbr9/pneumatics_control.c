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

/* The relay defines should exactly match the ones in the schematic
 */
#define RELAY_1_TRIS  _TRISC15
#define RELAY_2_TRIS  _TRISB8
#define RELAY_3_TRIS  _TRISB7
#define RELAY_4_TRIS  _TRISB6
#define RELAY_5_TRIS  _TRISB5
#define RELAY_6_TRIS  _TRISB4
#define RELAY_7_TRIS  _TRISB3
#define RELAY_8_TRIS  _TRISB2
#define RELAY_9_TRIS  _TRISB1
#define RELAY_10_TRIS _TRISB0
#define RELAY_11_TRIS _TRISE0
#define RELAY_12_TRIS _TRISE1
#define RELAY_13_TRIS _TRISD3
#define RELAY_14_TRIS _TRISD2
#define RELAY_15_TRIS _TRISD0
#define RELAY_16_TRIS _TRISF6
#define RELAY_17_TRIS _TRISF5
#define RELAY_18_TRIS _TRISF4
#define RELAY_19_TRIS _TRISF1
#define RELAY_20_TRIS _TRISF0
#define RELAY_21_TRIS _TRISE5
#define RELAY_22_TRIS _TRISE4
#define RELAY_23_TRIS _TRISE3
#define RELAY_24_TRIS _TRISE2

#define RELAY_1       _LATC15
#define RELAY_2       _LATB8
#define RELAY_3       _LATB7
#define RELAY_4       _LATB6
#define RELAY_5       _LATB5
#define RELAY_6       _LATB4
#define RELAY_7       _LATB3
#define RELAY_8       _LATB2
#define RELAY_9       _LATB1
#define RELAY_10      _LATB0
#define RELAY_11      _LATE0
#define RELAY_12      _LATE1
#define RELAY_13      _LATD3
#define RELAY_14      _LATD2
#define RELAY_15      _LATD0
#define RELAY_16      _LATF6
#define RELAY_17      _LATF5
#define RELAY_18      _LATF4
#define RELAY_19      _LATF1
#define RELAY_20      _LATF0
#define RELAY_21      _LATE5
#define RELAY_22      _LATE4
#define RELAY_23      _LATE3
#define RELAY_24      _LATE2

/* Now for the logical names.  We have them in void-fill pairs */
#define VOID_PAIR_0  RELAY_13
#define VOID_PAIR_1  RELAY_14
#define VOID_PAIR_2  RELAY_15
#define VOID_PAIR_3  RELAY_16
#define VOID_PAIR_4  RELAY_17
#define VOID_PAIR_5  RELAY_18
#define VOID_PAIR_6  RELAY_19
#define VOID_PAIR_7  RELAY_20
#define VOID_PAIR_8  RELAY_21
#define VOID_PAIR_9  RELAY_22
#define VOID_PAIR_10 RELAY_23
#define VOID_PAIR_11 RELAY_24

#define FILL_PAIR_0  RELAY_1
#define FILL_PAIR_1  RELAY_2
#define FILL_PAIR_2  RELAY_3
#define FILL_PAIR_3  RELAY_4
#define FILL_PAIR_4  RELAY_5
#define FILL_PAIR_5  RELAY_6
#define FILL_PAIR_6  RELAY_7
#define FILL_PAIR_7  RELAY_8
#define FILL_PAIR_8  RELAY_9
#define FILL_PAIR_9  RELAY_10
#define FILL_PAIR_10 RELAY_11
#define FILL_PAIR_11 RELAY_12

/**********************************************/
/* These are the prototypes for the functions */
/**********************************************/
void initUART(byte);
void initOSC(void);

/* The UART related functions */
void uartRXwait(void);
byte uartRX(void);
void writeUart(byte);

/* The pneumatic control crap */
void voidPair(unsigned char pair);
void fillPair(unsigned char pair);
void turnOff(unsigned char pair);
void voidAll(void);
void offAll(void);

void extendPiston(unsigned char piston);
void retractPiston(unsigned char piston);

/* timer stuff */
void wait10ms(void);
void wait100ms(void);
void wait1s(void);

/*****************************************************************************/
/*****************************************************************************/
/* This is the end of the prototypes, defines, and other such setup nonsense */
/*****************************************************************************/
/*****************************************************************************/

/* The main function sets everything up then loops */
int main()
{
    unsigned char buff[10];
    unsigned int i;

    // Set up the Oscillator
    initOSC();

    // make all of the pins digital pins
    initADC();

    // Initialize the UART module
    // We set the baud to 9600
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
    RELAY_13= RELAY_OFF;
    RELAY_14= RELAY_OFF;
    RELAY_15= RELAY_OFF;
    RELAY_16= RELAY_OFF;
    RELAY_17= RELAY_OFF;
    RELAY_18= RELAY_OFF;
    RELAY_19= RELAY_OFF;
    RELAY_20= RELAY_OFF;
    RELAY_21= RELAY_OFF;
    RELAY_22= RELAY_OFF;
    RELAY_23= RELAY_OFF;
    RELAY_24= RELAY_OFF;

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
    RELAY_13_TRIS= TRIS_OUT;
    RELAY_14_TRIS= TRIS_OUT;
    RELAY_15_TRIS= TRIS_OUT;
    RELAY_16_TRIS= TRIS_OUT;
    RELAY_17_TRIS= TRIS_OUT;
    RELAY_18_TRIS= TRIS_OUT;
    RELAY_19_TRIS= TRIS_OUT;
    RELAY_20_TRIS= TRIS_OUT;
    RELAY_21_TRIS= TRIS_OUT;
    RELAY_22_TRIS= TRIS_OUT;
    RELAY_23_TRIS= TRIS_OUT;
    RELAY_24_TRIS= TRIS_OUT;

    writeUart('P');
    writeUart('N');
    writeUart('E');
    writeUart('U');

    // Wait for 0xDEAFBEEF
    while(1) {
        //Check for sync packet #1
        uartRXwait();

        writeUart('G');
        writeUart('\n');

        buff[0]= uartRX();
        if(buff[0] != 0xDE)
            continue;

        writeUart('D');
        writeUart('E');

        // Check for sync packet #2
        uartRXwait();
        buff[0]= uartRX();
        if(buff[0] != 0xAF)
            continue;

        writeUart('A');
        writeUart('F');

        // Check for sync packet #3
        uartRXwait();
        buff[0]= uartRX();
        if(buff[0] != 0xBE)
            continue;

        writeUart('B');
        writeUart('E');

        // Check for sync packet #4
        uartRXwait();
        buff[0]= uartRX();
        if(buff[0] != 0xEF)
            continue;

        writeUart('E');
        writeUart('F');

        break;
    }

    writeUart('K');
    
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
        if((buff[3] != buff[4])) {
            writeUart('N');
            writeUart('C');
            writeUart('H');
            writeUart('K');
            continue;
        }

        writeUart('K');

        // Is this the enable packet? Or the other kind of packet?
        if(buff[0] == 'V' && buff[1] == 'A' && buff[2] == '!') {
            voidAll();
        } else if(buff[0] == 'V' && buff[1] == 'O') {
            if(buff[2] > 11) {
                writeUart('N');
                writeUart('P');
                writeUart('A');
                writeUart('R');
            } else {
                voidPair(buff[2]);
            }
        } else if(buff[0] == 'F' && buff[1] == 'L') {
            if(buff[2] > 11) {
                writeUart('N');
                writeUart('P');
                writeUart('A');
                writeUart('R');
            } else {
                fillPair(buff[2]);
            }
        } else if(buff[0] == 'E' && buff[1] == 'X') {
            if(buff[2] > 6) {
                writeUart('N');
                writeUart('P');
                writeUart('A');
                writeUart('R');
            } else {
                extendPiston(buff[2]);
            }
        } else if(buff[0] == 'R' && buff[1] == 'T') {
            if(buff[2] > 6) {
                writeUart('N');
                writeUart('P');
                writeUart('A');
                writeUart('R');
            } else {
                retractPiston(buff[2]);
            }
        }  else if(buff[0] == 'T' && buff[1] == 'O') {
            if(buff[2] > 11) {
                writeUart('N');
                writeUart('P');
                writeUart('A');
                writeUart('R');
            } else {
                turnOff(buff[2]);
            }
        } else if(buff[0] == 'O' && buff[1] == 'F' && buff[2] == 'F') {
            offAll();
        } else {
            writeUart('N');
            writeUart('C');
            writeUart('M');
            writeUart('D');
        }
    }

    return 0;
}

/* The pneumatic control crap */
void voidPair(unsigned char pair)
{
    if(pair == 0) {
        if(FILL_PAIR_0 == RELAY_ON) {
            FILL_PAIR_0= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        VOID_PAIR_0= RELAY_ON;
    } else if(pair == 1) {
        if(FILL_PAIR_1 == RELAY_ON) {
            FILL_PAIR_1= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        VOID_PAIR_1= RELAY_ON;
    } else if(pair == 2) {
        if(FILL_PAIR_2 == RELAY_ON) {
            FILL_PAIR_2= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        VOID_PAIR_2= RELAY_ON;
    } else if(pair == 3) {
        if(FILL_PAIR_3 == RELAY_ON) {
            FILL_PAIR_3= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        VOID_PAIR_3= RELAY_ON;
    } else if(pair == 4) {
        if(FILL_PAIR_4 == RELAY_ON) {
            FILL_PAIR_4= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        VOID_PAIR_4= RELAY_ON;
    } else if(pair == 5) {
        if(FILL_PAIR_5 == RELAY_ON) {
            FILL_PAIR_5= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        VOID_PAIR_5= RELAY_ON;
    } else if(pair == 6) {
        if(FILL_PAIR_6 == RELAY_ON) {
            FILL_PAIR_6= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        VOID_PAIR_6= RELAY_ON;
    } else if(pair == 7) {
        if(FILL_PAIR_7 == RELAY_ON) {
            FILL_PAIR_7= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        VOID_PAIR_7= RELAY_ON;
    } else if(pair == 8) {
        if(FILL_PAIR_8 == RELAY_ON) {
            FILL_PAIR_8= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        VOID_PAIR_8= RELAY_ON;
    } else if(pair == 9) {
        if(FILL_PAIR_9 == RELAY_ON) {
            FILL_PAIR_9= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        VOID_PAIR_9= RELAY_ON;
    } else if(pair == 10) {
        if(FILL_PAIR_10 == RELAY_ON) {
            FILL_PAIR_10= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        VOID_PAIR_10= RELAY_ON;
    } else if(pair == 11) {
        if(FILL_PAIR_11 == RELAY_ON) {
            FILL_PAIR_11= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        VOID_PAIR_11= RELAY_ON;
    }

    /* If they gave an invalid pair do nothing */
}

void fillPair(unsigned char pair)
{
    if(pair == 0) {
        if(VOID_PAIR_0 == RELAY_ON) {
            VOID_PAIR_0= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        FILL_PAIR_0= RELAY_ON;
    } else if(pair == 1) {
        if(VOID_PAIR_1 == RELAY_ON) {
            VOID_PAIR_1= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        FILL_PAIR_1= RELAY_ON;
    } else if(pair == 2) {
        if(VOID_PAIR_2 == RELAY_ON) {
            VOID_PAIR_2= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        FILL_PAIR_2= RELAY_ON;
    } else if(pair == 3) {
        if(VOID_PAIR_3 == RELAY_ON) {
            VOID_PAIR_3= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        FILL_PAIR_3= RELAY_ON;
    } else if(pair == 4) {
        if(VOID_PAIR_4 == RELAY_ON) {
            VOID_PAIR_4= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        FILL_PAIR_4= RELAY_ON;
    } else if(pair == 5) {
        if(VOID_PAIR_5 == RELAY_ON) {
            VOID_PAIR_5= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        FILL_PAIR_5= RELAY_ON;
    } else if(pair == 6) {
        if(VOID_PAIR_6 == RELAY_ON) {
            VOID_PAIR_6= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        FILL_PAIR_6= RELAY_ON;
    } else if(pair == 7) {
        if(VOID_PAIR_7 == RELAY_ON) {
            VOID_PAIR_7= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        FILL_PAIR_7= RELAY_ON;
    } else if(pair == 8) {
        if(VOID_PAIR_8 == RELAY_ON) {
            VOID_PAIR_8= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        FILL_PAIR_8= RELAY_ON;
    } else if(pair == 9) {
        if(VOID_PAIR_9 == RELAY_ON) {
            VOID_PAIR_9= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        FILL_PAIR_9= RELAY_ON;
    } else if(pair == 10) {
        if(VOID_PAIR_10 == RELAY_ON) {
            VOID_PAIR_10= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        FILL_PAIR_10= RELAY_ON;
    } else if(pair == 11) {
        if(VOID_PAIR_11 == RELAY_ON) {
            VOID_PAIR_11= RELAY_OFF;
            wait10ms(); wait10ms();
        }
        FILL_PAIR_11= RELAY_ON;
    }

    /* If they gave an invalid pair do nothing */
}

void turnOff(unsigned char pair)
{
    if(pair == 0) {
        VOID_PAIR_0= RELAY_OFF;
        FILL_PAIR_0= RELAY_OFF;
    } else if(pair == 1) {
        VOID_PAIR_1= RELAY_OFF;
        FILL_PAIR_1= RELAY_OFF;
    } else if(pair == 2) {
        VOID_PAIR_2= RELAY_OFF;
        FILL_PAIR_2= RELAY_OFF;
    } else if(pair == 3) {
        VOID_PAIR_3= RELAY_OFF;
        FILL_PAIR_3= RELAY_OFF;
    } else if(pair == 4) {
        VOID_PAIR_4= RELAY_OFF;
        FILL_PAIR_4= RELAY_OFF;
    } else if(pair == 5) {
        VOID_PAIR_5= RELAY_OFF;
        FILL_PAIR_5= RELAY_OFF;
    } else if(pair == 6) {
        VOID_PAIR_6= RELAY_OFF;
        FILL_PAIR_6= RELAY_OFF;
    } else if(pair == 7) {
        VOID_PAIR_7= RELAY_OFF;
        FILL_PAIR_7= RELAY_OFF;
    } else if(pair == 8) {
        VOID_PAIR_8= RELAY_OFF;
        FILL_PAIR_8= RELAY_OFF;
    } else if(pair == 9) {
        VOID_PAIR_9= RELAY_OFF;
        FILL_PAIR_9= RELAY_OFF;
    } else if(pair == 10) {
        VOID_PAIR_10= RELAY_OFF;
        FILL_PAIR_10= RELAY_OFF;
    } else if(pair == 11) {
        VOID_PAIR_11= RELAY_OFF;
        FILL_PAIR_11= RELAY_OFF;
    }

    /* If they gave an invalid pair do nothing */
}

void extendPiston(unsigned char piston)
{
}

void retractPiston(unsigned char piston)
{
}

void voidAll(void)
{
    char i= 0;

    for(;i < 12;i++) {
        voidPair(i);
    }
}

void offAll(void)
{
    char i= 0;

    for(;i < 12;i++) {
        turnOff(i);
    }
}

/* This function initializes the UART with the given baud */
void initUART(byte baud_rate) {
    /* Disable the UART before we mess with it */
    U2MODEbits.UARTEN= 0;

    _TRISF4= TRIS_IN;

    /* Set the baud rate */
    U2BRG= 0x0000 | baud_rate;

    /* Set up the UART settings: 8 bits, 1 stop bit, no parity */
    U2MODE= 0x0000;

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
    ADPCFG= 0xFFFF;
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

void wait10ms(void)
{
    // set up timer 2
    T2CONbits.TON= 0; /* Turn off the timer before we mess with it */
    
    T2CON= 0x0020;   /* bits 5-4: 3->x64 prescaler */
    TMR2= 0x0000;    /* Clear timer */
    PR2= 288;        /* 7.37MHz/(4) = 1.8425MHz = Fcy
                        Fcy/64 = 28,789.0625 Hz = Freq-post-prescalar = Fpps
                        10ms * Fpps = 287.891 = PR2
                      */
    _T2IF= 0;        /* Clear interrupt flag */

    T2CONbits.TON= 1; /* Turn on the timer! */

    while(!_T2IF)
        ;

    T2CONbits.TON= 0; /* Turn off the timer before we mess with it */
}

void wait100ms(void)
{
    // set up timer 2
    T2CONbits.TON= 0; /* Turn off the timer before we mess with it */
    
    T2CON= 0x0020;   /* bits 5-4: 3->x64 prescaler */
    TMR2= 0x0000;    /* Clear timer */
    PR2= 2879;       /* 7.37MHz/(4) = 1.8425MHz = Fcy
                        Fcy/64 = 28,789.0625 Hz = Freq-post-prescalar = Fpps
                        100ms * Fpps = 2,878.91 = PR2
                      */
    _T2IF= 0;        /* Clear interrupt flag */

    T2CONbits.TON= 1; /* Turn on the timer! */

    while(!_T2IF)
        ;

    T2CONbits.TON= 0; /* Turn off the timer now that we're done */
}

void wait1s(void)
{
    // set up timer 2
    T2CONbits.TON= 0; /* Turn off the timer before we mess with it */
    
    T2CON= 0x0020;   /* bits 5-4: 3->x64 prescaler */
    TMR2= 0x0000;    /* Clear timer */
    PR2= 28789;      /* 7.37MHz/(4) = 1.8425MHz = Fcy
                        Fcy/64 = 28,789.0625 Hz = Freq-post-prescalar = Fpps
                        1s * Fpps = 28,789.1 = PR2
                      */
    _T2IF= 0;        /* Clear interrupt flag */

    T2CONbits.TON= 1; /* Turn on the timer! */

    while(!_T2IF)
        ;

    T2CONbits.TON= 0; /* Turn off the timer now that we're done */
}
