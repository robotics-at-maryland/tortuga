#include "p30fxxxx.h"
#define byte unsigned char
#define TIMEOUT 50000 //whatever value equates to 500us or thereabouts
#define UART_ENABLED 1  //set to 0 to turn off uart and get steve's bus working

//For the dsPIC30F4012
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

byte pingCounter=0;

int main (void){
	const int hydrophoneMask = 7; //...00000111
	const int hydrophoneMask1 = 1; //...000001
	const int hydrophoneMask2 = 2; //...000010
	const int hydrophoneMask3 = 4; //...000100
	byte status, status1, status2, status3;
	byte finishedStatus = 0;
	int h1time, h2time, h3time;
	int diff1and2, diff2and3;

	initBus();
    initUart();
	sendString("\n\rRunning...\n\n\r");

	initIO();
	initTimer();

	/*it takes 34.67us to flip pin RE3
	TRISEbits.TRISE3=0;
	while(1){
		if(TMR2>1){
			//toggle pin
			TMR2=0;
			PORTEbits.RE3=PORTEbits.RE3^1;
		}
	}*/

	diff1and2=3333;//initialize these to make sure junk doesn't go here in the end
	diff2and3=2222;
	makeAvailableToBuffer(diff1and2,diff2and3,pingCounter);//just so that I have some data to be read even if there are no pings

	//start watching inputs
	while(1){
		if(finishedStatus==0) checkBus(); //only take time to check bus if ping timing is not in progress
		status = PORTB&hydrophoneMask;
		status1 = status&hydrophoneMask1;
		status2 = status&hydrophoneMask2;
		status3 = status&hydrophoneMask3;
		
		if(UART_ENABLED==1){
			sendString("status ");sendNum(status);sendString("\n\r");
			sendString("stat1:");sendNum(status1);
			sendString(", stat2:");sendNum(status2);
			sendString(", stat3:");sendNum(status3);sendString("\n\r");
			sendString("finishedStatus:");sendNum(finishedStatus);sendString("\n\r");
			sendString("TMR2:");sendNum(TMR2);
			sendString("   ping:");sendNum(pingCounter);sendString("\n\r\n\r");
			delay(500);
			delay(500);
		}

		if(status!=0){
			//////////////////////////////////
			//TURN OFF INTERUPTS HERE!!!  ////
			//////////////////////////////////
			if(finishedStatus==0){
				TMR2=0;//reset timer
			}
			sendString("bad1:");sendNum(finishedStatus&0b00000001);
			sendString(", bad2:");sendNum(finishedStatus&0b00000010);
			sendString(", bad3:");sendNum(finishedStatus&0b00000100);sendString("\n\r");
			if((status1==1) && ((finishedStatus&0b00000001)!=1)){ //hydrophone 1 signal came in
				h1time=TMR2;
				finishedStatus++;
				sendString(" _ 1 _\n\r");
			}
			if((status2==2) && ((finishedStatus&0b00000010)!=2)){ //hydrophone 2 signal came in
				h2time=TMR2;
				finishedStatus+=2;
				sendString(" _ 2 _\n\r");
			}
			if((status3==4) && ((finishedStatus&0b00000100)!=4)){ //hydrophone 3 signal came in
				h3time=TMR2;
				finishedStatus+=4;
				sendString(" _ 3 _\n\r");
			}
		}
		if(0 && finishedStatus!=0 && TMR2>TIMEOUT){
			//if a hydrophone triggered the timer but more than 440us have
			//passed then I need to reset everything because it was a false
			//positive or a missed ping
			finishedStatus=0;
			TMR2=0;
			pingCounter++;
		}
		if(finishedStatus==7){
			diff1and2=h1time-h2time;
			diff2and3=h2time-h3time;
			pingCounter++;

			sendString(" _ THE DATA _\n\r");
			sendString("___hydrophone 1 time = ");sendNum(h1time);sendString("\n\r");
			sendString("___hydrophone 2 time = ");sendNum(h2time);sendString("\n\r");
			sendString("___hydrophone 3 time = ");sendNum(h3time);sendString("\n\r");
			sendString("___diff1and2 = ");sendNum(diff1and2);sendString("\n\r");
			sendString("___diff2and3 = ");sendNum(diff2and3);sendString("\n\r\n\r");
			sendString("___ping counter = ");sendNum(pingCounter);sendString("\n\r\n\r");
			
			makeAvailableToBuffer(diff1and2,diff2and3,pingCounter);
			//////////////////////////////////
			//TURN ON INTERUPTS HERE!!!     //
			//////////////////////////////////
			//delay(1); //delay long enough to wait for next pulse
			//delay(500);
			//delay(500);
			finishedStatus=0; //reset for next round
			
		}
	}
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
		checkBus();//this way steve's bus will not time out when I'm waiting to listen to the next ping
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
	U1BRG = (MIPS / 4 / BAUD / 16) - 1
EXAMPLE...
15MHz Oscillator with pll of 8
MIPS = 15MMhz * pll8 = 120,000,000
120000000/4 = 30,000,000
30000000/BAUD(230400) = 130.208...
130.208... / 16 - 1 = 7.138... ~~~ 7
****don't forget to invert the signal coming out of the PIC
to create a signal that most computer's serial ports will 
interpret  */

void initUart(void){
	sendUARTsettingTObusCODE(UART_ENABLED);//even to let steve's code know that the uart is disabled
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

void initTimer(void){
	//setup "master timer" for master timing functionality of master timing PIC
	T2CONbits.T32=0; //turn off 32 bit operation
	T2CONbits.TCKPS=0b11;
	T2CONbits.TON=1;
}