#include "p30fxxxx.h"
#include "scottcommon.h"

//Functions and Variables with Global Scope:
extern int data1[SAMPLE_LENGTH];
extern int data2[SAMPLE_LENGTH];
extern int filtered1;
extern int filtered2;
extern unsigned int count;

/*ADC_Init() is used to configure A/D to convert 1 samples 
of 2 input channel per interrupt. The A/D is set up for a 
sampling rate of 200SPS.  Timer3 is used to provide sampling 
time delay.  The input pin being acquired and converted is AN1.*/
void ADC_Init(void){
	/*  ___ADCON1 Register___ */
	ADCON1bits.FORM = 0;		//unsigned integer output
	ADCON1bits.SSRC = 0b010;	//sampling rate set by TMR3 compare event
	ADCON1bits.SIMSAM = 0;		//enable sequential sampling
	ADCON1bits.ASAM = 1;		//enable auto sampling
	
	/*  ___ADCON2 Register___
	Set up A/D for interrupting after 2 samples get filled in the buffer
	Set up to sample on 2 S/H amplifiers - CH0 and CH1 */
	ADCON2bits.VCFG = 0b000;	//Ideally use external references (here uses AVdd and AVss)
	ADCON2bits.CSCNA = 1;
	ADCON2bits.CHPS = 1;
	ADCON2bits.CHPS = 0b00;	//selects CH0 
	ADCON2bits.SMPI = 1;	//interupts at completion of 2nd sample/convert sequence
	
	/*___ADCON3 Register___*/
	//SAMC needs updating to 0b'00001' or ob'00010'
	ADCON3bits.SAMC = 2;	//minimum sampling time  //was 0;
	ADCON3bits.ADCS = 9;	//minimum Tad time   //was 4;

	/*___ADCHS Register___
	Set up A/D Channel Select Register to convert AN3 on Mux A input
	of CH0 and CH1 S/H amplifiers		*/
	ADCHS = 0b0000000000000000;		//sets analog input pins to the channels
    //0x23 = 0000 0000 0010 0011  //previous value
    
	/*___ADCSSL Register___
	Channel Scanning is disabled. All bits left to their default state*/
	ADCSSL = 0x0003;  //AN0 and AN1 are in the scan sequence

	/*___ADPCFG Register___
	Set up channels AN7 as analog input and configure rest as digital
	Recall that we configured all A/D pins as digital when code execution
	entered main() out of reset  */
	ADPCFG = 0xFFFF;
	ADPCFGbits.PCFG0 = 0; //AN0 is set for analog input
	ADPCFGbits.PCFG1 = 0; //AN1 is set for analog inupt

	IFS0bits.ADIF = 0;	//Clear the A/D interrupt flag bit
	IEC0bits.ADIE = 1;	//Set the A/D interrupt enable bit

	//Turn on A/D converter - typically done after configuring other registers
	ADCON1bits.ADON = 1;
}

//_ADCInterrupt() is the A/D interrupt service routine (ISR).
//The routine must have global scope in order to be an ISR.
//The ISR name is chosen from the device linker script.
void _ISR _ADCInterrupt(void){
	//result[count++] = (byte)(ADCBUF0>>2);
	//result[count++] = (byte)(ADCBUF1>>2);
	//sendNum(result[count-2]);
	//sendNum(result[count-1]);
	sendNum(ADCBUF0); // now comes in correctly as AN0 (timing check still required)
	sendNum(ADCBUF1); // now comes in correctly as AN1 (timing check still required)
	sendString("\n\r");
	
       /*if(count>SAMPLE_LENGTH){
			int i=0;
			for(i=0; i<1950; i++){
				sendByte(result[i]);
			}
			count = 0; 
		}*/
		
	//Clear the A/D Interrupt flag bit or else the CPU will
    //keep vectoring back to the ISR
    IFS0bits.ADIF = 0;
}

void send_samples_over_UART_as_bytes(int start){
	int i=0;
	for(i=start; i<SAMPLE_LENGTH; i++){
		sendByte(data1[i]);
	}
	for(i=0; i<start; i++){
		sendByte(data1[i]);
	}
}

void send_samples_over_UART_as_ASCII(){
	int i=0;
	for(i=0; i<SAMPLE_LENGTH; i++){			
		sendNum(data1[i]);
		if(i%10==0) sendString("\n\r");
	}
}