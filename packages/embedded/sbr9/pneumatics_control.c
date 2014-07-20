#include <p30fxxxx.h>
#include "buscodes.h"

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

/* Relay control definitions */
enum PairCommand {
    PCMD_FILL,
    PCMD_VOID,
    PCMD_OFF
};

struct CmdEnt {
    int count;
    char cmd;
    char pair;
};

#define CMDCOUNT 48
#define VALIDBITS (CMDCOUNT / 16)

struct CmdEnt cmds[CMDCOUNT];
unsigned int cmdValid[VALIDBITS];

/* Pin defines! */

/* {{{ The relay defines should exactly match the ones in the schematic */
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
/* }}} */

/* {{{ Logical names - We have them in void-fill pairs */
#define FILL_PAIR_0  RELAY_13
#define FILL_PAIR_1  RELAY_14
#define FILL_PAIR_2  RELAY_15
#define FILL_PAIR_3  RELAY_16
#define FILL_PAIR_4  RELAY_17
#define FILL_PAIR_5  RELAY_18
#define FILL_PAIR_6  RELAY_19
#define FILL_PAIR_7  RELAY_20
#define FILL_PAIR_8  RELAY_21
#define FILL_PAIR_9  RELAY_22
#define FILL_PAIR_10 RELAY_23
#define FILL_PAIR_11 RELAY_24

#define VOID_PAIR_0  RELAY_1
#define VOID_PAIR_1  RELAY_2
#define VOID_PAIR_2  RELAY_3
#define VOID_PAIR_3  RELAY_4
#define VOID_PAIR_4  RELAY_5
#define VOID_PAIR_5  RELAY_6
#define VOID_PAIR_6  RELAY_7
#define VOID_PAIR_7  RELAY_8
#define VOID_PAIR_8  RELAY_9
#define VOID_PAIR_9  RELAY_10
#define VOID_PAIR_10 RELAY_11
#define VOID_PAIR_11 RELAY_12

/* Maps pistons as extend and retract pairs */
#define PIST_0_EXT 2
#define PIST_0_RET 0

#define PIST_1_EXT 3
#define PIST_1_RET 1

#define PIST_2_EXT 4
#define PIST_2_RET 6

#define PIST_3_EXT 5
#define PIST_3_RET 7

#define PIST_4_EXT 8
#define PIST_4_RET 10

#define PIST_5_EXT 9
#define PIST_5_RET 11

/* Maps specific types of pistons to piston numbers */
#define TORP_1_PIST 4
#define TORP_1_RET  PIST_4_RET
#define TORP_1_EXT  PIST_4_EXT

#define TORP_2_PIST 5
#define TORP_2_RET  PIST_5_RET
#define TORP_2_EXT  PIST_5_EXT

#define GRABBER_1_PIST 0
#define GRABBER_1_RET  PIST_0_RET
#define GRABBER_1_EXT  PIST_0_EXT

#define GRABBER_2_PIST 1
#define GRABBER_2_RET  PIST_1_RET
#define GRABBER_2_EXT  PIST_1_EXT

#define MARKER_1_PIST 2
#define MARKER_1_RET  PIST_2_RET
#define MARKER_1_EXT  PIST_2_EXT

#define MARKER_2_PIST 3
#define MARKER_2_RET  PIST_3_RET
#define MARKER_2_EXT  PIST_3_EXT
/* }}} */

/* {{{ Function Prototypes */
/**********************************************/
void initUART(byte);
void initADC(void);

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

void handleCmd(enum PairCommand cmd, char pair);
void addCmd(enum PairCommand cmd, char pair, int count);

void extendPiston(unsigned char piston);
void retractPiston(unsigned char piston);

/* timer stuff */
void wait10ms(void);
void wait100ms(void);
void wait1s(void);
/* }}} */

/*****************************************************************************/
/*****************************************************************************/
/* This is the end of the prototypes, defines, and other such setup nonsense */
/*****************************************************************************/
/*****************************************************************************/

/* {{{ _T1Interrupt - runs every 20mS, looking for commands to run
 */
void _ISR _T1Interrupt()
{
    int i, j, curf;

    _T1IF= 0;

    // Find and decrement all valid commands
    for(i= 0;i < VALIDBITS;++i) {
        if(cmdValid[i] == 0)
            continue;

        j= i * 16; curf= 0x0001;
        while(curf != 0) {
            if(cmdValid[i] & curf) {
                --cmds[j].count;
                if(cmds[j].count <= 0) {
                    handleCmd(cmds[j].cmd, cmds[j].pair);
                    cmdValid[i]= cmdValid[i] & (~curf);
                }
            }
            ++j; curf= (curf << 1);
        }
    }
} /* }}} */

/* {{{ The main function */
int main(void)
{
    unsigned char buff[10];
    unsigned int i;
    static int marker1_ext= 1, marker2_ext= 1;

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

    // No valid commands
    for(i= 0;i < VALIDBITS;++i)
        cmdValid[i]= 0;

    // Initialize timer1
    PR1= 36850;
    T1CON= 0x8000;

    // Enable the interrupt
    _T1IE= 1;

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
                addCmd(PCMD_VOID, buff[2], 1);
            }
        } else if(buff[0] == 'F' && buff[1] == 'L') {
            if(buff[2] > 11) {
                writeUart('N');
                writeUart('P');
                writeUart('A');
                writeUart('R');
            } else {
                addCmd(PCMD_FILL, buff[2], 1);
            }
        } else if(buff[0] == 'E' && buff[1] == 'X') {
            if(buff[2] > 5) {
                writeUart('N');
                writeUart('P');
                writeUart('A');
                writeUart('R');
            } else {
                extendPiston(buff[2]);
            }
        } else if(buff[0] == 'R' && buff[1] == 'T') {
            if(buff[2] > 5) {
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
                addCmd(PCMD_OFF, buff[2], 1);
            }
        } else if(buff[0] == 'O' && buff[1] == 'F' && buff[2] == 'F') {
            offAll();
        } else if(buff[0] == 'B' && buff[1] == 'C') {
            if(buff[2] == BUS_CMD_FIRE_TORP_1) {
                retractPiston(TORP_1_PIST);
            } else if(buff[2] == BUS_CMD_FIRE_TORP_2) {
                retractPiston(TORP_2_PIST);
            } else if(buff[2] == BUS_CMD_VOID_TORP_1) {
                addCmd(PCMD_VOID, TORP_1_EXT, 1);
                addCmd(PCMD_VOID, TORP_1_RET, 1);
            } else if(buff[2] == BUS_CMD_VOID_TORP_2) {
                addCmd(PCMD_VOID, TORP_2_EXT, 1);
                addCmd(PCMD_VOID, TORP_2_RET, 1);
            } else if(buff[2] == BUS_CMD_ARM_TORP_1) {
                extendPiston(TORP_1_PIST);
            } else if(buff[2] == BUS_CMD_ARM_TORP_2) {
                extendPiston(TORP_2_PIST);
            } else if(buff[2] == BUS_CMD_EXT_GRABBER) {
                extendPiston(GRABBER_1_PIST);
                extendPiston(GRABBER_2_PIST);
            } else if(buff[2] == BUS_CMD_EXT_GRABBER_1) {
                extendPiston(GRABBER_1_PIST);
            } else if(buff[2] == BUS_CMD_EXT_GRABBER_2) {
                extendPiston(GRABBER_2_PIST);
            } else if(buff[2] == BUS_CMD_RET_GRABBER) {
                retractPiston(GRABBER_1_PIST);
                retractPiston(GRABBER_2_PIST);
            } else if(buff[2] == BUS_CMD_VOID_GRABBER) {
                addCmd(PCMD_VOID, GRABBER_1_EXT, 1);
                addCmd(PCMD_VOID, GRABBER_1_RET, 1);
                addCmd(PCMD_VOID, GRABBER_2_EXT, 1);
                addCmd(PCMD_VOID, GRABBER_2_RET, 1);
            } else if(buff[2] == BUS_CMD_MARKER1) {
                if(marker1_ext) {
                    retractPiston(MARKER_1_PIST);
                } else {
                    extendPiston(MARKER_1_PIST);
                }

                marker1_ext= !marker1_ext;
            } else if(buff[2] == BUS_CMD_MARKER2) {
                if(marker2_ext) {
                    retractPiston(MARKER_2_PIST);
                } else {
                    extendPiston(MARKER_2_PIST);
                }

                marker2_ext= !marker2_ext;
            } else if(buff[2] == BUS_CMD_VOID_PNEU) {
                voidAll();
            } else if(buff[2] == BUS_CMD_OFF_PNEU) {
                offAll();
            } else {
                writeUart('N');
                writeUart('P');
                writeUart('A');
                writeUart('R');
            }
        } else {
            writeUart('N');
            writeUart('C');
            writeUart('M');
            writeUart('D');
        }
    }

    return 0;
} /* }}} */

/* {{{ voidPair - takes in a pair number and voids that pair */
void voidPair(unsigned char pair)
{
    if(pair == 0) {
        if(FILL_PAIR_0 == RELAY_ON) {
            FILL_PAIR_0= RELAY_OFF;
        }
        VOID_PAIR_0= RELAY_ON;
    } else if(pair == 1) {
        if(FILL_PAIR_1 == RELAY_ON) {
            FILL_PAIR_1= RELAY_OFF;
        }
        VOID_PAIR_1= RELAY_ON;
    } else if(pair == 2) {
        if(FILL_PAIR_2 == RELAY_ON) {
            FILL_PAIR_2= RELAY_OFF;
        }
        VOID_PAIR_2= RELAY_ON;
    } else if(pair == 3) {
        if(FILL_PAIR_3 == RELAY_ON) {
            FILL_PAIR_3= RELAY_OFF;
        }
        VOID_PAIR_3= RELAY_ON;
    } else if(pair == 4) {
        if(FILL_PAIR_4 == RELAY_ON) {
            FILL_PAIR_4= RELAY_OFF;
        }
        VOID_PAIR_4= RELAY_ON;
    } else if(pair == 5) {
        if(FILL_PAIR_5 == RELAY_ON) {
            FILL_PAIR_5= RELAY_OFF;
        }
        VOID_PAIR_5= RELAY_ON;
    } else if(pair == 6) {
        if(FILL_PAIR_6 == RELAY_ON) {
            FILL_PAIR_6= RELAY_OFF;
        }
        VOID_PAIR_6= RELAY_ON;
    } else if(pair == 7) {
        if(FILL_PAIR_7 == RELAY_ON) {
            FILL_PAIR_7= RELAY_OFF;
        }
        VOID_PAIR_7= RELAY_ON;
    } else if(pair == 8) {
        if(FILL_PAIR_8 == RELAY_ON) {
            FILL_PAIR_8= RELAY_OFF;
        }
        VOID_PAIR_8= RELAY_ON;
    } else if(pair == 9) {
        if(FILL_PAIR_9 == RELAY_ON) {
            FILL_PAIR_9= RELAY_OFF;
        }
        VOID_PAIR_9= RELAY_ON;
    } else if(pair == 10) {
        if(FILL_PAIR_10 == RELAY_ON) {
            FILL_PAIR_10= RELAY_OFF;
        }
        VOID_PAIR_10= RELAY_ON;
    } else if(pair == 11) {
        if(FILL_PAIR_11 == RELAY_ON) {
            FILL_PAIR_11= RELAY_OFF;
        }
        VOID_PAIR_11= RELAY_ON;
    }

    /* If they gave an invalid pair do nothing */
} /* }}} */

/* {{{ fillPair - takes in a pair number and fills that pair */
void fillPair(unsigned char pair)
{
    if(pair == 0) {
        if(VOID_PAIR_0 == RELAY_ON) {
            VOID_PAIR_0= RELAY_OFF;
        }
        FILL_PAIR_0= RELAY_ON;
    } else if(pair == 1) {
        if(VOID_PAIR_1 == RELAY_ON) {
            VOID_PAIR_1= RELAY_OFF;
        }
        FILL_PAIR_1= RELAY_ON;
    } else if(pair == 2) {
        if(VOID_PAIR_2 == RELAY_ON) {
            VOID_PAIR_2= RELAY_OFF;
        }
        FILL_PAIR_2= RELAY_ON;
    } else if(pair == 3) {
        if(VOID_PAIR_3 == RELAY_ON) {
            VOID_PAIR_3= RELAY_OFF;
        }
        FILL_PAIR_3= RELAY_ON;
    } else if(pair == 4) {
        if(VOID_PAIR_4 == RELAY_ON) {
            VOID_PAIR_4= RELAY_OFF;
        }
        FILL_PAIR_4= RELAY_ON;
    } else if(pair == 5) {
        if(VOID_PAIR_5 == RELAY_ON) {
            VOID_PAIR_5= RELAY_OFF;
        }
        FILL_PAIR_5= RELAY_ON;
    } else if(pair == 6) {
        if(VOID_PAIR_6 == RELAY_ON) {
            VOID_PAIR_6= RELAY_OFF;
        }
        FILL_PAIR_6= RELAY_ON;
    } else if(pair == 7) {
        if(VOID_PAIR_7 == RELAY_ON) {
            VOID_PAIR_7= RELAY_OFF;
        }
        FILL_PAIR_7= RELAY_ON;
    } else if(pair == 8) {
        if(VOID_PAIR_8 == RELAY_ON) {
            VOID_PAIR_8= RELAY_OFF;
        }
        FILL_PAIR_8= RELAY_ON;
    } else if(pair == 9) {
        if(VOID_PAIR_9 == RELAY_ON) {
            VOID_PAIR_9= RELAY_OFF;
        }
        FILL_PAIR_9= RELAY_ON;
    } else if(pair == 10) {
        if(VOID_PAIR_10 == RELAY_ON) {
            VOID_PAIR_10= RELAY_OFF;
        }
        FILL_PAIR_10= RELAY_ON;
    } else if(pair == 11) {
        if(VOID_PAIR_11 == RELAY_ON) {
            VOID_PAIR_11= RELAY_OFF;
        }
        FILL_PAIR_11= RELAY_ON;
    }

    /* If they gave an invalid pair do nothing */
} /* }}} */

/* {{{ turnOff - takes in a pair and turns both relays off */
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
} /* }}} */

/* {{{ extendPiston - takes in a piston number and sets up the commands to extend it
 */
void extendPiston(unsigned char piston)
{
    char p_ext= -1, p_ret= -1;
    if(piston == 0) {
        p_ext= PIST_0_EXT;
        p_ret= PIST_0_RET;
    } else if(piston == 1) {
        p_ext= PIST_1_EXT;
        p_ret= PIST_1_RET;
    } else if(piston == 2) {
        p_ext= PIST_2_EXT;
        p_ret= PIST_2_RET;
    } else if(piston == 3) {
        p_ext= PIST_3_EXT;
        p_ret= PIST_3_RET;
    } else if(piston == 4) {
        p_ext= PIST_4_EXT;
        p_ret= PIST_4_RET;
    } else if(piston == 5) {
        p_ext= PIST_5_EXT;
        p_ret= PIST_5_RET;
    }

    // Do nothing if we got something invalid
    if(p_ext == -1 || p_ret == -1)
        return;

    addCmd(PCMD_VOID, p_ret, 1);
    addCmd(PCMD_FILL, p_ext, 2);
    addCmd(PCMD_OFF, p_ext, 500);
    addCmd(PCMD_FILL, p_ret, 501);
    addCmd(PCMD_OFF, p_ret, 551);
} /* }}} */

/* {{{ retractPiston - take in a piston number and set up all the commands to retract it
 */
void retractPiston(unsigned char piston)
{
    char p_ext= -1, p_ret= -1;
    if(piston == 0) {
        p_ext= PIST_0_EXT;
        p_ret= PIST_0_RET;
    } else if(piston == 1) {
        p_ext= PIST_1_EXT;
        p_ret= PIST_1_RET;
    } else if(piston == 2) {
        p_ext= PIST_2_EXT;
        p_ret= PIST_2_RET;
    } else if(piston == 3) {
        p_ext= PIST_3_EXT;
        p_ret= PIST_3_RET;
    } else if(piston == 4) {
        p_ext= PIST_4_EXT;
        p_ret= PIST_4_RET;
    } else if(piston == 5) {
        p_ext= PIST_5_EXT;
        p_ret= PIST_5_RET;
    }

    // Do nothing if we got something invalid
    if(p_ext == -1 || p_ret == -1)
        return;

    addCmd(PCMD_VOID, p_ext, 1);
    addCmd(PCMD_FILL, p_ret, 2);
    addCmd(PCMD_OFF, p_ret, 500);
    addCmd(PCMD_FILL, p_ext, 501);
    addCmd(PCMD_OFF, p_ext, 551);
} /* }}} */

/* {{{ voidAll - immediately void all pairs and empty the command list
 */
void voidAll(void)
{
    int i;

    // Clear out the valid bitfields (removes all commands)
    for(i= 0;i < VALIDBITS;i++) {
        cmdValid[i]= 0;
    }

    // Void all pistons
    for(i= 0;i < 12;i++) {
        voidPair(i);
    }
} /* }}} */

/* {{{ offAll - immediately turn off all pairs and empty the command list
 */
void offAll(void)
{
    int i;

    // Clear out the valid bitfields (removes all commands)
    for(i= 0;i < VALIDBITS;i++) {
        cmdValid[i]= 0;
    }

    for(i= 0;i < 12;i++) {
        turnOff(i);
    }
} /* }}} */

/* {{{ handleCmd - takes in a command and a pair and runs it
 */
void handleCmd(enum PairCommand cmd, char pair)
{
    if(cmd == PCMD_FILL) {
        fillPair(pair);
    } else if(cmd == PCMD_VOID) {
        voidPair(pair);
    } else if(cmd == PCMD_OFF) {
        turnOff(pair);
    }
} /* }}} */

/* {{{ addCmd - safely add a command to the command list
   in  - the command, the pair, and the count
   out - nothing
 */
void addCmd(enum PairCommand cmd, char pair, int count)
{
    unsigned int curf, i, j;
    int firstInv= -1;

    // Scan through the command field and set any commands
    // acting on the same pair but later as invalid
    for(i= 0;i < VALIDBITS;i++) {
        // Ignore any empty sections unless we still need an invalid spot
        if(cmdValid[i] == 0 && firstInv != -1)
            continue;

        j= 16 * i;
        curf= 0x0001;

        // Unrolling this might help with speed issues
        while(curf && (firstInv == -1 || (cmdValid[i] & ~(curf - 1)) != 0)) {
            // Spot any valid commands acting on the same pair and remove them
            if((cmdValid[i] & curf) != 0 && cmds[j].pair == pair
                                         && cmds[j].count >= count)
                cmdValid[i]&= ~curf;

            // Catch the first invalid spot
            if(firstInv == -1 && !(cmdValid[i] & curf))
                firstInv= j;

            // fix the bitmask, increment the command array index
            curf= (curf << 1);
            j++;
        }
    }

    // Now the command list should be ready (clear of dangerous commands)

    // If we have no space just stop
    if(firstInv == -1) {
        writeUart('N');
        writeUart('S');
        writeUart('P');
        writeUart('C');
        return;
    }

    // put the command in the first spot
    cmds[firstInv].count= count;
    cmds[firstInv].pair= pair;
    cmds[firstInv].cmd= cmd;

    i= (firstInv >> 4);
    j= (firstInv & 0x0f);

    // valid bit MUST BE SET LAST
    cmdValid[i]|= (0x0001 << j);
}
/* }}} */

/* {{{ initUART - This function initializes the UART with the given baud */
void initUART(byte baud_rate)
{
    /* Disable the UART before we mess with it */
    U1MODEbits.UARTEN= 0;

    _TRISC14= TRIS_IN;
    _TRISC14= TRIS_IN;

    /* Set the baud rate */
    U1BRG= 0x0000 | baud_rate;

    /* Set up the UART settings: 8 bits, 1 stop bit, no parity */
    U1MODE= 0x0000;

    /* Everything that we need is set up, so go ahead and activate the UART */
    U1MODEbits.UARTEN= 1;
    U1MODEbits.ALTIO= 1;

    /* Enable Transmission. This MUST BE DONE **AFTER** enabling the UART */
    U1STAbits.UTXEN= 1;
} /* }}} */

/* {{{ initADC - This turns off the ADC and sets all pins to digital I/O */
void initADC() {
    /* In case it isn't already off, kill the ADC module */
    ADCON1bits.ADON= 0;
    
    /* Disable the ADCs for now. This sets all ADC pins as
     * digital pins. */
    ADPCFG= 0xFFFF;
} /* }}} */

/* {{{ uartRXwait - waits for a byte to hit the receive buffer, then returns*/
void uartRXwait() {
    /* Loop waiting for there to be a byte */
    while(!U1STAbits.URXDA)
        ;
} /* }}} */

/* {{{ uartRXCheck - returns 0 if there is nothing in the RX buffer */
unsigned int uartRXCheck() {
    return U1STAbits.URXDA;
} /* }}} */

/* {{{ uartRX - grabs a byte off the recieve buffer and returns it*/
byte uartRX() {
    return U1RXREG;
} /* }}} */

/* {{{ writeUart - *safely* writes a packet to the Uart1 output */
void writeUart(byte packet)
{
    /* Wait for space to be available */
    while(U1STAbits.UTXBF)
        ;

    /* Send the packet! */
    U1TXREG= packet;
} /* }}} */

/* {{{ wait10ms - sets up timer 2 and waits 10ms, then returns
 */
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
} /* }}} */

/* {{{ wait100ms - sets up timer 2 and waits 100ms, then returns
 */
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
} /* }}} */

/* {{{ wait1s - sets up timer 2 and waits 1s, then returns
 */
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
} /* }}} */

