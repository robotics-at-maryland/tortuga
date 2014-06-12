//Scott Watson - April 20, 2008
//timer and timeing code

#include "p30fxxxx.h"
#include "scottcommon.h"
#include "time.h"

void init_Timer2and3(void){
	//setup TMR2 and TMR3... in this project, 
	//TMR2 is used to do continuous PWM on OC1 and OC2
	//TMR3 is used to trigger the ADC event
	//	if this is faster than 20Hz (the update rate of the Sharp IR range sensor)
	//	then we can filter ADC noise as well as general sensor noise
	
	//PWM timer
	T2CONbits.T32=0; //turn off 32 bit operation
	T2CONbits.TCKPS=0b10;  // 1:64 prescale value
	T2CONbits.TSIDL=0; //continue in idle mode
	T2CONbits.TCS=0; //internal clock  (F_OSC/4)
	T2CONbits.TON=1;
	PR2=11845; //25ms period for 40Hz operation
	IEC0bits.T2IE=1;  //enable interupts for Timer2
	IPC1bits.T2IP=6;  //interupt priority0-7 where 7 is the highest priority 
	//(here the control loop timer has priority over the sampling timer)
	
	//ADC timer
	T3CONbits.TCKPS=0b10; // 1:64 prescale value
	T3CONbits.TSIDL=0; //continue in idle mode
	T3CONbits.TCS=0; //internal clock  (F_OSC/4)
	T3CONbits.TON=1;
	PR3=5;//5.875=80kHz  11=40kHz 23=20kHz   47=10kHz   94=5kHz(split between all channels)   //2369=5ms period for 200Hz operation
	IEC0bits.T3IE=1;  //enable interupts for Timer3
	IPC1bits.T3IP=4;  //interupt priority0-7 where 7 is the highest priority
}


void disableTimer3(){
	T3CONbits.TON=0;
}
void disableTimer2(){
	T2CONbits.TON=0;
}
void disableTimer1(){
	T1CONbits.TON=0;
}

void enableTimer3(){
	T3CONbits.TON=1;
}
void enableTimer2(){
	T2CONbits.TON=1;
}
void enableTimer1(){
	T1CONbits.TON=1;
}

void delay(int mseconds){ //cannot be called with more than about 550ms
	const int onemillisecond = 0b000000001110101;
	//const int onemillisecond = 1;
	mseconds=mseconds*onemillisecond;
	//start config bit selection
	T1CONbits.TON=0;
	TMR1=0;
	T1CONbits.TCS=0; 
	T1CONbits.TCKPS=0b11;
	T1CONbits.TON=1;
	while(TMR1<mseconds){
		Nop();
	}
	T1CONbits.TON=0;
}
