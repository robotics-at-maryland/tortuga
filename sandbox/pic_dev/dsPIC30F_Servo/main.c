#include "p30fxxxx.h"
#define byte unsigned char
#define UART_ENABLED 1  //set to 0 to turn off uart

//Configuration settings for the dsPIC30F4012
_FOSC(CSW_FSCM_OFF & XT_PLL8); //to get 30MIPS with 15MHz clock
//_FOSC(CSW_FSCM_OFF & XT_PLL16); //to get 30MIPS with 7.37MHz clock
_FOSCSEL(PRIOSC_PLL);
_FWDT(WDT_OFF);	
_FGS(CODE_PROT_OFF);

void initUart(void);
void intiIO(void);
void initTimer(void);
void sendString(unsigned char * s);
void sendNum(unsigned int i);
void sendByte(byte i);
void delay(int microseconds);

int main(void){
	int dutyCycle = 710;
	
	delay(200);  
	/* nothing should run in the first 200 ms so that 
	 * if the chip resets the output is not interupted */

    initUart();
	sendString("\n\rRunning...\n\r\n\r");
	sendString("Starting at duty cycle = ");
	sendNum(dutyCycle);
	sendString("\n\r");
	initIO();

	initTimer2and3();
	OC2CONbits.OCM = 0b000; //keep OC2 off while mod's are made
	OC2CONbits.OCSIDL = 0; //0=continued operation in idle mode
	OC2CONbits.OCTSEL = 1; //1=timer3  0=timer2
	OC2R=0;
	OC2RS=dutyCycle;
	OC2CONbits.OCM = 0b101; //initialize OC2 low, generate continuous output pulses
	
	/*  OUTPUT COMPARE CONFIGURATION CALCULATIONS
	instruction clock cycle time.... 30MIPS... 33ns
	using timer prescaler of 1:64 so 2.112us per TMR3 incriment
	2.112us * 2^16 = 138.4ms maximum period
	1ms = 473.5 timer ticks
	1.5ms = 710.3 timer ticks
	2ms = 947 timer ticks
	25ms = 11,837.1 timer ticks
	*/
	
	while(1){
		if(PORTBbits.RB0==1 && dutyCycle<1065){
			sendString("decreased duty cycle to ");
			dutyCycle+=5;
			OC2RS=dutyCycle;
			sendNum(dutyCycle);
			sendString("\n\r");
			//delay(10);
		}
		if(PORTBbits.RB1==1 && dutyCycle>110){
			sendString("increased duty cycle to ");
			dutyCycle-=5;
			OC2RS=dutyCycle;
			sendNum(dutyCycle);
			sendString("\n\r");
			//delay(10);
		}
		
	}
}

void initTimer2and3(void){
	//setup TMR2 and TMR3... in this project, TMR3 is used to do continuous PWM on 
	T2CONbits.T32=0; //turn off 32 bit operation
	T2CONbits.TCKPS=0b11;  // 1:256 prescale value
	T2CONbits.TON=1;
	
	T3CONbits.TCKPS=0b10; // 1:8 prescale value
	T3CONbits.TSIDL=0; //continue in idle mode
	T3CONbits.TCS=0; //internal clock  (F_OSC/4)
	T3CONbits.TON=1;
	PR3=11837; //25ms period for 40Hz operation
}

void delay(int mseconds){ //cannot be called with more than about 550ms
	const int onemillisecond = 0b000000001110101;
	//const int onemillisecond = 1;
	mseconds=mseconds*onemillisecond;
	//start config bit selection
	T1CONbits.TON=0;
	T1CONbits.TCKPS=0b11;
	TMR1=0;
	T1CONbits.TON=1;
	while(TMR1<mseconds){
		Nop();
	}
	T1CONbits.TON=0;
}

//////////////////////////////////////////////
//Use this function to send 1 byte to the UART
void sendByte(byte i){
	if(!UART_ENABLED)return;
	while(U1STAbits.UTXBF);
    U1TXREG = i;
    while(U1STAbits.UTXBF);
}
//////////////////////////////////////////////
//Use this function to send literal strings in quotes as
//ASCII bytes to the UART
void sendString(unsigned char str[]){
	if(!UART_ENABLED)return;
    byte i=0;
    for(i=0; str[i]!=0; i++){
        sendByte(str[i]);
	}
}
//////////////////////////////////////////////
//Use this function to send an unsigned integer to the UART
//as ASCII text
void sendNum(unsigned int i){
	if(!UART_ENABLED)return;
	unsigned char tmp[10];
	sprintf(tmp, "%u ", i);
	sendString(tmp);
}

//////////////////////////////////////////////
/* UART BRG "Baud" rate calculation "How To"
Baud Rate is the same as Bits Per Second
	U1BRG = (MIPS / BAUD / 16) - 1
EXAMPLE...
15MHz Oscillator with pll of 8
MIPS = 15MMhz * pll8 / 4 = 30,000,000
30000000/BAUD(230400) = 130.208...
130.208... / 16 - 1 = 7.138... ~~~ 7
****don't forget to invert the signal coming out of the PIC
to create a signal that most computer's serial ports will 
interpret  */
void initUart(void){
	if(!UART_ENABLED)return;
    U1MODE = 0x0000;
    U1BRG = 7;  // 25 for baud of 38400 //7 for baud of 230400 pll8
    U1MODEbits.ALTIO = 1;   // Use alternate IO
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;   // Enable transmit
}

void initIO(void){
	//setup master timing inputs and outputs
	ADPCFG=0b1111111111111111;//turn off all analog stuff
	TRISBbits.TRISB0=1; //Hydrophone input 1
	TRISBbits.TRISB1=1;	//Hydrophone input 2
	TRISBbits.TRISB2=1;	//Hydrophone input 3
	TRISBbits.TRISB3=1; //FFT validation input
}
