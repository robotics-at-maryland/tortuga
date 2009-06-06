#include "p30fxxxx.h"
#include "scottcommon.h"
#include "iir.h"
#include "adc.h"
#include "uart.h"
#include "time.h"
#include "math.h"

//Functions and Variables with Global Scope:
extern int data1[SAMPLE_LENGTH];
extern int data2[SAMPLE_LENGTH];
extern int data3[SAMPLE_LENGTH];
extern int otherData[OTHERDATA_LENGTH];
extern int filtered1;
extern int filtered2;
extern int filtered3;
extern int mem1a;
extern int mem1b;
extern int mem2a;
extern int mem2b;
extern int mem3a;
extern int mem3b;
extern unsigned int count;

extern byte found1;
extern byte found2;
extern byte found3;
extern byte foundTime1;
extern byte foundTime2;
extern byte foundTime3;

//filter memory
float s1[] = {0,0,0};
float s2[] = {0,0,0};
float s3[] = {0,0,0};

//_ADCInterrupt() is the A/D interrupt service routine (ISR).
//The routine must have global scope in order to be an ISR.
//The ISR name is chosen from the device linker script.
void _ISR _ADCInterrupt(void){
	int v1; int v2; int v3;
	//ADCBUF0 contains AN3 information (not needed until a 4th channel is added to the system
	data1[count] = ADCBUF1;
	data2[count] = ADCBUF2;
	data3[count] = ADCBUF3;
	
	int outputReal1; int outputReal2; int outputReal3;
	int outputImag1; int outputImag2; int outputImag3;
	int output1; int output2; int output3;
	int intermediate1; int intermediate2; int intermediate3;
	int index;
	//k=4 N=32 fs=20ksps delay=1.6ms
	
//asm ("mov w0,%0" : "+r"(mem1));

///asm volatile(
//"mov #IPC2, w0	\n\t"
//"mov #IPC2, w0	\n\t" );
	
//0     0     0     4     5     6     0     0     0    10    11    12
	intermediate1 = mem1a + (mem1a>>4) + (mem1a>>5) + (mem1a>>6) + (mem1a>>10) + (mem1a>>11) + (mem1a>>12);
	v1 = ADCBUF1 + intermediate1 - mem1b;
	intermediate2 = mem2a + (mem2a>>4) + (mem2a>>5) + (mem2a>>6) + (mem2a>>10) + (mem2a>>11) + (mem2a>>12);
	v2 = ADCBUF2 + intermediate2 - mem2b;
	intermediate3 = mem3a + (mem3a>>4) + (mem3a>>5) + (mem3a>>6) + (mem3a>>10) + (mem3a>>11) + (mem3a>>12);
	v3 = ADCBUF3 + intermediate3 - mem3b;
	if(count>=32){
		index = count-N;
		v1-=data1[index];
		v2-=data2[index];
		v3-=data3[index];
	}else{
		index = SAMPLE_LENGTH-N+count;
		v1-=data1[index];
		v2-=data2[index];
		v3-=data3[index];
	}
//1     0     0     0     5     6     7     0     0    10
	intermediate1 = (mem1a>>1) + (mem1a>>5) + (mem1a>>6) + (mem1a>>7) + (mem1a>>10);
	outputReal1 = v1 - intermediate1;
	intermediate2 = (mem2a>>1) + (mem2a>>5) + (mem2a>>6) + (mem2a>>7) + (mem2a>>10);
	outputReal2 = v2 - intermediate2;
	intermediate3 = (mem3a>>1) + (mem3a>>5) + (mem3a>>6) + (mem3a>>7) + (mem3a>>10);
	outputReal3 = v3 - intermediate3;
//1     2     0     4     0     6     0     0     9    10    11
	intermediate1 = (mem1a>>1) + (mem1a>>2) + (mem1a>>4) + (mem1a>>6) + (mem1a>>9) + (mem1a>>10) + (mem1a>>11);
	outputImag1 =  -1 * intermediate1;
	intermediate2 = (mem2a>>1) + (mem2a>>2) + (mem2a>>4) + (mem2a>>6) + (mem2a>>9) + (mem2a>>10) + (mem2a>>11);
	outputImag2 =  -1 * intermediate2;
	intermediate3 = (mem3a>>1) + (mem3a>>2) + (mem3a>>4) + (mem3a>>6) + (mem3a>>9) + (mem3a>>10) + (mem3a>>11);
	outputImag3 =  -1 * intermediate3;
	if(outputReal1<0){ 
		output1 = -outputReal1;
	}else{output1 = outputReal1;}
	if(outputImag1<0){
		output1 = output1 - outputImag1;
	}else{output1 = output1 + outputImag1;}
	
	if(outputReal2<0){ 
		output2 = -outputReal2;
	}else{output2 = outputReal2;}
	if(outputImag2<0){
		output2 = output2 - outputImag2;
	}else{output2 = output2 + outputImag2;}
	
	if(outputReal3<0){ 
		output3 = -outputReal3;
	}else{output3 = outputReal3;}
	if(outputImag3<0){
		output3 = output3 - outputImag3;
	}else{output3 = output3 + outputImag3;}
		
	if(output1>2100){ //blue wire  //2100 when resetting every 250 samples
		PORTEbits.RE0=1;
		if(found1==0){
			foundTime1=count;
		}
		found1=1;
	}else{
		PORTEbits.RE0=0;
	}
	if(output2>1650){ //green wire //1650 when resetting every 250 samples
		PORTEbits.RE1=1;
		if(found2==0){
			foundTime2=count;
		}
		found2=1;
	}else{
		PORTEbits.RE1=0;
	}
	if(output3>1600){ //brown wire //1600 when resetting every 250 samples
		PORTEbits.RE2=1;
		if(found3==0){
			foundTime3=count;
		}
		found3=1;
	}else{
		PORTEbits.RE2=0;
	}
	//data2[count]=output;
	
	mem1b=mem1a;
	mem1a=v1;
	mem2b=mem2a;
	mem2a=v2;
	mem3b=mem3a;
	mem3a=v3;
	count++;
	
	
	
	if(count>=SAMPLE_LENGTH){
		count=0;
		//send_data_over_UART(data2, 1); //data2 stored freqMag values instead
		send_samples_over_UART_as_bytes(0);
		clearData();
	}	
	

	
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
	/*  ___ADCON1 Register___ */
	ADCON1bits.FORM = 0;		//unsigned integer output
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

void send_data_over_UART(int data[], byte dataType){
	int i=0;
	disableTimer3(); //no sense in sampling while we are transmitting
	if(dataType==0){ //send bytes
		for(i=0; i<SAMPLE_LENGTH; i++){			
			sendByte(data1[i]);
		}
	}else{//send text
		for(i=0; i<SAMPLE_LENGTH; i++){			
			sendNum(data[i]);
			if(i%10==0) sendString("\n\r");
		}
	}
	enableTimer3(); //turn sampling back on
}

void clearData(){
	int i;
	for(i=0;i<SAMPLE_LENGTH;i++){
		data1[i]=0; data2[i]=0; data3[i]=0;
	}
	mem1a=0; mem1b=0;
	mem2a=0; mem2b=0;
	mem3a=0; mem3b=0;
	
	found1=0;found2=0;found3=0;
	foundTime1=0;foundTime2=0;foundTime3=0;

}	