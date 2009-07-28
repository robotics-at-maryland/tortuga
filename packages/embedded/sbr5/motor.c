#include <p30fxxxx.h>
#include <stdio.h>
#include <string.h>
#include <outcompare.h>
#include <timer.h>
#include "buscodes.h"
#include "servo.c"

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
#define BUF_SIZE 256

/* Debug! FUCK EVERYTHIGN! */
#define PING_FUCKED 0x25
#define PING_NORMAL 0x00
byte ping_val;

/* This defines how long we should hold off on starting up to let the motors
 * get completely started, preventing premature i2c errors. */
#define START_TIMEOUT    500000
/* This defines how many loop iterations before we consider the i2c bus to be
 * unresponsive (it takes 320 micro seconds to transmit 32 bits, and we're
 * running at 2.5MHz (FCY) which gives ~800 instructions.  2000 just to be on
 * the safe side) */
#define ERR_TIMEOUT      2000
/* How many loops should we keep an error LED on? */
#define SHOW_ERR_TIMEOUT 10

/* So the R/W bit is low for a write, and high for a read
 * In other words, if the master sends data, use I2C_WRITE,
 * if the master is receiving data use I2C_READ */
#define I2C_TIMEOUT 100000
#define I2C_READ 1
#define I2C_WRITE 0
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

/* This is an error state where something isn't ACK'd or some other bad state*/
#define I2CSTATE_STOPPING_BORKED 0xFD
#define I2CSTATE_BORKED          0xFE

/* Here are the states needed for stevebus communication */
#define STATE_TOP_LEVEL      0x00
#define STATE_SET_MOT_SPEEDS 0x01
#define STATE_SET_MOT_N      0x02

#define STATE_SERVO_ENABLE   0x03
#define STATE_SET_SERVO_POS  0x04

/* Timeout value */
// Specifies number of Tcy to wait for I2C ack before borking
#define BORK_TIMEOUT_PERIOD	20


/*
 * Bus = D8-D15
 * Req = A14
 * Akn = C13
 * RW  = C14
 */

/* Bus pin assignments */
#define REQ_INT_BIT (_INT3IE)
#define IN_REQ      _RA14
#define TRIS_REQ    _TRISA14

#define LAT_AKN     _LATC13
#define TRIS_AKN    _TRISC13

#define IN_RW       _RC14
#define TRIS_RW     _TRISC14

#define RW_READ     0
#define RW_WRITE    1

/* LED Definitions */
#define LED_ON      1
#define LED_OFF     0
#define TRIS_ERR    _TRISB6
#define LAT_ERR     _LATB6
#define TRIS_STA    _TRISA9
#define LAT_STA     _LATA9
#define TRIS_ACT    _TRISB7
#define LAT_ACT     _LATB7

/* Servo Pin definitions */
#define WAH_ON             1
#define WAH_OFF            0

#define TRIS_SERVO_WAH_INH _TRISA10
#define LAT_SERVO_WAH_INH  _LATA10

#define SERVO_ENABLED      0
#define SERVO_DISABLED     1

#define TRIS_SERVO_01_EN   _TRISF1
#define LAT_SERVO_01_EN    _LATF1
#define TRIS_SERVO_23_EN   _TRISF0
#define LAT_SERVO_23_EN    _LATF0
#define TRIS_SERVO_45_EN   _TRISG1
#define LAT_SERVO_45_EN    _LATG1
#define TRIS_SERVO_67_EN   _TRISG0
#define LAT_SERVO_67_EN    _LATG0

#define LAT_SERVO_0_EN     LAT_SERVO_01_EN
#define LAT_SERVO_1_EN     LAT_SERVO_01_EN
#define LAT_SERVO_2_EN     LAT_SERVO_23_EN
#define LAT_SERVO_3_EN     LAT_SERVO_23_EN
#define LAT_SERVO_4_EN     LAT_SERVO_45_EN
#define LAT_SERVO_5_EN     LAT_SERVO_45_EN
#define LAT_SERVO_6_EN     LAT_SERVO_67_EN
#define LAT_SERVO_7_EN     LAT_SERVO_67_EN


/* Here are some motor board variables */
byte motorSpeed[6];
unsigned int servoSpeed[8];

/* Here's the buffer for i2c and additional variables */
byte i2cState= I2CSTATE_IDLE;
byte i2cBuf[BUF_SIZE];
unsigned int i2cPtr= 0;
unsigned int packetSize= 0;
unsigned int i2c_timeout;

/* Here's the Stevebus buffer and other variables */
byte busState= STATE_TOP_LEVEL;
byte nParam= 0;
byte rxBuf[BUF_SIZE];
byte txBuf[BUF_SIZE];
unsigned int rxPtr= 0;
unsigned int txPtr= 0;

unsigned int tmp_servo_speed;

/**********************************************/
/* These are the prototypes for the functions */
/**********************************************/
void initI2C(byte);
void initUART(byte);
void initOSC(void);
void initADC(void);

/* The UART related functions */
void uartRXwait(void);
byte uartRX(void);
void writeUart(byte);

/* The I2C related functions */
byte AckI2C(void);
unsigned int getI2C(void);
void StartI2C(void);
unsigned int RestartI2C(void);
unsigned int StopI2C(void);
unsigned int WriteI2C(byte);
unsigned int IdleI2C(void);
unsigned int WaitAck(void);
unsigned int wasAckI2C(void);
void resetI2C_registers(void);

/* The steveBus related functions */
void checkBus(void);
void freeBus(void);
byte readBus(void);
void writeBus(byte);
void processData(byte);

/*****************************************************************************/
/*****************************************************************************/
/* This is the end of the prototypes, defines, and other such setup nonsense */
/*****************************************************************************/
/*****************************************************************************/


/* Goes through and sets all motors to 0 */
void kill_motors() {
    motorSpeed[0]= 0x80; /* Set all speeds to 0 */
    motorSpeed[1]= 0x80; /* Set all speeds to 0 */
    motorSpeed[2]= 0x80; /* Set all speeds to 0 */
    motorSpeed[3]= 0x80; /* Set all speeds to 0 */
    motorSpeed[4]= 0x80; /* Set all speeds to 0 */
    motorSpeed[5]= 0x80; /* Set all speeds to 0 */

    return;
}

/* This function does whatever we need to do when we're borked. */
void BorkedI2C() {
    i2cState= I2CSTATE_STOPPING_BORKED;
    StopI2C();        /* Generate the Stop condition */
}

/* This routine is called everytime that something happens on the i2c bus */
void _ISR _MI2CInterrupt() {
    /* No matter what happens, remember to clear the interrupt flag! */
    _MI2CIF= 0;

    /* The master i2c interrupt routine is just a big 'ol state machine
     * So get the current state and figure out what the hell should be
     * happening.  Then do stuff based on that. */
    //writeUart('0' + i2cState);
    switch(i2cState) {
        case I2CSTATE_IDLE:
        {
            /* If we're in this state we shouldn't be in this function! */
            //BorkedI2C();
            break;
        }

        case I2CSTATE_START:
        {
            if((i2cBuf[0] & 0x01) == I2C_WRITE) {
                i2cPtr= 1;
                i2cState= I2CSTATE_TRANS;
            } else {
                /* So, this transition is a bit odd, we set the pointer, then
                 * we have to make sure some slave ACK'd the address, THEN we
                 * can wait for bytes */
                i2cPtr= 0;
                i2cState= I2CSTATE_RX_ADR;
            }

            WriteI2C(i2cBuf[0]); /* No matter what happens we want to send the
                                    address and the R/W bit */
            break;
        }

        case I2CSTATE_TRANS:
        {
            /* Check to see if the packet was ACK'd or NACK'd */
            if(!wasAckI2C()) {
                BorkedI2C(); /* We're fucked! Stop the bus! */
                break;
            }

            /* Have we finished sending the whole packet? */
            if(i2cPtr >= packetSize) {
                /* Yup! Clean up and wait for more. */
                StopI2C();
                i2cState= I2CSTATE_STOP;
                break;
            }

            /* If we've made it here we need to write the next bus to the line
             * and wait for all the nonsense again. */
            if(!IdleI2C()) {
                BorkedI2C(); /* We're Fucked! Stop the Bus! */
                break;
            }

            /* If we made it here write the next byte to the bus */
            WriteI2C(i2cBuf[i2cPtr++]);
            i2cState= I2CSTATE_TRANS;
            break;
        }

        case I2CSTATE_RX_ADR:
        {
            if(!wasAckI2C()) {
                BorkedI2C(); /* We're fucked! Stop the bus! */
                break;
            }

            i2cState= I2CSTATE_REC;
            I2CCONbits.RCEN= 1;

            break;
        }

        case I2CSTATE_REC:
        {
            /* Recieving is a bit more complex then sending we have to
             * get the byte out of the buffer, then generate an ACK or NACK */
            i2cBuf[i2cPtr++]= I2CRCV;
            I2CCONbits.ACKDT= (i2cPtr >= packetSize ? I2C_ACK : I2C_NACK);
            I2CCONbits.ACKEN= 1;
            i2cState= I2CSTATE_ACK;
            break;
        }

        case I2CSTATE_ACK:
        {
            /* So now we've received a whole packet and ACK'd it.  If we have
             * more packets we'll need to wait to recieve them.  So go check! */
            if(i2cPtr >= packetSize) {
                StopI2C(); /* Stop the bus! We got everything! */
                i2cState= I2CSTATE_STOP;
                break;
            }

            /* If the above wasn't true then we have more packets.  Go back and
             * wait. */
            I2CCONbits.RCEN= 1; /* Enable reception! */
            i2cState= I2CSTATE_REC;
            break;
        }

        case I2CSTATE_STOP:
        {
            /* This is the end of the stop state. If we're here, in theory 
             * we got what we needed and we're done.  We're idle again! */
            i2cState= I2CSTATE_IDLE;
            break;
        }

        case I2CSTATE_STOPPING_BORKED:
        {
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

/* Put the bus in the high impedence state */
void freeBus() {
    TRISD= TRISD | 0xFF00;
    TRIS_AKN= TRIS_IN;
}

/* Make the bus pins into outputs and put the data on those pins */
void writeBus(byte data) {
    TRISD= TRISD & 0x00FF; /* Make the data pins outputs */

    LATD= (LATD & 0x00FF);
    LATD= LATD | (((unsigned int) data) << 8);
}

/* Get the data off the bus */
byte readBus() {
    return (0xFF & (PORTD >> 8));
}

/* Checks to see what data is and isn't on the Bus */
void checkBus() {
    byte data;

    /* This chunk helps in avoiding glitches,
     * if the IRQ is low? don't do anything! */
    if(IN_REQ == 0)
        return;

    if(IN_RW == RW_WRITE) {
        /* Read the data */
        data= readBus();

        /* Acknowledge the read */
        LAT_AKN= 1;
        TRIS_AKN= TRIS_OUT;

        /* Wait for the master to stop requesting things */
        while(IN_REQ)
            ;

        /* Let go of the ACK pin */
        TRIS_AKN= 1;

        /* Process the data we got from the master*/
        processData(data);
    } else {
        /* Queue up the byte to be sent */
        data= txBuf[txPtr++];

        /* Stick in on the data pins */
        writeBus(data);

        /* Tell the master the data is ready and on the pins */
        LAT_AKN= 1;
        TRIS_AKN= TRIS_OUT;

        /* Wait for the master to acknowledge the fact that he's got our stuff */
        while(IN_REQ)
            ;

        /* Let go of everything! */
        freeBus();
    }
}

/* If Master writes us data, this gets called */
void processData(byte data)
{
    unsigned int i;
    txPtr = 0;

    switch(busState)
    {
        case STATE_TOP_LEVEL:     /* New commands */
        {
            switch(data)
            {
                case BUS_CMD_PING:
                {
                    txBuf[0]= ping_val;
                    ping_val= PING_FUCKED;
                    break;
                }

                case BUS_CMD_ID:
                {
                    txBuf[0] = sprintf(txBuf+1, "MTR SRV TSN");
                    break;
                }

                case BUS_CMD_SET_MOT_SPEEDS:
                {
                    nParam= 0;
                    busState= STATE_SET_MOT_SPEEDS;
                    break;
                }

                case BUS_CMD_KILL_MOTORS:
                {
                    kill_motors();
                    break;
                }

                case BUS_CMD_SET_MOT_N:
                {
                    nParam= 0;
                    busState= STATE_SET_MOT_N;
                    break;
                }

                case BUS_CMD_SERVO_POWER_ON:
                {
                    LAT_SERVO_WAH_INH= WAH_ON;
                    break;
                }

                case BUS_CMD_SERVO_POWER_OFF:
                {
                    LAT_SERVO_WAH_INH= WAH_OFF;
                    break;
                }

                case BUS_CMD_SERVO_ENABLE:
                {
                    busState= STATE_SERVO_ENABLE;
                    break;
                }

                case BUS_CMD_SET_SERVO_POS:
                {
                    nParam= 0;
                    busState= STATE_SET_SERVO_POS;
                    break;
                }
            }

            break;
        }

        case BUS_CMD_SET_SERVO_POS:
        {
            rxBuf[nParam++]= data;

            if(nParam == 3) {
                /* Consolidate the two speed bytes into one short */
                tmp_servo_speed= rxBuf[1];
                tmp_servo_speed= (tmp_servo_speed << 8) | rxBuf[2];

                /* Actually hand the data to the interrupts */
                SetServo(rxBuf[0], tmp_servo_speed);

                /* We're done! Go back to the beginning! */
                busState= STATE_TOP_LEVEL;
            }

            break;
        }

        case BUS_CMD_SERVO_ENABLE:
        {
            /* Servo stuff for 0 and 1 */
            if((data & 0x01)) {
                LAT_SERVO_0_EN= SERVO_ENABLED;
                EnableServo(0x00);
            } else {
                DisableServo(0x00);
            }

            if((data & 0x02)) {
                LAT_SERVO_1_EN= SERVO_ENABLED;
                EnableServo(0x01);
            } else {
                DisableServo(0x01);
            }

            if(!(data & 0x01) && !(data & 0x02)) {
                LAT_SERVO_01_EN= SERVO_DISABLED;
            }

            /* Servo stuff for 2 and 3 */

            if((data & 0x04)) {
                LAT_SERVO_2_EN= SERVO_ENABLED;
                EnableServo(0x02);
            } else {
                DisableServo(0x02);
            }

            if((data & 0x08)) {
                LAT_SERVO_3_EN= SERVO_ENABLED;
                EnableServo(0x03);
            } else {
                DisableServo(0x03);
            }

            if(!(data & 0x04) && !(data & 0x08)) {
                LAT_SERVO_23_EN= SERVO_DISABLED;
            }

            /* Servo stuff for 4 and 5 */

            if((data & 0x10)) {
                LAT_SERVO_4_EN= SERVO_ENABLED;
                EnableServo(0x04);
            } else {
                DisableServo(0x04);
            }

            if((data & 0x20)) {
                LAT_SERVO_5_EN= SERVO_ENABLED;
                EnableServo(0x05);
            } else {
                DisableServo(0x05);
            }

            if(!(data & 0x10) && !(data & 0x20)) {
                LAT_SERVO_45_EN= SERVO_DISABLED;
            }

            /* Servo stuff for 6 and 7 */

            if((data & 0x40)) {
                LAT_SERVO_6_EN= SERVO_ENABLED;
                EnableServo(0x06);
            } else {
                DisableServo(0x06);
            }

            if((data & 0x80)) {
                LAT_SERVO_7_EN= SERVO_ENABLED;
                EnableServo(0x07);
            } else {
                DisableServo(0x07);
            }

            if(!(data & 0x40) && !(data & 0x80)) {
                LAT_SERVO_67_EN= SERVO_DISABLED;
            }

            break;
        }

        /* Setting motor speeds is straight forward, just grab the packet,
         * check the checksum, check the values, then write it to the speed
         * array.  The loop will start setting the speeds. */
        case STATE_SET_MOT_SPEEDS:
        {
            /* Save each byte in the recieve buffer */
            rxBuf[nParam++]= data;

            if(nParam >= 6) {
                /* Then go through and set each motor to that speed */
                for(i= 0;i < 6;i++) {

                    /* If we get any bad speeds... */
                    if(rxBuf[i] > 0xE6 || rxBuf[i] < 0x19) {
                        kill_motors();
                        break;
                    }

                    /* If the speed is good, write it to the right slot */
                    motorSpeed[i]= rxBuf[i];
                }

                /* We're done setting the speeds! */
                busState= STATE_TOP_LEVEL;
            }

            break;
        }

        case STATE_SET_MOT_N:
        {
            rxBuf[nParam++]= data;

            if(nParam >= 2) {
                /* Did we get a bad state? */
                if(rxBuf[1] > 0xE6 || rxBuf[1] < 0x19 || rxBuf[0] > 5) {
                    kill_motors(); 
                    break;
                }

                /* Set motor N to the speed defined */
                motorSpeed[rxBuf[0]]= rxBuf[1];

                /* We're done setting speeds! */
                busState= STATE_TOP_LEVEL;
            }

            break;
        }
    }
}

/* This is the interrupt vector for getting things off the bus */
void _ISR _INT3Interrupt() {
    /* CLEAR THE INTERUPT FLAG! ALWAYS! */
    _INT3IF= 0;

    LAT_ACT= LED_ON; /* Let us know you're doing things! */

    checkBus();

    LAT_ACT= LED_OFF; /* Let the user know we're done doing stuff */
}

/* The main function sets everything up then loops */
int main()
{
    byte heartBeat= 0;
    unsigned int i, temp;
    unsigned long timeout, err_reset;
    byte activeSpeed[6];

    /* Set up the Oscillator */
    initOSC();

    for(i= 0;i < 6;i++)
        motorSpeed[i]= 0x80;

    ping_val= PING_NORMAL;

    /* Set up the ADCs*/
    initADC();

    /* The value of the equation given by the formula on the reference sheet is
     * 21.75 for a 10MHz clock (so a 2.5MHz FCY) running on a 100kHz i2c port.
     * Thus we set the Baud Rate Generator to 0x16 (22 in decimal) */
    initI2C(0x16);

    /* Initialize the UART module */
    /* We set the baud to 9600 */
    initUART(0x0F);

    /* Initialize the servo junk */
    InitServos();
    
    /* Initialize timeout timer */
    // Timer runs continuously
	OpenTimer1( T1_ON & 
            T1_IDLE_CON &
            T1_GATE_OFF &
            T1_PS_1_1 &
            T1_SOURCE_INT,
            BORK_TIMEOUT_PERIOD);


    /* Set up the bus stuff for its initial stuff */
    TRIS_REQ= TRIS_RW= TRIS_IN;

    /* Turn off all the servos intially and set up PORTD as it should be */
    LATD= 0x0000;
    TRISD= 0xFF00;

    /* Set up all the extra servo enable and disable stuff */
    LAT_SERVO_WAH_INH= WAH_OFF;
    TRIS_SERVO_WAH_INH= TRIS_OUT;

    LAT_SERVO_01_EN= SERVO_DISABLED;
    LAT_SERVO_23_EN= SERVO_DISABLED;
    LAT_SERVO_45_EN= SERVO_DISABLED;
    LAT_SERVO_67_EN= SERVO_DISABLED;

    TRIS_SERVO_01_EN= TRIS_OUT;
    TRIS_SERVO_23_EN= TRIS_OUT;
    TRIS_SERVO_45_EN= TRIS_OUT;
    TRIS_SERVO_67_EN= TRIS_OUT;

    /* Turn on the Bus interrupt */
    _INT3IF= 0;
    REQ_INT_BIT= 1;

    /* Set up the LEDs */
    TRIS_ERR= TRIS_STA= TRIS_ACT= TRIS_OUT;
    LAT_ERR= LAT_ACT= LED_OFF;
    LAT_STA= LED_ON;

    /* Wait for everything to settle down. */
    LAT_ACT= LED_ON;
    for(timeout= 0;timeout < (START_TIMEOUT >> 2);timeout++)
        ;
    LAT_ACT= LED_OFF;
    for(timeout= 0;timeout < (START_TIMEOUT >> 2);timeout++)
        ;
    LAT_ACT= LED_ON;
    for(timeout= 0;timeout < (START_TIMEOUT >> 2);timeout++)
        ;
    LAT_ACT= LED_OFF;
    for(timeout= 0;timeout < (START_TIMEOUT >> 2);timeout++)
        ;

    _TRISF6= TRIS_OUT;
    _TRISF7= TRIS_OUT;
    _TRISF8= TRIS_OUT;

    while(1) {
        /* Avoid a race condition!!! */
        REQ_INT_BIT= 0;
        for(i= 0;i < 6;i++) {
            activeSpeed[i]= motorSpeed[i];
            /* Debug */
            //writeUart(activeSpeed[i]);
        }
        REQ_INT_BIT= 1;

        ping_val= PING_NORMAL;

        for(i= 0;i < 6;i++) {
            /*temp= LATF & 0xFE3F;
            LATF= temp | (i << 6);*/
            /*temp= ~i;
            _LATF6= !(temp & 0x01);
            _LATF7= !(temp & 0x02);
            _LATF8= !(temp & 0x04);*/
            _LATF6= i & 0x01;
            _LATF7= (i & 0x02) >> 1;
            _LATF8= (i & 0x04) >> 2;

            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();

            packetSize= 4;
            temp= i2cBuf[0]= 0x52;
            temp+= (i2cBuf[1]= activeSpeed[i]);
            temp+= (i2cBuf[2]= 0x64);
            i2cBuf[3]= temp & 0xFF;

            StartI2C();

            // Start timer
            TMR1 = 0;
            _T1IF = 0;

            while(i2cState != I2CSTATE_IDLE && i2cState != I2CSTATE_BORKED && !_T1IF)
                writeUart('E');

            if(_T1IF) {
                BorkedI2C();
                while(i2cState != I2CSTATE_BORKED)
                    writeUart('F');
            }

            if(i2cState == I2CSTATE_BORKED) {
                /* We should probably do something with this info! */
                LAT_ERR= LED_ON;
                err_reset= SHOW_ERR_TIMEOUT;
                resetI2C_registers();
                i2cState= I2CSTATE_IDLE;
            }

            if(LAT_ERR == LED_ON)
                if(err_reset-- == 0)
                    LAT_ERR= LED_OFF;

            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();
            Nop();Nop();Nop();Nop();Nop();

            writeUart(heartBeat++);
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

    /* Set up the UART settings: 8 bits, 1 stop bit, no parity, standard IO */
    U1MODE= 0x0000;

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
    I2CCONbits.SEN = 1;        //Generate Start Condition
    i2cState= I2CSTATE_START;
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

/* This function generates the stop condition */
unsigned int StopI2C(void)
{
    I2CCONbits.PEN = 1;        /* Generate the Stop condition */
    return 0;
}

/* This function transmits the byte passed to it over the i2c bus */
unsigned int WriteI2C(byte b)
{
    /* So make sure there's space in the transmit buffer before we stick
     * anything in there */
    /* So for now, we'll pretend like there's never anything in the buffer
     * because there shouldn't be, and besides that, we really don't care. */
/*    if(I2CSTATbits.TBF) {
        return 0xFF;
    }*/

    /* Jam the byte in the transmit buffer! */
    I2CTRN = b;
    return 0;
}

/* Returns 1 if the bus is idle, 0 otherwise */
unsigned int IdleI2C(void)
{
    return !(I2CCONbits.SEN || I2CCONbits.PEN || I2CCONbits.RCEN || I2CCONbits.ACKEN || I2CSTATbits.TRSTAT);
}

/* This byte returns whether the previous byte was ACK'd */
/* returns 0 if the previous sent byte was NACK'd, non-0 otherwise */
unsigned int wasAckI2C(void) {
    return (I2CSTATbits.ACKSTAT == I2C_ACK);
}

/* This should stop any of the dumb errors we're getting. */
void resetI2C_registers(void) {
    I2CSTAT= 0x0000; /* Wipe out I2CSTAT */
}
