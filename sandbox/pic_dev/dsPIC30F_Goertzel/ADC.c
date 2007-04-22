#include "p30fxxxx.h"
#define byte unsigned char
#define SAMPLE_LENGTH 1900
#define TRIGGER_POSITION 1500	//0-(SAMPLE_LENGTH-1)
#define TRIGGER_VALUE 700		//0-1023 in Volts

byte result[SAMPLE_LENGTH];
unsigned int count = 0;
byte trigger_status = 2;
unsigned int trigger_count = 0;


//Functions and Variables with Global Scope:
void ADC_Init(void);
void __attribute__((__interrupt__)) _ADCInterrupt(void);
void initUart(void);
void send_samples_over_UART_as_bytes(int start);
void send_samples_over_UART_as_ASCII(void);

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
	ADCON1bits.FORM = 0;	//
	ADCON1bits.SSRC = 7;	//enable auto convert mode
	ADCON1bits.ASAM = 1;	//enable auto sampling
	ADCON1bits.SIMSAM = 0;	//enable sequential sampling

/*  ___ADCON2 Register___
Set up A/D for interrupting after 2 samples get filled in the buffer
Set up to sample on 2 S/H amplifiers - CH0 and CH1
All other bits to their default state  */
	ADCON2bits.SMPI = 1;	//ensure samples per interrupt is greater than 1
	ADCON2bits.CHPS = 1;	//selects CH0 and CH1
	//ADCON2bits.VCFG = 3;	//Ideally use external references

/*___ADCON3 Register___
We would like to set up a sampling rate of 1 MSPS
Total Conversion Time= 1/Sampling Rate = 125 microseconds
At 29.4 MIPS, Tcy = 33.9 ns = Instruction Cycle Time
The A/D converter will take 12*Tad periods to convert each sample
So for ~1 MSPS we need to have Tad close to 83.3ns
Using equaion in the Family Reference Manual we have
	ADCS = 2*Tad/Tcy - 1  		*/

//SAMC needs updating to 0b'00001' or ob'00010'
	ADCON3bits.SAMC = 2;	//minimum sampling time  //was 0;
	ADCON3bits.ADCS = 9;	//minimum Tad time   //was 4;

/*___ADCHS Register___
Set up A/D Channel Select Register to convert AN3 on Mux A input
of CH0 and CH1 S/H amplifiers		*/
	ADCHS = 0x0023;		//sets analog input pins to the channels

/*___ADCSSL Register___
Channel Scanning is disabled. All bits left to their default state*/
	ADCSSL = 0x0000;

/*___ADPCFG Register___
Set up channels AN7 as analog input and configure rest as digital
Recall that we configured all A/D pins as digital when code execution
entered main() out of reset  */
	ADPCFG = 0xFFFF;
	ADPCFGbits.PCFG3 = 0;

	IFS0bits.ADIF = 0;	//Clear the A/D interrupt flag bit
	IEC0bits.ADIE = 1;	//Set the A/D interrupt enable bit

//Turn on A/D converter - typically done after configuring other registers
	ADCON1bits.ADON = 1;
}

//////////////////////////////////////////////
//Use this function to send 1 byte to the UART
void sendByte(byte i){
  //  U1STAbits.OERR = 0;
  //  U1STAbits.FERR = 0;
  //  U1STAbits.PERR = 0;
  //  U1STAbits.URXDA = 0;
    while(U1STAbits.UTXBF);
    U1TXREG = i;
    while(U1STAbits.UTXBF);
}

//Use this function to send literal strings in quotes as
//ASCII bytes to the UART
void sendString(unsigned char str[]){
    byte i=0;
    for(i=0; str[i]!=0; i++){
        sendByte(str[i]);
	}
}

//Use this function to send an unsigned integer to the UART
//as ASCII text
void sendNum(unsigned int i){
	unsigned char tmp[10];
	sprintf(tmp, "%u ", i);
	sendString(tmp);
}

//_ADCInterrupt() is the A/D interrupt service routine (ISR).
//The routine must have global scope in order to be an ISR.
//The ISR name is chosen from the device linker script.
void __attribute__((__interrupt__)) _ADCInterrupt(void){
	unsigned int temp = 0;
	//Clear the A/D Interrupt flag bit or else the CPU will
	//keep vectoring back to the ISR
	IFS0bits.ADIF = 0;

	temp = ADCBUF0;
	result[count++] = (byte)(ADCBUF0>>2);
	result[count++] = (byte)(ADCBUF1>>2);
	
//frequency watch mode
	//signal seen going above trigger
	/*if(TRIGGER_VALUE<ADCBUF0 || TRIGGER_VALUE<ADCBUF1){
		if(trigger_status==3){
			trigger_status=4;
		}else if(trigger_status==5){ //period found!
			if(trigger_count>255){
				sendByte(255);//just send max value for now
			}else{
				sendByte((byte)trigger_count);
			}
			count=0;	//reset period counter
			trigger_status=2;	//look for next period
		}
	}
	//signal seen going below trigger
	if(TRIGGER_VALUE>ADCBUF0 || TRIGGER_VALUE>ADCBUF1){
		if(trigger_status==2){
			count=0;
			trigger_status=3;
		}else if(trigger_status==4){
			trigger_status=5;
		}
	}
	if(count>=SAMPLE_LENGTH){
		//no period found, reset buffer and status
		count=0;
		trigger_status=2;
	}*/

//trigger mode
	//if trigger tripped
	if(trigger_status == 1) trigger_count+=2;
	//if prog just started then fill buffer min amount
	if(trigger_status == 2){
		trigger_count+=2;
		//fill buffer some minimum
		if(trigger_count>(SAMPLE_LENGTH-TRIGGER_POSITION)){
			trigger_status=0;//tiggering re-enabled
			trigger_count=0;
		}
	}
	if(ADCBUF0 > TRIGGER_VALUE && trigger_status!=2) trigger_status = 1; 
	if(trigger_count>TRIGGER_POSITION){
		send_samples_over_UART_as_bytes(count);
		trigger_count=0;
		trigger_status=0;//watch for trigger again
	}
	if(count>=SAMPLE_LENGTH)count=0;

//continuous sample and display mode
	/*if(count>=SAMPLE_LENGTH){
		send_samples_over_UART_as_bytes(0)
		send_samples_over_UART_as_ASCII()
		count = 0; 
	}*/
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
