/*SONAR code for AUV2008 to be run on the dsPIC33FJ256GP710 board design*/

#include "p33fxxxx.h"
#define byte unsigned char
#define UART_ENABLED 1  //set to 0 to turn off uart

//Configuration settings for the dsPIC33FJ256GP710
//...to get 40MIPS with 7.5MHz crystal
_FOSC(FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMD_HS); //Clock switching and clock monitor disabled. OSC2 is clock. High Speed selected
_FOSCSEL(FNOSC_PRIPLL);	//primary oscilator with PLL
_FWDT(FWDTEN_OFF & WINDIS_OFF);	// watchdog timer and window are off
_FGS(GSS_OFF);  //code and write protection off
_FPOR(FPWRT_PWR128);  //longest power on reset available... fast startup not critical
_FICD( BKBUG_OFF & COE_OFF & JTAGEN_OFF & ICS_PGD2 );  //everything turned off as best I can tell, programming set to channel 2

void init_IO(void);
void init_OC2(void);
void init_Uart(void);
void init_Timer2and3(void);
void sendByte(byte i);
void sendString( char * s);
void sendNum(unsigned int i);
void delay(int microseconds);
void startup_flashes_text(void);
void _ISRFAST _T3Interrupt(void);

int dutyCycle = 937;

#define M_SLIDING_DFT_k 10
#define M_SLIDING_DFT_N 100
#define M_SLIDING_DFT_nchannels 4

#include "dft_singleton_c30.h"


int main(){
	//	Initialize the DFT.
	sliding_dft_t * dft = M_DFT_INIT();
	adcdata_t zeroMatrix[M_SLIDING_DFT_nchannels];
	
	int i;

	//	Set up a vector of zeros; this is the simplest test
	//  of the DFT -- pump it with zeros
	for (i = 0; i < M_SLIDING_DFT_nchannels; i++){
		zeroMatrix[i] = 0;
	}
	
	//	Infinite loop -- just pass zeros over and over again.
	//	Nothing interesting will happen, but at least we can
	//	see that the code is working if the amplitude stays
	//	zero always.
	while(1){
		//	Replace zeroMatrix with an array of adcdata_t's 
		//	(or int8_t's, or signed char's if you prefer)
		//	from the most recent data collected on all channels
		dft_update(dft, zeroMatrix);
	}
}


void _ISRFAST _T3Interrupt(void){
//	OC2RS=dutyCycle;
	IFS0bits.T3IF=0;
}

void init_OC2(void){
	/*  OUTPUT COMPARE CONFIGURATION CALCULATIONS
	instruction clock cycle time.... 40MIPS... 25ns
	using timer prescaler of 1:64 so 1.6us per TMR3 incriment
	1.6us * 2^16 = 104.8576ms maximum period
	1ms = 625 timer ticks
	1.5ms = 937.5 timer ticks
	2ms = 1250 timer ticks
	25ms = 15,625 timer ticks
	
	with a prescaler of 1:256 each tick is 6.4us and the maximum period is 419.4ms
	*/	
	OC2CONbits.OCM = 0b000; //keep OC2 off while mod's are made
	OC2CONbits.OCSIDL = 0; //0=continued operation in idle mode
	OC2CONbits.OCTSEL = 1; //1=timer3  0=timer2
	OC2R=dutyCycle;
	OC2RS=dutyCycle;
	OC2CONbits.OCM = 0b110; //initialize OC2 low, generate continuous output pulses
}	

void startup_flashes_text(void){
	PORTBbits.RB2=1;
	delay(200);
	PORTBbits.RB2=0;
	delay(200);  
	PORTBbits.RB2=1;
	delay(200);  
	PORTBbits.RB2=0;
	delay(200);  
	PORTBbits.RB2=1;
	
	sendString("\n\rRunning...\n\r\n\r");
	sendString("Starting at duty cycle = ");
	sendNum(dutyCycle);
	sendString("\n\r");
}	

void init_Timer2and3(void){
	//setup TMR2 and TMR3... in this project, TMR3 is used to do continuous PWM on 
	T2CONbits.T32=0; //turn off 32 bit operation
	T2CONbits.TCKPS=0b11;  // 1:256 prescale value
	T2CONbits.TON=1;
	
	T3CONbits.TCKPS=0b10; // 1:64 prescale value
	T3CONbits.TSIDL=0; //continue in idle mode
	T3CONbits.TCS=0; //internal clock  (F_OSC/4)
	T3CONbits.TON=1;
	PR3=11845; //25ms period for 40Hz operation
	IEC0bits.T3IE=1;  //enable interupts for Timer3
	IPC2bits.T3IP=6;  //interupt priority0-7 where 7 is the highest priority
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

//////////////////////////////////////////////
//Use this function to send 1 byte to the UART
void sendByte(byte i){
	if(!UART_ENABLED)return;
	while(U1STAbits.UTXBF);
    U1TXREG = i;
    while(U1STAbits.UTXBF);
}
//////////////////////////////////////////////
//Use this function to send literal strings in quotes as
//ASCII bytes to the UART
void sendString( char str[]){
	if(!UART_ENABLED)return;
    byte i=0;
    for(i=0; str[i]!=0; i++){
        sendByte(str[i]);
	}
}
//////////////////////////////////////////////
//Use this function to send an unsigned integer to the UART
//as ASCII text
void sendNum(unsigned int i){
	if(!UART_ENABLED)return;
	char tmp[10];
	sprintf(tmp, "%u ", i);
	sendString(tmp);
}

//////////////////////////////////////////////
/* UART BRG "Baud" rate calculation "How To"
Baud Rate is the same as Bits Per Second
	U1BRG = (MIPS / BAUD / 16) - 1
EXAMPLE...
7.5MHz Oscillator with pll of 8
MIPS = 7.5MMhz * pll8 / 4 = 15,000,000
15,000,000/BAUD(115200) = 130.208...
130.208... / 16 - 1 = 7.138... ~~~ 7
****don't forget to invert the signal coming out of the PIC
to create a signal that most computer's serial ports will 
interpret  */
void init_Uart(void){
	if(!UART_ENABLED)return;
    U1MODE = 0x0000;
    // Calculations for differnt Baud rates...
    // 25 for baud of 38400 
    // 7  for baud of 230400 pll8
    // 15 for baud of 115200
    U1BRG = 15;
    //U1MODEbits.ALTIO = 1;   // Use alternate IO
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;   // Enable transmit
}

void init_IO(void){
	//setup master timing inputs and outputs
	ADPCFG=0b1111111111111111;//turn off all analog stuff
	TRISBbits.TRISB0=1; //left button
	TRISBbits.TRISB1=1;	//right button
	TRISBbits.TRISB2=0;	//status LED
	PORTBbits.RB2=0;
	TRISBbits.TRISB3=0; //status LED
	PORTBbits.RB3=0;
}
