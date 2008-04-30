/* Author: Scott Watson
 * Date: April 19, 2008
 * last modified(If I remembered to update this): April 20, 2008
 * (for real)
 * 
 * This project demonstrates algorithms for the AUV2008 SONAR
 * on a autonomous groud vehicle platform using piezo electric
 * transducers instead of hydrophones.  
 *
 * code written for a dsPIC30F4012 running with a 7.5MHz crystal*/
 
#include "p30fxxxx.h"
#include "scottcommon.h"
#include "iir.h"
#include "uart.h"
#include "adc.h"
#include "time.h"
#include "math.h"
#define UART_ENABLED 1  //set to 0 to turn off uart



//Configuration settings for the dsPIC30F4012
_FOSC(CSW_FSCM_OFF & FRC_PLL16); //to get 30MIPS with 7.5MHz clock
_FWDT(WDT_OFF);	
_FGS(CODE_PROT_OFF);
_FBORPOR(PBOR_OFF & PWRT_64);

int dutyCycle1;
int dutyCycle2;

int data1[SAMPLE_LENGTH];
int data2[SAMPLE_LENGTH];
int data3[SAMPLE_LENGTH];
int filtered1;
int filtered2;
int filtered3;
unsigned int count;

	
int main(void){
//declare main() variables

TRISEbits.TRISE0 = 0;
PORTEbits.RE0 = 1;

//initialize comms
	init_IO();
	init_Uart(UART_ENABLED);
	startup_flashes_text();
//initialize variables
	dutyCycle1 = 710;
	dutyCycle2 = 710;
	count=0;
	filtered1=0;
	filtered2=0;
	filtered3=0;
//initialize controller
	init_Timer2and3();
	init_OutputCompare();
	ADC_Init(); 

	int SGcount = 0;
	int mem[] = {0,0};
	int v=0;
	int sample = 234;
	int sampleNold = 123;
	int outputReal = 0;
	int outputImag = 0;
	int intermediate;
	//k=4 N=32 fs=20ksps delay=1.6ms
	
	/*  EDU>> approxCoef(cos(pi/4),.0001)
	result = slight underestimate
	ans = 1     0     1     1     0     1     0     1 
	EDU>> approxCoef((2*cos((2*pi*k)/N))-1,.00005)
	result = slight underestimate
	ans = 0     1     1     0     1     0     1     0     0     0     0     0     1 */

	intermediate = mem[1] + mem[1]>>2 + mem[1]>>3 + mem[1]>>5 + mem[1]>>7 + mem[1]>>13;
	v = sample + intermediate - mem[2];
	if(SGcount>32+1) v-=sampleNold;
	//since 2pik/N = pi/4 ... cos and sin terms are same and only 2 instead of 3 multiplications are needed
	intermediate = mem[1]>>1 + mem[1]>>3 + mem[1]>>4 + mem[1]>>6 + mem[1]>>8;
	outputReal = v - intermediate;
	outputImag =  intermediate;
	mem[2]=mem[1];
	mem[1]=v;
	SGcount++;
	while(1){
		} //interupt driven from here on out
}

void _ISRFAST _U1RXInterrupt(void){//AltU1RXInterrupt
	char charIn = U1RXREG;
	if(charIn=='1'){
		dutyCycle1+=10;
		OC1RS=dutyCycle1;
	}else if(charIn=='2'){
		dutyCycle1-=10;
		OC1RS=dutyCycle1;
	}else if(charIn=='3'){
		dutyCycle2+=10;
		OC2RS=dutyCycle2;
	}else if(charIn=='4'){
		dutyCycle2-=10;
		OC2RS=dutyCycle2;
	}
	U1STAbits.OERR=0;
	IFS0bits.U1RXIF = 0; //clear interupt flag
}	

void _ISRFAST _T2Interrupt(void){
	//OC2 timer event (control loop)
//OC2RS=dutyCycle1;
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
}

void init_IO(void){
	//setup analog and digital inputs and outputs
	//this must agree with ADC initialization...
	ADPCFG=0b1111111111110000;//turn off all analog stuff except for AN0,AN1,AN2,AN3
	
	TRISBbits.TRISB0=1; //AN0
	TRISBbits.TRISB1=1;	//AN1
	TRISBbits.TRISB2=1;	//AN2
	TRISBbits.TRISB3=1;	//AN3  //not needed, but maybe this helps? becaues simultaneous sampling only works for gruops of 1,2,or 4
	
	TRISEbits.TRISE0=0;
	PORTEbits.RE0=0;
	
	//TRISBbits.TRISB2=0;	//status LED
	//PORTBbits.RB2=0;
	//TRISBbits.TRISB3=0; //status LED
	//PORTBbits.RB3=0;
}

void init_OutputCompare(void){
	/*  OUTPUT COMPARE CONFIGURATION CALCULATIONS
	instruction clock cycle time.... 30MIPS... 33ns
	using timer prescaler of 1:64 so 2.112us per TMR2 incriment
	2.112us * 2^16 = 138.4ms maximum period
	1ms = 473.5 timer ticks
	1.5ms = 710.3 timer ticks
	2ms = 947 timer ticks
	25ms = 11,837.1 timer ticks
	*/	
	OC1CONbits.OCM = 0b000; //keep OC2 off while mod's are made
	OC1CONbits.OCSIDL = 0; //0=continued operation in idle mode
	OC1CONbits.OCTSEL = 0; //1=timer3  0=timer2
	OC1R=dutyCycle1;
	OC1RS=dutyCycle1;
	OC1CONbits.OCM = 0b110; //initialize OC2 low, generate continuous output pulses
	
	OC2CONbits.OCM = 0b000; //keep OC2 off while mod's are made
	OC2CONbits.OCSIDL = 0; //0=continued operation in idle mode
	OC2CONbits.OCTSEL = 0; //1=timer3  0=timer2
	OC2R=dutyCycle2;
	OC2RS=dutyCycle2;
	OC2CONbits.OCM = 0b110; //initialize OC2 low, generate continuous output pulses
}	

void startup_flashes_text(void){
	PORTBbits.RB3=1; PORTEbits.RE0=1; delay(200);
	PORTBbits.RB3=0; PORTEbits.RE0=0; delay(200);  
	PORTBbits.RB3=1; PORTEbits.RE0=1; delay(200);  
	PORTBbits.RB3=0; PORTEbits.RE0=0; delay(200);  
	PORTBbits.RB3=1; PORTEbits.RE0=1; 
	sendString("\n\rRunning...\n\r");
	//sendString("Starting at duty cycle = ");
	
	//enabling the following line causes the following errors
/*
CORE-E0011: Trap due to unimplemented RAM or PSV memory access, occurred from instruction at 0x0001c2
CORE-E0001: Trap due to stack error, occurred from instruction at 0x0001c2
CORE-E0003: Trap due to unimplemented RAM memory access, occurred from instruction at 0x0001c2
CORE-E0003: Trap due to unimplemented RAM memory access, occurred from instruction at 0x0001c2
CORE-E0003: Trap due to unimplemented RAM memory access, occurred from instruction at 0x0001c2
CORE-E0003: Trap due to unimplemented RAM memory access, occurred from instruction at 0x0001c2
*/
	
	//sendNum(dutyCycle1);sendString("\n\r");
}	




