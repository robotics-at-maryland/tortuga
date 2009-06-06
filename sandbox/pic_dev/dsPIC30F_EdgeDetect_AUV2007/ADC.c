#include "p30fxxxx.h"
//#include <stdio.h>
#define byte unsigned char
#define SAMPLE_LENGTH 19
#define TRIGGER_POSITION 1500	//0-(SAMPLE_LENGTH-1)
#define TRIGGER_VALUE 650		//0-1023 in Volts

byte result[SAMPLE_LENGTH];
unsigned int count = 0;
byte trigger_status = 2;
unsigned int trigger_count = 0;


//Functions and Variables with Global Scope:
void ADC_Init(void);
void __attribute__((__interrupt__)) _ADCInterrupt(void);
void send_samples_over_UART_as_bytes(int start);
void send_samples_over_UART_as_ASCII(void);


/*ADC_Init() is used to configure A/D to convert 16 samples 
of 1 input channel per interrupt. The A/D is set up for a 
sampling rate of 1MSPS.  Timer3 is used to provide sampling 
time delay.  The input pin being acquired and converted is AN7.*/
void ADC_Init(void){
/*  ___ADCON1 Register___
Set up A/D for Automatic Sampling...
Use internal counter (SAMC) to provide sampling time.
Set up A/D conversion results to be read in 1.15 fractional
number format.

FORMAT modified?

Set up Sequential sampling for multiple S/H amplifiers
All other bits to their default state */
	//ADCON1bits.FORM = 0;	//
	//ADCON1bits.SSRC = 7;	//enable auto convert mode
	//ADCON1bits.ASAM = 1;	//enable auto sampling
	//ADCON1bits.SIMSAM = 1;	//enable simultaneous sampling

	//ADCON2bits.BUFM = 1;

/*  ___ADCON2 Register___
Set up A/D for interrupting after 2 samples get filled in the buffer
Set up to sample on 2 S/H amplifiers - CH0 and CH1
All other bits to their default state  */
	//ADCON2bits.SMPI = 1;	//ensure samples per interrupt is greater than 1
	//ADCON2bits.CHPS = 1;	//selects CH0 and CH1
	////ADCON2bits.VCFG = 3;	//Ideally use external references

/*___ADCON3 Register___
We would like to set up a sampling rate of 1 MSPS
Total Conversion Time= 1/Sampling Rate = 125 microseconds
At 29.4 MIPS, Tcy = 33.9 ns = Instruction Cycle Time
The A/D converter will take 12*Tad periods to convert each sample
So for ~1 MSPS we need to have Tad close to 83.3ns
Using equaion in the Family Reference Manual we have
	ADCS = 2*Tad/Tcy - 1  		*/

//SAMC needs updating to 0b'00001' or ob'00010'
	//ADCON3bits.SAMC = 0;	//minimum sampling time  //was 0;
	//ADCON3bits.ADCS = 4;	//minimum Tad time   //was 4;

/*___ADCHS Register___
Set up A/D Channel Select Register to convert AN3 on Mux A input
of CH0 and CH1 S/H amplifiers		*/
	//ADCHS = 0x0023;		//sets analog input pins to the channels

/*___ADCSSL Register___
Channel Scanning is disabled. All bits left to their default state*/
	//ADCSSL = 0x0000;

/*___ADPCFG Register___
Set up channels AN7 as analog input and configure rest as digital
Recall that we configured all A/D pins as digital when code execution
entered main() out of reset  */
	ADPCFG = 0xFFFF;
	ADPCFGbits.PCFG3 = 0;

	IFS0bits.ADIF = 0;	//Clear the A/D interrupt flag bit
	IEC0bits.ADIE = 1;	//Set the A/D interrupt enable bit

//Turn on A/D converter - typically done after configuring other registers
	//ADCON1bits.ADON = 1;
}


void send_samples_over_UART_as_bytes(int start){
	int i=0;
	for(i=start; i<SAMPLE_LENGTH; i++){
		sendByte(result[i]);
	}
	for(i=0; i<start; i++){
		sendByte(result[i]);
	}
}

void send_samples_over_UART_as_ASCII(){
	int i=0;
	for(i=0; i<SAMPLE_LENGTH; i++){			
		sendNum(result[i]);
		if(i%10==0) sendString("\n\r");
	}
}
