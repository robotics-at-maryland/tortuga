#include <p30fxxxx.h>
#include <stdio.h>
#include "scottcommon.h"

int uart_enabled;

//////////////////////////////////////////////
//Use this function to send 1 byte to the UART
void sendByte(byte i){
	if(!uart_enabled)return;
	while(U1STAbits.UTXBF);
    U1TXREG = i;
    while(U1STAbits.UTXBF);
}
//////////////////////////////////////////////
//Use this function to send literal strings in quotes as
//ASCII bytes to the UART
void sendString( char str[]){
	if(!uart_enabled)return;
    byte i=0;
    for(i=0; str[i]!=0; i++){
        sendByte(str[i]);
	}
}
//////////////////////////////////////////////
//Use this function to send an unsigned integer to the UART as ASCII text
void sendNum(unsigned int i){
	if(!uart_enabled)return;
	char tmp[10];
	sprintf(tmp, "%u ", i);
	sendString(tmp);
}

//////////////////////////////////////////////
/* UART BRG "Baud" rate calculation "How To"
Baud Rate is the same as Bits Per Second
	U1BRG = (MIPS / BAUD / 16) - 1
EXAMPLE...
15MHz Oscillator with pll of 8
MIPS = 7.5Mhz * pll16 / 4 = 30,000,000 MIPS
30000000/BAUD(230400) = 130.208...
130.208... / 16 - 1 = 7.138... ~~~ 7
****don't forget to invert the signal coming out of the PIC
to create a signal that most computer's serial ports will 
interpret.  When communicating to other PIC devices, no inversion is needed. */
extern void init_Uart(byte enabled){
	uart_enabled=enabled;
	if(!uart_enabled)return;
    U1MODE = 0x0000;
    // Calculations for differnt Baud rates... (assumes 30MIPS operation)
    // 25 for baud of 38400 
    // 7  for baud of 230400 pll8
    // 15 for baud of 115200
    U1BRG = 15;
    U1MODEbits.ALTIO = 1;   // Use alternate IO
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;   // Enable transmit
}

