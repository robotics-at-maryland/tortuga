#include "p30fxxxx.h"
#define byte unsigned char

unsigned int result[500];
unsigned int count = 0;
unsigned int ADResult1 = 0;
unsigned int ADResult2 = 0;

//Functions and Variables with Global Scope:
void ADC_(void);
void __attribute__((__interrupt__)) _ADCInterrupt(void);
void initUart(void);

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

//Functions:
//ADC_Init() is used to configure A/D to convert 16 samples of 1 input
//channel per interrupt. The A/D is set up for a sampling rate of 1MSPS
//Timer3 is used to provide sampling time delay.
//The input pin being acquired and converted is AN7.
void ADC_Init(void){
        //ADCON1 Register
        //Set up A/D for Automatic Sampling
        //Use internal counter (SAMC) to provide sampling time
        //Set up A/D conversrion results to be read in 1.15 fractional
        //number format.
        //Set up Sequential sampling for multiple S/H amplifiers
        //All other bits to their default state
        ADCON1bits.FORM = 0;
        ADCON1bits.SSRC = 7;
        ADCON1bits.ASAM = 1;
        ADCON1bits.SIMSAM = 0;

        //ADCON2 Register
        //Set up A/D for interrupting after 2 samples get filled in the buffer
        //Set up to sample on 2 S/H amplifiers - CH0 and CH1
        //All other bits to their default state
        ADCON2bits.SMPI = 1;
        ADCON2bits.CHPS = 1;
	//ADCON2bits.VCFG = 3; //Ideally use external references

        //ADCON3 Register
        //We would like to set up a sampling rate of 1 MSPS
        //Total Conversion Time= 1/Sampling Rate = 125 microseconds
        //At 29.4 MIPS, Tcy = 33.9 ns = Instruction Cycle Time
        //The A/D converter will take 12*Tad periods to convert each sample
        //So for ~1 MSPS we need to have Tad close to 83.3ns
        //Using equaion in the Family Reference Manual we have
        //ADCS = 2*Tad/Tcy - 1
        ADCON3bits.SAMC = 0;
        ADCON3bits.ADCS = 4;

        //ADCHS Register
        //Set up A/D Channel Select Register to convert AN3 on Mux A input
        //of CH0 and CH1 S/H amplifiers
        ADCHS = 0x0023;

        //ADCSSL Register
        //Channel Scanning is disabled. All bits left to their default state
        ADCSSL = 0x0000;

        //ADPCFG Register
        //Set up channels AN7 as analog input and configure rest as digital
        //Recall that we configured all A/D pins as digital when code execution
        //entered main() out of reset
        ADPCFG = 0xFFFF;
        ADPCFGbits.PCFG3 = 0;

        //Clear the A/D interrupt flag bit
        IFS0bits.ADIF = 0;

        //Set the A/D interrupt enable bit
        IEC0bits.ADIE = 1;

        //Turn on the A/D converter
        //This is typically done after configuring other registers
        ADCON1bits.ADON = 1;
}

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
    for(i=0; str[i]!=0; i++)
        sendByte(str[i]);
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
	ADResult1 = ADCBUF0;
	ADResult2 = ADCBUF1;
	
        //Clear the A/D Interrupt flag bit or else the CPU will
        //keep vectoring back to the ISR
        IFS0bits.ADIF = 0;
	
		result[count++] = ADResult1;
		result[count++] = ADResult2;

        if(count>=500){
				int i=0;
				byte temp;

				//sendString("\n\rData: ");
				for(i=0; i<500; i++){
					//sendNum(result[i]);
					temp = (byte)(result[i]>>2);
					sendByte(temp);
					//if(i%10==0){
					//	sendString("\n\r");
					//} 
				}
				count = 0; 
    		
		}
//original code from ADC interupt function for reference
/*	ADResult1 = ADCBUF0;
	ADResult2 = ADCBUF1;
	
        //Clear the A/D Interrupt flag bit or else the CPU will
        //keep vectoring back to the ISR
        IFS0bits.ADIF = 0;
	
		result[count++] = ADResult1;
		result[count++] = ADResult2;

        if(count>=500){
				int i=0;

				sendString("\n\rData: ");
				for(i=0; i<500; i++)
					sendNum(result[i]);
				count = 0; 
		}
*/
}

