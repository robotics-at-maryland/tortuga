//Scott Watson - April 20, 2008
//timer and timeing code

#include "p30fxxxx.h"
#include "scottcommon.h"
#include "time.h"


void init_Timer2(void){
	//TMR2 is used to do continuous PWM on OC1 and OC2
	T2CONbits.T32=0; //turn off 32 bit operation
	T2CONbits.TCKPS=0b10;  // 1:64 prescale value
	T2CONbits.TSIDL=0; //continue in idle mode
	T2CONbits.TCS=0; //internal clock  (F_OSC/4)
	T2CONbits.TON=1;
	PR2=11845; //25ms period for 40Hz operation
	IEC0bits.T2IE=1;  //enable interupts for Timer2
	IPC1bits.T2IP=6;  //interupt priority0-7 where 7 is the highest priority 
	//(here the control loop timer has priority over the sampling timer)
}	
void init_Timer3(void){
	//TMR3 is used to trigger the ADC event
	T3CONbits.TCKPS=0b00; // 1:1 prescale value
	T3CONbits.TSIDL=0; //continue in idle mode
	T3CONbits.TCS=0; //internal clock  (F_OSC/4)
	T3CONbits.TON=1;
	PR3=60;		// set for 125ksps on each channel (exact)
	IEC0bits.T3IE=1;  //enable interupts for Timer3
	IPC1bits.T3IP=6;  //interupt priority0-7 where 7 is the highest priority
}


/*for prescaler of 1:64 at 30MIPS
	PR3=1 - 468.75kHz or 117.1875kHz per channel
	PR3=2 - 234.375kHz or 58.59375 kHz/Channel
	PR3=3 - 156.25kHz or 39.0625 kHz/Channel
	PR3=4 - 117.1875kHz or 29.296875 kHz/Channel
	PR3=5 - 93.75kHz or 23.4375	kHz/Channel
	PR3=11 - 42.613kHz or 10.65340909 kHz/Channel
	PR3=23 - 20.380kHz or 5.095kHz/Channel
	PR3=46 - 10.190kHz or 2.547kHz/Channel
	PR3=47 - 9.973kHz or 2.493kHz/Channel
	PR3=93 - 5.040kHz or 1.260kHz/Channel
	PR3=94 - 4.986kHz or 1.246kHz/Channel
for prescaler of 1:16 at 30MIPS
	PR3=2	937.5kHz or 234.375kHz/Channel
	PR3=3	625kHz or 156.25kHz/Channel
	PR3=4	468.75kHz or 117.1875kHz/Channel
	PR3=5	375kHz or 93.75kHz/Channel
	PR3=6	312.5kHz or 78.125kHz/Channel
	PR3=7	267.857kHz or 66.964kHz/Channel
	PR3=8	234.375kHz or 58.593kHz/Channel
	PR3=9	208.333kHz or 52.083kHz/Channel
	PR3=10	187.5kHz or 46.875kHz/Channel
for prescaler of 1:1 at 30MIPS
	PR3=30	1000kHz	250	kHz/Channel
	PR3=34	882.352kHz or 220.588kHz/Channel
	PR3=37	810.810kHz or 202.702kHz/Channel
	PR3=38	789.473kHz or 197.368kHz/Channel
	PR3=40	750kHz or 187.5kHz/Channel
	PR3=41	731.707kHz or 182.926kHz/Channel
	PR3=44	681.818kHz or 170.454kHz/Channel
	PR3=50	600kHz or 150kHz/Channel
	PR3=60	500kHz	125kHz/Channel
	PR3=62	483.870kHz or 120.967kHz/Channel
	PR3=63	476.190kHz or 119.047kHz/Channel 
	*/

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
