/* Author: Scott Watson
 * Date: April 16, 2008
 * last modified(If I remembered to update this): April 16, 2008
 * ENEE461 student project
 * Project Spy Ride - a distance following controller
 * 
 * code written for a dsPIC30F4012 running with a 7.5MHz crystal*/
 
#include "p30fxxxx.h"
#include "scottcommon.h"
#define UART_ENABLED 1  //set to 0 to turn off uart

//Configuration settings for the dsPIC30F4012
_FOSC(CSW_FSCM_OFF & XT_PLL16); //to get 30MIPS with 7.5MHz clock
_FWDT(WDT_OFF);	
_FGS(CODE_PROT_OFF);
_FBORPOR(PBOR_OFF & PWRT_64);

volatile int dutyCycle = 710;
int data1[SAMPLE_LENGTH];
int data2[SAMPLE_LENGTH];
int filtered1;
int filtered2;
unsigned int count;

int main(void){
//initialize comms
	init_IO();
	init_Uart(UART_ENABLED);
	startup_flashes_text();
//initialize variables
	count=0;
	filtered1=0;
	filtered2=0;
//initialize controller
	init_Timer2and3();
	init_OC2();
	ADC_Init(); 
	
	while(1){} //interupt driven from here on out
}

void _ISRFAST _T2Interrupt(void){
	//OC2 timer event (control loop)
//OC2RS=dutyCycle;
	IFS0bits.T2IF=0;
	//sendString("\t(in control loop)\n\r");
	//sendNum(2);
	
	/*based on pseudocode from http://en.wikipedia.org/wiki/PID_controller
	start:
  		previous_error = error or 0 if undefined
  		error = setpoint - actual_position
  		P = Kp * error
  		I = I + Ki * error * dt
  		D = Kd * (error - previous_error) / dt
  		output = P + I + D
  		wait(dt)
  		goto start
	*/
}
void _ISRFAST _T3Interrupt(void){
	//ADC timer event
	IFS0bits.T3IF=0;
	//sendNum(3);
}

void init_IO(void){
	//setup analog and digital inputs and outputs
	ADPCFG=0b1111111111111100;//turn off all analog stuff except for AN0 and AN1
	
	TRISBbits.TRISB0=1; //left button
	TRISBbits.TRISB1=1;	//right button
	TRISBbits.TRISB2=0;	//status LED
	PORTBbits.RB2=0;
	TRISBbits.TRISB3=0; //status LED
	PORTBbits.RB3=0;
}

void init_OC2(void){
	/*  OUTPUT COMPARE CONFIGURATION CALCULATIONS
	instruction clock cycle time.... 30MIPS... 33ns
	using timer prescaler of 1:64 so 2.112us per TMR3 incriment
	2.112us * 2^16 = 138.4ms maximum period
	1ms = 473.5 timer ticks
	1.5ms = 710.3 timer ticks
	2ms = 947 timer ticks
	25ms = 11,837.1 timer ticks
	*/	
	OC2CONbits.OCM = 0b000; //keep OC2 off while mod's are made
	OC2CONbits.OCSIDL = 0; //0=continued operation in idle mode
	OC2CONbits.OCTSEL = 0; //1=timer3  0=timer2
	OC2R=dutyCycle;
	OC2RS=dutyCycle;
	OC2CONbits.OCM = 0b110; //initialize OC2 low, generate continuous output pulses
}	

void startup_flashes_text(void){
	PORTBbits.RB2=1; delay(200);
	PORTBbits.RB2=0; delay(200);  
	PORTBbits.RB2=1; delay(200);  
	PORTBbits.RB2=0; delay(200);  
	PORTBbits.RB2=1;
	sendString("\n\rRunning...\n\r\n\r");
	sendString("Starting at duty cycle = ");
	sendNum(dutyCycle);sendString("\n\r");
}	

void init_Timer2and3(void){
	//setup TMR2 and TMR3... in this project, 
	//TMR2 is used to do continuous PWM on OC2
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
	PR3=2369;		//5ms period for 200Hz operation
	IEC0bits.T3IE=1;  //enable interupts for Timer3
	IPC1bits.T3IP=4;  //interupt priority0-7 where 7 is the highest priority
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


