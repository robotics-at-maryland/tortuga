#include "p30fxxxx.h"

#define byte unsigned char
#define sampleTotal 70  //can go up to 70

//for the dsPIC30F2020
_FOSC(EC & HS); 
_FOSCSEL(PRIOSC_PLL);
//_FWDT(WDT_OFF);  //why does this fail?
_FWDT(FWDTEN_OFF); //relevant fix?
_FGS(CODE_PROT_OFF);

void initUart(void);
void dsPIC30F2020ADC_Init();
void sendString(unsigned char str[]);
//void toggleinfinite(void);
void sendNum(unsigned int i);
void sendByte(byte i);
//void printADSTATbits();
//void printADCBUF();
void getSamples();
void delay(int mseconds);
int getBestTrigger();

int timerCounter=0;

int main(void){
	byte temp;
	int temp2;
	int max=0,max1,max2,max3,max4;
	int bestTrigger=0;

	initUart();
	dsPIC30F2020ADC_Init();

	sendString("\n\r\n\rrunning...\n\r");

	//getSamples();
	max1=getBestTrigger();
	delay(500);
	max2=getBestTrigger();
	delay(500);
	max3=getBestTrigger();
	delay(500);
	max4=getBestTrigger();
	if(max1>max){
		bestTrigger=max; max=max1;}
	if(max2>max){
		bestTrigger=max; max=max2;}
	if(max3>max){
		bestTrigger=max; max=max3;}
	if(max4>max){
		bestTrigger=max; max=max4;}
	bestTrigger+=9;

	sendString("Trigger Value set to ");
	sendNum(bestTrigger);
	sendString(" or ");
	sendNum(bestTrigger*4.88758);
	sendString(" in millivolts\n\r");


	TRISBbits.TRISB2=0; //enable pin 4 for digital output
	PORTBbits.RB2=0; // set pin 4 low

	temp=0;
	ADCPC1bits.SWTRG2 = 1;
	while(1){	
		while(ADCPC1bits.PEND2){}
		ADCPC1bits.SWTRG2 = 1;//set to 1 to start conversion of AN5 and AN4
		if(ADCBUF4>bestTrigger){ //42=200mV  90=440mV
			int saveValue = ADCBUF4;
			int maxValue = ADCBUF4;
			byte keepGoing = 1;
			
			//trigger activated
			PORTBbits.RB2=1; // set pin 4 high
			
			while(keepGoing){
			while(ADCPC1bits.PEND2){}
			ADCPC1bits.SWTRG2 = 1;//set to 1 to start conversion of AN5 and AN4
				if(ADCBUF4>maxValue){
					 maxValue=ADCBUF4;
				}else{
					keepGoing=0;
				}
			}
			//if(maxValue==saveValue)break;//probably noise

			sendNum(timerCounter);
			sendString(" triggered value: ");sendNum(saveValue);sendString("\n\r");
			sendString(" max value: ");sendNum(maxValue);sendString("\n\r");
			sendString(" TRIGGER: ");sendNum(bestTrigger);sendString("\n\r");
			delay(500);
			sendNum(timerCounter++);
			sendString("\n\r\n\r");

			ADCPC1bits.SWTRG2 = 1;//set to 1 to start conversion of AN5 and AN4
			PORTBbits.RB2=0; // set pin 4 low
		}
		
	}
	
	while(1);
}

void delay(int mseconds){ //cannot be called with more than about 550ms
	const int onemillisecond = 0b000000001110101;
	mseconds=mseconds*onemillisecond;
	
	T1CONbits.TON=0;
	T1CONbits.TCKPS=0b11;
	TMR1=0;
	T1CONbits.TON=1;
	while(TMR1<mseconds){}
	T1CONbits.TON=0;

}


int getBestTrigger(){
	byte temp;
	//int temp2;
	int max=0;
	int samples[sampleTotal];

	temp=0;
	ADCPC1bits.SWTRG2 = 1;
	while(temp<sampleTotal){	
		//Nop();Nop();Nop();//Nop();
		//conversion pending if 1... conversion complete if 0
		while(ADCPC1bits.PEND2==1){}
		ADCPC1bits.SWTRG2 = 1;//start conversion of AN5 and AN4
		samples[temp++]=ADCBUF4;
	}

	/*sendString("print result...\n\r");
	for(temp=0;temp<sampleTotal;temp++){
		//sendByte(samples[temp]>>2);

		sendNum(samples[temp]);
		sendString(" ");
		if(temp==9||temp==19||temp==29||temp==39||temp==49||temp==59||temp==69||temp==79||temp==89){
			sendString("\n\r");
		}
	}*/

	for(temp=0;temp<sampleTotal;temp++){
		if(samples[temp]>max) max=samples[temp];
	}
	sendString("Max value ");
	sendNum(max);
	sendString("\n\r");

	return max;
}

/*void getSamples(){
	byte temp;
	int temp2;
	int samples[10];

	temp=0;
	ADCPC1bits.SWTRG2 = 1;
	while(temp<10){	
		
		//Nop();Nop();Nop();//Nop();
		//conversion pending if 1... conversion complete if 0
		while(ADCPC1bits.PEND2==1){}
		
		ADCPC1bits.SWTRG2 = 1;//set to 1 to start conversion of AN5 and AN4

		//ADSTAT = 0; //I think I might need this, but it doesn't seem to matter
		//ADCPC1bits.SWTRG2 = 0; //don't think I need this

		samples[temp++]=ADCBUF4;
		
	}

	sendString("print result...\n\r");
	
	for(temp=0;temp<10;temp++){
		//sendByte(samples[temp]>>2);

		sendNum(samples[temp]);
		sendString(" ");
		if(temp==9||temp==19||temp==29||temp==39||temp==49||temp==59||temp==69||temp==79||temp==89){
			sendString("\n\r");
		}
	}
	//sendByte(0);

	while(1);
}*/


//////////////////////////////////////////////
//This function initializes the ADC for a dsPIC30F2020
void dsPIC30F2020ADC_Init(){
	// ADCON: ADC Control Register 1
	ADCONbits.ADCS      = 3;        // Clock Divider is set up for Fadc/10
	ADCONbits.SEQSAMP   = 1;        // Sequential Sampling disabled 
	ADCONbits.ORDER     = 0;        // Even channel first 
	ADCONbits.EIE       = 0;        // No Early Interrupt
	ADCONbits.FORM      = 0;        // Output in Integer Format   
 	ADCONbits.GSWTRG 	= 0;		// global software trigger bit
	ADCONbits.ADSIDL    = 0;        // Operate in Idle Mode 
	//ADCONbits.ADON = not needed to be set here
	
	//ADPCFG: ADC Port Configuration Register
	//ADPCFGbits.PCFG0 = 0;
	//thru
	//ADPCFGbits.PCFG7 = 0;
	ADPCFG = 0b1111111111001111;    // AN4 and AN5 are analog inputs 

	// ADSTAT: ADSTAT  Register 
	//ADSTATbits.P0RDY = 0;
	//thru
	//ADSTATbits.P3RDY = 0;
	// bit set with data is ready in buffer... cleared when a zero is written to this bit
	ADSTAT = 0;     // Clear the ADSTAT register 
	
	ADCPC0bits.TRGSRC0 = 0;	//AN3 and AN2
	ADCPC0bits.SWTRG0 = 0;	//AN3 and AN2
	ADCPC0bits.PEND0 = 0;	//AN3 and AN2
	ADCPC0bits.IRQEN0 = 0; 	//AN3 and AN2
	ADCPC0bits.TRGSRC1 = 0;	//AN1 and AN0
	ADCPC0bits.SWTRG1 = 0;	//AN1 and AN0
	ADCPC0bits.PEND1 = 0;	//AN1 and AN0
	ADCPC0bits.IRQEN1 = 0;	//AN1 and AN0
	
	ADCPC1bits.TRGSRC2 = 1;	// Individual software trigger
	ADCPC1bits.SWTRG2 = 0;  //set to 1 to start conversion of AN5 and AN4
	ADCPC1bits.PEND2 = 0;  	//conversion pending if 1... conversion complete if 0
	ADCPC1bits.IRQEN2 = 0; 	// disable the interrupt
	ADCPC1bits.TRGSRC3 = 0; //AN7 and AN6
	ADCPC1bits.SWTRG3 = 0;	//AN7 and AN6
	ADCPC1bits.PEND3 = 0;	//AN7 and AN6
	ADCPC1bits.IRQEN3 = 0;	//AN7 and AN6

	ADCONbits.ADON = 1; //Start the ADC module
}


//////////////////////////////////////////////
//Use this function to send 1 byte to the UART
void sendByte(byte i){
	while(U1STAbits.UTXBF);
    U1TXREG = i;
    while(U1STAbits.UTXBF);
}

//////////////////////////////////////////////
//Use this function to send literal strings in quotes as
//ASCII bytes to the UART
void sendString(unsigned char str[]){
    byte i=0;
    for(i=0; str[i]!=0; i++){
        sendByte(str[i]);
	}
}

//////////////////////////////////////////////
//Use this function to send an unsigned integer to the UART
//as ASCII text
void sendNum(unsigned int i){
	unsigned char tmp[10];
	sprintf(tmp, "%u ", i);
	sendString(tmp);
}

//////////////////////////////////////////////
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


/*void printADSTATbits(){
	sendString("ADSTATbits...\n\r");
	sendNum(ADSTATbits.P0RDY);sendString(" ");
	sendNum(ADSTATbits.P1RDY);sendString(" ");
	sendNum(ADSTATbits.P2RDY);sendString(" ");
	sendNum(ADSTATbits.P3RDY);sendString("\n\r");
}

void printADCBUF(){
	sendString("0=");sendNum(ADCBUF0);sendString("\t");
	sendString("1=");sendNum(ADCBUF1);sendString("\t");
	sendString("2=");sendNum(ADCBUF2);sendString("\t");
	sendString("3=");sendNum(ADCBUF3);sendString("\t");
	sendString("4=");sendNum(ADCBUF4);sendString("\t");
	sendString("5=");sendNum(ADCBUF5);sendString("\t");
	sendString("6=");sendNum(ADCBUF6);sendString("\t");
	sendString("7=");sendNum(ADCBUF7);sendString("\n\r");
}*/


//////////////////////////////////////////////
//This function toggles the port B pins infinitely (no stoppping!)
// used to measure the MIPS
/*void toggleinfinite(){ 
	ADPCFG = 0xFFFF;
	TRISB = 0;
	LATB = 0xFFFF;

	long i=0;
	while(1){
		LATB ^= 0xFFFF;
	}
}*/


