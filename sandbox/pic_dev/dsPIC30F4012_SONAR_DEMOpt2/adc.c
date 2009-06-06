#include "p30fxxxx.h"
#include "scottcommon.h"
#include "iir.h"
#include "adc.h"
#include "uart.h"
#include "time.h"

#define M_SLIDING_DFT_k 4
#define M_SLIDING_DFT_N 32
#define M_SLIDING_DFT_nchannels 4
#include <dft_singleton_c30.h>

#define DFT_MAG_THRESHOLD 1000
sliding_dft_t *dft;

//Functions and Variables with Global Scope:
extern int data1[SAMPLE_LENGTH];
extern int data2[SAMPLE_LENGTH];
extern int data3[SAMPLE_LENGTH];
extern int filtered1;
extern int filtered2;
extern int filtered3;
extern unsigned int count;

//filter memory
float s1[] = {0,0,0};
float s2[] = {0,0,0};
float s3[] = {0,0,0};

//_ADCInterrupt() is the A/D interrupt service routine (ISR).
//The routine must have global scope in order to be an ISR.
//The ISR name is chosen from the device linker script.
void _ISR _ADCInterrupt(void){
	//ADCBUF0 contains AN3 information (not needed until a 4th channel is added to the system
	/*data1[count] = ADCBUF1;
	data2[count] = ADCBUF2;
	data3[count++] = ADCBUF3;*/
	
	
	//	Copy and cast converted ADC data to the correct type
	adcdata_t rawData[M_SLIDING_DFT_nchannels];
	
	rawData[0] = ADCBUF1;
	
	
	//	Propate the DFT
	dft_update(dft, rawData);
	if(dft->mag[0]>32000){
		PORTEbits.RE0=1;
	}else{
		PORTEbits.RE0=0;
	}
	
	//sendNum(dft->mag[0]);
	
	/*if (abs(rawData[0]) > 900)
		PORTEbits.RE0=1;
	else
		PORTEbits.RE0=0;*/
	
	/*if(count>=SAMPLE_LENGTH){
		count=0;
		send_samples_over_UART_as_bytes(0);
	}*/
	
	//sendByte((byte)(ADCBUF0>>2));
	//sendNum(ADCBUF0); // now comes in correctly as AN0 (timing check still required)
	//sendNum((int)filtered1);
	//sendString(" : ");
	//sendByte((byte)(ADCBUF1>>2));
	//sendNum(ADCBUF1); // now comes in correctly as AN1 (timing check still required)
	//sendNum((int)filtered2);
	//sendString(" : ");
	//sendByte((byte)(ADCBUF2>>2));
	//sendNum(ADCBUF2); // now comes in correctly as AN1 (timing check still required)
	//sendNum((int)filtered3);
	//sendString("\n\r");
	
	//Clear the A/D Interrupt flag bit or else the CPU will keep vectoring back to the ISR
    IFS0bits.ADIF = 0;
}

/*ADC_Init() is used to configure A/D to SIMULTANEOUSLY sample 4 inputs on
pins AN0-AN3 (discarding AN3 in software) and then convert them all and 
only then supply an interrupt. The A/D is set up for a sampling rate 
of 20kHz.  Timer3 is used to provide sampling time delay. */
void ADC_Init(void){
	//	Initialize the sliding DFT
	dft = M_DFT_INIT();
	
	/*  ___ADCON1 Register___ */
	ADCON1bits.FORM = 01;//was 00		//unsigned integer output
	ADCON1bits.SSRC = 0b010;	//sampling rate set by TMR3 compare event
	ADCON1bits.SIMSAM = 1;		//enable simultaneous sampling
	ADCON1bits.ASAM = 1;		//enable auto sampling
	
	/*  ___ADCON2 Register___
	Set up A/D for interrupting after 2 samples get filled in the buffer
	Set up to sample on 2 S/H amplifiers - CH0 and CH1 */
	ADCON2bits.VCFG = 0b000;	//Ideally use external references (here uses AVdd and AVss)
	ADCON2bits.CSCNA = 0; 		//Scan Input Selections for CH0+ S/H Input for MUX A Input Multiplexer Setting bit
	ADCON2bits.CHPS = 0b10;	//selects CH0,CH1,CH2,CH3
	ADCON2bits.SMPI = 0b0011;	//interupts at completion of 4th sample/convert sequence
	
	/*___ADCON3 Register___*/
	//SAMC needs updating to 0b'00001' or ob'00010'
	ADCON3bits.SAMC = 2;	//minimum sampling time  //was 0;
	ADCON3bits.ADCS = 9;	//minimum Tad time   //was 4;

	/*___ADCHS Register___
	Set up A/D Channel Select Register to convert AN3 on Mux A input
	of CH0 and CH1 S/H amplifiers		*/
	ADCHS = 0b0000000000000011;		//CH0 samples AN3 (CH1-3 are doing AN0-2) however I'm probably ignoring this bit anyway
    //0x23 = 0000 0000 0010 0011  //previous value
    
	/*___ADCSSL Register___
	Channel Scanning is disabled. All bits left to their default state*/
	ADCSSL = 0b0000000000000111;  //AN0,AN1,AN2 are in the scan sequence (ignored when CSNA==0)

	/*___ADPCFG Register___
	Set up channels AN7 as analog input and configure rest as digital
	Recall that we configured all A/D pins as digital when code execution
	entered main() out of reset  */
	ADPCFG = 0xFFFF;
	ADPCFGbits.PCFG0 = 0; //AN0 is set for analog input
	ADPCFGbits.PCFG1 = 0; //AN1 is set for analog inupt
	ADPCFGbits.PCFG2 = 0; //AN2 is set for analog inupt
	ADPCFGbits.PCFG3 = 0; //AN2 is set for analog inupt

	IFS0bits.ADIF = 0;	//Clear the A/D interrupt flag bit
	IEC0bits.ADIE = 1;	//Set the A/D interrupt enable bit

	//Turn on A/D converter - typically done after configuring other registers
	ADCON1bits.ADON = 1;
}

void send_samples_over_UART_as_bytes(int start){
	int i=0;
	disableTimer3(); //no sense in sampling while we are transmitting
	for(i=start; i<SAMPLE_LENGTH; i++){
		sendByte(data1[i]);
		sendByte(data2[i]);
		sendByte(data3[i]);
	}
	for(i=0; i<start; i++){
		sendByte(data1[i]);
		sendByte(data2[i]);
		sendByte(data3[i]);
	}
	enableTimer3();
}

void send_samples_over_UART_as_ASCII(){
	int i=0;
	for(i=0; i<SAMPLE_LENGTH; i++){			
		sendNum(data1[i]);
		if(i%10==0) sendString("\n\r");
	}
}
