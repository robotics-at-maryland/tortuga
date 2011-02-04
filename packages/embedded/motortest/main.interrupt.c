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

/* So the R/W bit is low for a write, and high for a read */
/* In other words, if the master sends data, use I2C_WRITE, */
/* if the master is receiving data use I2C_READ */
#define I2C_TIMEOUT 100000
#define I2C_READ 0x0001
#define I2C_WRITE 0x0000
#define I2C_NACK 1
#define I2C_ACK 0

/* Here's basically a definition of all the states the bus can be in.
 * The important thing is that the bus and the main function essentially work
 * in tandem based on the main function waiting for the bus to be in the
 * waiting state, and the buffer which holds the information to be sent to
 * the motor in question. */
#define I2CSTATE_IDLE   0x00
#define I2CSTATE_START  0x01
#define I2CSTATE_TRANS  0x02
#define I2CSTATE_RX_ADR 0x03
#define I2CSTATE_REC    0x04
#define I2CSTATE_ACK    0x05
#define I2CSTATE_STOP   0x06

// Here's the definitions of all the errors we can get:
#define I2CERR_WASIDLE   0x00
#define I2CERR_NACK      0x01
#define I2CERR_WRITEFAIL 0x02
#define I2CERR_NOTIDLE   0x03

/* This is an error state where something isn't ACK'd or some other bad state*/
#define I2CSTATE_STOPPING_BORKED 0xFD
#define I2CSTATE_BORKED 0xFE

#define I2C_WASACK (I2CSTATbits.ACKSTAT == I2C_ACK)

/* Here's the buffer and additional variables */
byte i2cState= I2CSTATE_IDLE;
byte i2cBuf[BUF_SIZE];
int i2cPtr= 0;
int packetSize= 0;

/**********************************************/
/* These are the prototypes for the functions */
/**********************************************/
void initI2C(byte);
void initUART(byte);
void initOSC(void);
void initADC(void);

void uartRXwait(void);
byte uartRX(void);
void writeUart(byte);

byte AckI2C(void);
unsigned int getI2C(void);
void StartI2C(void);
unsigned int RestartI2C(void);
unsigned int StopI2C(void);
unsigned int WriteI2C(byte);
unsigned int IdleI2C(void);

byte getNumChar(byte num) {
    if(num < 10)
        return '0' + num;
    return 'A' + num;
}

/* This function does whatever we need to do when we're borked. */
void BorkedI2C(byte error) {
    writeUart('B');
    writeUart(getNumChar(error));
    writeUart(getNumChar(i2cState));
    LATE= 0x0001;
    i2cState= I2CSTATE_STOPPING_BORKED;
    I2CCONbits.PEN = 1;        /* Generate the Stop condition */
}

/* This routine is called everytime that something happens on the i2c bus */
void _ISR _MI2CInterrupt() {
    // No matter what happens, remember to clear the interrupt flag!
    _MI2CIF= 0;

    /* The master i2c interrupt routine is just a big 'ol state machine
     * So get the current stte and figure out what the hell should be
     * happening.  Then do stuff based on that. */
    switch(i2cState) {
        case I2CSTATE_IDLE:
        {
            // If we're in this state we shouldn't be in this function!
            BorkedI2C(I2CERR_WASIDLE);
            break;
        }

        case I2CSTATE_START:
        {
            if((i2cBuf[0] & 0x01) == I2C_WRITE) {
                i2cPtr= 1;
                i2cState= I2CSTATE_TRANS;
            } else {
                // Have the pointer start at the beginning!
                i2cPtr= 0;
                i2cState= I2CSTATE_RX_ADR;
            }

            // Write the address and R/W bit to the bus!
            if(WriteI2C(i2cBuf[0])) {
                BorkedI2C(I2CERR_WRITEFAIL);
                break;
            }

            break;
        }

        case I2CSTATE_TRANS:
        {
            // Check to see if the packet was ACK'd or NACK'd
            if(!I2C_WASACK) {
                BorkedI2C(I2CERR_NACK); // We're fucked! Stop the bus!
                break;
            }

            // Have we finished sending the whole packet?
            if(i2cPtr >= packetSize) {
                // Yup! Clean up and wait for more.
                StopI2C();
                break;
            }

            /* If we've made it here we need to write the next bus to the line
             * and wait for all the nonsense again. */
            if(!IdleI2C()) {
                BorkedI2C(I2CERR_NOTIDLE); // We're Fucked! Stop the Bus!
                break;
            }

            WriteI2C(i2cBuf[i2cPtr++]);
            i2cState= I2CSTATE_TRANS;
            break;
        }

        case I2CSTATE_RX_ADR:
        {
            if(!I2C_WASACK) {
                BorkedI2C(I2CERR_NACK); /* We're fucked! Stop the bus! */
                break;
            }

            LATE= 0x0000;

            i2cState= I2CSTATE_REC;
            I2CCONbits.RCEN= 1;

            break;
        }

        case I2CSTATE_REC:
        {
            /* Recieving is a bit more complex then sending we have to
             * get the byte out of the buffer, then generate an ACK */
            LATE= 0x0002;
            i2cBuf[i2cPtr++]= I2CRCV;
            I2CSTATbits.I2COV= 0;

            if(i2cPtr < packetSize) {
                I2CCONbits.ACKDT= I2C_ACK;
            } else {
                I2CCONbits.ACKDT= I2C_NACK;
            }

            i2cState= I2CSTATE_ACK;
            I2CCONbits.ACKEN= 1;

            break;
        }

        case I2CSTATE_ACK:
        {
            /* So now we've received a whole packet and ACK'd it.  If we have
             * more packets we'll need to wait to recieve them.  So go check! */
            LATE= 0x0001;

            if(i2cPtr >= packetSize) {
                StopI2C(); /* Stop the bus! We got everything! */
                break;
            }

            /* If the above wasn't true then we have more packets.  Go back and
             * wait. */
            i2cState= I2CSTATE_REC;
            I2CCONbits.RCEN= 1; /* Enable reception! */
            break;
        }

        case I2CSTATE_STOP:
        {
            /* This is the end of the stop state. If we're here, in theory 
             * we got what we needed and we're done.  We're idle again! */
            LATE= 0x0000;
            i2cState= I2CSTATE_IDLE;
            break;
        }

        case I2CSTATE_STOPPING_BORKED:
        {
            /* If we're here that means that */
            i2cState= I2CSTATE_BORKED;
            break;
        }

        case I2CSTATE_BORKED:
        {
            /* If we're BORKED just break. */
            break;
        }

        default:
        {
            break;
        }
    }
}

/* The main function sets everything up then loops */
int main(void)
{
    byte i, j, complete_packet, chksum;
    byte buff[BUF_SIZE];
    long timeout;
 
    i2cState= I2CSTATE_IDLE;
    i2cPtr= 0;
    packetSize= 0;

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

        while(i2cState == I2CSTATE_STOPPING_BORKED)
            ;

        if(i2cState == I2CSTATE_BORKED) {
            writeUart('F');
            writeUart('B');
            writeUart('\n');
            i2cState= I2CSTATE_IDLE;
        }

        timeout= 0;
        while(i2cState != I2CSTATE_IDLE)
            if(timeout++ == I2C_TIMEOUT)
                continue;

        if((buff[1] & 0x01) == I2C_READ) {
            packetSize= buff[2];
            i2cBuf[0]= buff[1];
            StartI2C();
            while(i2cState != I2CSTATE_IDLE && i2cState != I2CSTATE_BORKED)
                ;

            /* Now we've got a packet! Push it out to the Uart */
            for(j= 0;j < packetSize;j++)
                writeUart(i2cBuf[j]);
        } else {
            for(j= 1;j < i;j++) {
                i2cBuf[j - 1]= buff[j];
            }

            packetSize= i - 1;

            StartI2C();
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

    IEC0bits.MI2CIE = 1;
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

/* This function *safely* writes a packet to the Uart1 output */
void writeUart(byte packet) {
    /* Wait for space to be available */
    while(U1STAbits.UTXBF)
        ;

    /* Send the packet! */
    U1TXREG= packet;
}


/**********************************\
|**********************************|
 * This section begins the I2C
 * functions for basic input and
 * output as the master.
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


void StartI2C(void)
{
    i2cState= I2CSTATE_START;  // Set the state machine up
    I2CCONbits.SEN = 1;        // Generate Start Condition
}

/* This function generates the restart condition and returns the timeout */
unsigned int RestartI2C(void)
{
    long timeout= 0;
    I2CCONbits.RSEN= 1; /* Generate the restart */

    while(I2CCONbits.RSEN)
        if(timeout++ == I2C_TIMEOUT)
            return 255;

    return 0;
}

/* This function generates the stop condition and reports a timeout */
unsigned int StopI2C(void) {
    i2cState= I2CSTATE_STOP;
    I2CCONbits.PEN = 1;        /* Generate the Stop condition */
    return 0;
}

/* This function transmits the byte passed to it over the i2c bus */
unsigned int WriteI2C(byte b) {
    /* So make sure there's space in the transmit buffer before we stick
     * anything in there */
    if(I2CSTATbits.TBF) {
        return 0xFF;
    }

    /* Jam the byte in the transmit buffer! */
    I2CTRN = b;
    return 0;
}

/* Returns 1 if the bus is idle, 0 otherwise */
unsigned int IdleI2C(void) {
    return !(I2CCONbits.SEN || I2CCONbits.PEN || I2CCONbits.RCEN || I2CCONbits.ACKEN || I2CSTATbits.TRSTAT);
}
