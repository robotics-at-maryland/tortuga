#include "p30fxxxx.h"

#define byte unsigned char

//For the dsPIC30F4012
_FOSC(CSW_FSCM_OFF & XT_PLL8); //to get 30MIPS with 15MHz clock
//_FOSC(CSW_FSCM_OFF & XT_PLL16); //to get 30MIPS with 7.37MHz clock
_FOSCSEL(PRIOSC_PLL);
_FWDT(WDT_OFF);	
_FGS(CODE_PROT_OFF);

void initUart(void);
void sendString(unsigned char * s);
void sendNum(unsigned int i);
void sendByte(byte i);
void delay();

int timerCounter=0;

int main (void){
	const int hydrophoneMask = 7; //...00000111
	int status;
	int diff1and2;
	int diff2and3;

    initUart();
	sendString("\n\rRunning...\n\n\r");

	//setup master timing inputs and outputs
	ADPCFG=0b1111111111111111;//turn off all analog stuff
	TRISBbits.TRISB0=1; //Hydrophone input 1
	TRISBbits.TRISB1=1;	//Hydrophone input 2
	TRISBbits.TRISB2=1;	//Hydrophone input 3
	TRISBbits.TRISB3=1; //FFT validation input

	//setup "master timer"
	T2CONbits.T32=0; //turn off 32 bit operation
	T2CONbits.TCKPS=0b11;
	T2CONbits.TON=1;
	
	

	//start watching inputs
	while(1){
		status=PORTB&hydrophoneMask;
		sendNum(status);sendString("\n\r");

		if(status){
		//////////////////////////////////
		//TURN OFF INTERUPTS HERE!!!  ////
		//////////////////////////////////
		TMR2=0; //reset timer
		diff1and2=33333;
		diff2and3=22222;
		if(status==1){ //!=0 then
			//Hydrophone 1 sees a signal
			sendString("Hydrophone 1 sees a signal\n\r");
			while(status==1){
				status=PORTB&hydrophoneMask;
				if(status==3){
					sendString("Hydrophone 2 sees a signal\n\r");
					diff1and2=TMR2;
					while(status==3){
						status=PORTB&hydrophoneMask;
						if(status==7){
							sendString("Hydrophone 3 sees signal last\n\r");
							diff2and3=TMR2-diff1and2;
						}
					}
				}else if(status==5){
					sendString("Hydrophone 3 sees a signal\n\r");
					diff1and2=TMR2;
					while(status==5){
						status=PORTB&hydrophoneMask;
						if(status==7){
							int temp;
							sendString("Hydrophone 2 sees signal last\n\r");
							temp=TMR2;
							diff1and2=temp;
							diff2and3=temp-diff2and3;
						}
					}
				}else if(status==7){
					sendString("Hydrophones 2 and 3 see a signal\n\r");
					diff1and2=TMR2;
					diff2and3=0;
				}
			}

		}else if(status==2){
			//Hydrophone 2 sees a signal
			sendString("Hydrophone 2 sees a signal\n\r");
			while(status==2){
				status=PORTB&hydrophoneMask;
				if(status==3){
					//hydrophone 1 checks in
					sendString("Hydrophone 1 sees a signal\n\r");
					diff1and2=TMR2*-1;
					while(status==3){
						status=PORTB&hydrophoneMask;
						if(status==7){
							int temp;
							sendString("Hydrophone 3 sees signal last\n\r");
							diff2and3=TMR2;
						}
					}
				}else if(status==6){
					//hydrophone 3 checks in
					sendString("Hydrophone 2 sees a signal\n\r");
					diff2and3=TMR2;
					while(status==6){
						status=PORTB&hydrophoneMask;
						if(status==7){
							int temp;
							sendString("Hydrophone 1 sees signal last\n\r");
							diff1and2=TMR2*-1;
						}
					}
				}else if(status==7){
					//hydropohne 1 and 3 check in
					sendString("Hydrophone 1 and 3 see a signal\n\r");
					diff1and2=TMR2*-1;
					diff2and3=TMR2;
				}
			}

		}else if(status==3){
			//Hydrophone 1 & 2 sees a signal
			diff1and2=0;
			sendString("Hydrophone 1 & 2 sees a signal\n\r");
			while(status==3){
				status=PORTB&hydrophoneMask;
				if(status==7){
					diff2and3=TMR2;
					sendString("Hydrophone 3 sees a signal last\n\r");
				}
			}
			
		}else if(status==4){
			//Hydrophone 3 sees a signal
			sendString("Hydrophone 3 sees a signal\n\r");
			while(status==4){
				status=PORTB&hydrophoneMask;
				if(status==5){
					int temp = TMR2;
					sendString("Hydrophone 1 sees a signal\n\r");
					while(status==5){
						status=PORTB&hydrophoneMask;
						if(status==7){
							int temp2 = TMR2;
							sendString("Hydrophone 2 sees signal last\n\r");
							diff1and2=temp2-temp;
							diff2and3=temp2*-1;
						}
					}
				}else if(status==6){
					diff2and3=TMR2*-1;
					sendString("Hydrophone 2 sees a signal\n\r");
					while(status==6){
						status=PORTB&hydrophoneMask;
						if(status==7){
							sendString("Hydrophone 1 sees signal last\n\r");
							diff1and2=diff2and3-TMR2;
						}
					}
				}else if(status==7){
					sendString("Hydrophone 1 & 2 see a signal\n\r");
					diff2and3=TMR2*-1;
					diff1and2=0;
				}
			}

		}else if(status==5){
			//Hydrophone 1 & 3 sees a signal
			sendString("Hydrophone 1 & 3 see a signal\n\r");
			while(status==5){
				status=PORTB&hydrophoneMask;
				if(status==7){
					sendString("Hydrophone 2 sees the signal\n\r");
					diff1and2=TMR2;
					diff2and3=TMR2*-1;
				}
			}
		}else if(status==6){
			//Hydrophone 2 & 3 sees a signal
			diff2and3=0;
			sendString("Hydrophone 2 & 3 see a signal\n\r");
			while(status==6){
				status=PORTB&hydrophoneMask;
				if(status==7){
					diff1and2=TMR2*-1;
					sendString("Hydrophone 1 sees signal last\n\r");
				}
			}
		}else if(status==7){
			//all signals arrived at the same time
			diff1and2=0;
			diff2and3=0;
			sendString("ALL Hydrophones recieved signal simultaneously... dead above pinger\n\r");
		}
		sendString("___diff1and2 = ");sendNum(diff1and2);sendString("\n\r");
		sendString("___diff2and3 = ");sendNum(diff2and3);sendString("\n\r");
		delay();
		}

	}
	while (1);
}


void delay(){
	//sendNum(timerCounter);
	//sendString(" timer started\n\r");
	T1CONbits.TON=0;
	T1CONbits.TCKPS=0b11;
	TMR1=0;
	T1CONbits.TON=1;
	while(TMR1<0b1111111111111110){}
	T1CONbits.TON=0;
	//sendNum(timerCounter++);
	//sendString(" timer STOPPED\n\r");
}

//////////////////////////////////////////////
//Use this function to send 1 byte to the UART
void sendByte(byte i){
	while(U1STAbits.UTXBF);
    U1TXREG = i;
    while(U1STAbits.UTXBF);
}

//////////////////////////////////////////////
//Use this function to send literal strings in quotes as
//ASCII bytes to the UART
void sendString(unsigned char str[]){
    byte i=0;
    for(i=0; str[i]!=0; i++){
        sendByte(str[i]);
	}
}

//////////////////////////////////////////////
//Use this function to send an unsigned integer to the UART
//as ASCII text
void sendNum(unsigned int i){
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
    U1MODE = 0x0000;
    U1BRG = 7;  // 25 for baud of 38400 //7 for baud of 230400 pll8
    U1MODEbits.ALTIO = 1;   // Use alternate IO
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;   // Enable transmit
}


