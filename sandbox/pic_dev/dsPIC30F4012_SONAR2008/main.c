/* Author: Scott Watson
 * Date: May 1, 2008
 * last modified(If I remembered to update this): May 1, 2008
 * 
 * This is a branch of some of my commonly used dsPIC30F4012 code
 * that is meant to be used for a 4 channel simultaneous sampling 
 * sonar system.  I'm going to try and push the sampling as fast 
 * as possible within the ability of the various recursive filters
 * to run within the number of samples.
 *
 * code written for a dsPIC30F4012 running with a 7.5MHz crystal to run at 30MIPS*/
 
#include "p30fxxxx.h"
#include "scottcommon.h"
#include "iir.h"
#include "uart.h"
#include "adc.h"
#include "time.h"
#include "math.h"
#define UART_ENABLED 1  //set to 0 to turn off uart

//Configuration settings for the dsPIC30F4012
_FOSC(CSW_FSCM_OFF & XT_PLL16); //to get 30MIPS with 7.5MHz clock
_FWDT(WDT_OFF);	
_FGS(CODE_PROT_OFF);
_FBORPOR(PBOR_OFF & PWRT_64);

int data1[SAMPLE_LENGTH];
int data2[SAMPLE_LENGTH];
int data3[SAMPLE_LENGTH];
int data4[SAMPLE_LENGTH];
int filtered1;int filtered2;int filtered3;int filtered4;
int mem1a;int mem1b;
int mem2a;int mem2b;
int mem3a;int mem3b;
int mem4a;int mem4b;
byte found1;byte found2;
byte found3;byte found4;
byte foundTime1;byte foundTime2;
byte foundTime3;byte foundTime4;
unsigned int count;

int main(void){
//declare main() variables
	
//initialize comms
	init_IO();
	init_Uart(UART_ENABLED);
	startup_flashes_text();
//initialize variables
	clearData();
	count=0;
	filtered1=0; filtered2=0; filtered3=0;
//initialize controller
	init_Timer2and3();
	ADC_Init(); 
	

	while(1){} //interupt driven from here on out
}

void _ISRFAST _U1RXInterrupt(void){//AltU1RXInterrupt
	char charIn = U1RXREG;
	if(charIn=='1'){
		//do stuff
	}else if(charIn=='2'){
		//do stuff
	}else if(charIn=='3'){
		//do stuff
	}else if(charIn=='4'){
		//do stuff
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
	
	TRISEbits.TRISE0=0;	//status LED1
	PORTEbits.RE0=0;
	TRISEbits.TRISE1=0;	//status LED2
	PORTEbits.RE1=0;
	TRISEbits.TRISE2=0;	//status LED3
	PORTEbits.RE2=0;
}

void startup_flashes_text(void){
	PORTEbits.RE0=1; delay(200);
	PORTEbits.RE0=0; delay(200);  
	PORTEbits.RE0=1; delay(200);  
	PORTEbits.RE0=0; delay(200);  
	PORTEbits.RE0=1; delay(200);
	PORTEbits.RE0=0;
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


void init_CORCON(){ //just does default configuration for now... (I THINK, NOT TESTED)
/* On device Reset, the CORCON is set to 0x0020, which sets the following mode:
- Program Space not Mapped to Data Space (PSV = 0)
- ACCA and ACCB Saturation Disabled (SATA = 0, SATB = 0)
- Data Space Write Saturation Enabled (SATDW = 1)
- Accumulator Saturation mode set to normal (ACCSAT = 0)
- Accumulator Rounding mode set to unbiased (RND = 0)
- DSP Multiplier mode set to signed fractional (US = 0, IF = 0)
In addition to setting CPU modes, the CORCON contains status information about the 
DO loop nesting level (DL<2:0>) and the IPL<3> status bit, which indicates if a 
trap exception is being processed.*/
	CORCONbits.IF = 0;		// Integer/Fractional mode 
	CORCONbits.RND =0;		// Rounding mode
	CORCONbits.PSV = 0;		// Program Space Visibility enable
	CORCONbits.IPL3 = 0;	// CPU Interrupt Priority Level bit 3
	CORCONbits.ACCSAT = 0;	// Acc saturation mode
	CORCONbits.SATDW = 1;	// Data space write saturation enable
	CORCONbits.SATB = 0;	// Acc B saturation enable
	CORCONbits.SATA = 0;	// Acc A saturation enable
	CORCONbits.DL = 0;		// DO loop nesting level status
	CORCONbits.EDT = 0;		// Early DO loop termination control
	CORCONbits.US = 0;		// Signed/Unsigned mode
}	
