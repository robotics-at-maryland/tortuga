/* Author: Scott Watson

Description: This is a redo of the 3 direction PIC16F690
passive sonar application in c code with the PICC Lite compiler

update... it now supports 6 directions on the breadboard display

update... it now supports 6 directions on the 13 LED display

1-27-07 update... it now supports 12 directions with the 13 LED display
*/

#include <pic.h>
 __CONFIG(FCMDIS & IESODIS & BORDIS & UNPROTECT & 
    MCLRDIS & PWRTEN & WDTDIS & INTIO);
/* The last word represents clock setting.... options are:
RCCLK - RC Clock, RA5 Clockout
RCIO - RC Clock, RA5 IO pin
INTCLK - Internal Oscillator, RA5 clockout
INTIO - Internal Oscillator, RA4 IO pin
EC - External Clock on RA5, RA4 IO pin
HS - High Speed (4-20MHz) Crystal on RA4 and RA5
XT - Nominal speed (1-4Mhz) Crystal on RA5 and RA5
LP - Low speed (32kHz-1MHz) Crystal on RA4 and RA5
*/

void delay1();
void checkT0T4();
void checkT2T4();
void checkT0T2();
bit sampleT0();
bit sampleT2();
bit sampleT4();

unsigned int delayNum; //delay loop
unsigned char thresholdVal0;
unsigned char thresholdVal2;
unsigned char thresholdVal4;
unsigned char td02;
unsigned char td04;
unsigned char td24;
unsigned char midThresh;

main(){
    //CMCON0 = 7; //turns off comparator, but register is not same in pic16f690
    
	//disabling the analog functions on port C pins
	ANSEL = 0x7; //0b00000111
	ANSELH = 0xc; //0b00001100

	//enable digital output on port C
    TRISC = 0; //0b00000000

	//enable port A pins 0,1,2 for analog input
	TRISA = 0b00000111;
	//setup ADC
	//ADCON0: left justified - Vdd - channel 0(0000,an0) - not(go) - enabled
	//ADCON0: ADFM VCFG CHS3 CHS2 CHS1 CHS0 GO/DONE ADON
	ADCON0 = 0b00000001;
	ADCON1 = 0b00010000; // Fosc/8

	PORTC = 0; //turn off all of port C

// 924 = 1110 0111 00
// 4.43/4.9 <-- stable level out of audio amp
// the pic chip is powered at 4.9V
// each transducer/amplifer settles at slightly different values
	thresholdVal0 = 0b11100101; //(calm:4.27) 4.39V
	thresholdVal2 = 0b11101100; //(calm:4.4V) 4.52V
	thresholdVal4 = 0b11101010; //(calm:4.37) 4.49V

// Setting up the timer!
// configure Timer0.  Sourced from the Processor clock
//		X X T0CS T0SE PSA PS2 PS1 PS0
// LEGEND:
//	X: 	Don’t cares – not Timer0 related.
//	T0CS: 	Timer0 Clock Source 0 for Instruction Clock.
//	T0SE: 	Timer0 Source Edge – Don’t care when connected to instruction clock.
//	PSA: 	Prescaler Assignment 0 assigns to Timer0.
//	PS: 	Prescaler rate select ‘111’ – full prescaler, divide by 256.
	OPTION = 0b00000010; //prescaler set to 8
	//INTCON = 0b00000000; 
	T0IE = 0; //T0 interupt enable //disable timer 0 interupt
	T0IF = 0; //T0 interupt flag //clear pending interupts and ready timer for first run

	//set threshold for in between values
	midThresh=48; //50; //update comment needed //at PSA=8 and 4MHz clock this is 400us

	//Main part of program begins here...
    while (1 == 1){
/* max distance between tranducers... 35cm or 0.35m
speed of sound in air...  343 m/s

time to travel this distance... 1020.4us

with prescaler set to 4 and a clock of 4MHz this means that each
timer incriment will be equivelent to 4us
***************************************************
*/

		//time delays written back to td02,td04,td24
		if(sampleT0()){
			T0IF = 0;
			TMR0 = 0;
			checkT2T4();
			if(td02<midThresh && td04<midThresh && td24<midThresh){
				// this means that the sound originated from above or below
				PORTC=0b00001100;
			}else if(td02<midThresh){
				PORTC = 0b00000010; //RC1=1;
			}else if(td04<midThresh){
				PORTC = 0b00001010; //RC5=1;
			}else if(td04<td02){
				PORTC = 0b00001011; 
			}else if(td04>td02){
				PORTC = 0b00000001; 
			}else{
				PORTC = 0b00000000; //RC0=1;
			}
			delay1();//in order to hold the display result for a time
			PORTC=0;//turn off the lights
		}
		if(sampleT2()){
			T0IF = 0;
			TMR0 = 0;
			checkT0T4();
			if(td02<midThresh && td04<midThresh && td24<midThresh){
				// this means that the sound originated from above or below
				PORTC=0b00001100;
			}else if(td02<midThresh){
				PORTC = 0b00000010; //RC1=1;
			}else if(td24<midThresh){
				PORTC = 0b00000110; //RC3=1;
			}else if(td24<td02){
				PORTC = 0b00000011; 
			}else if(td24>td02){
				PORTC = 0b00000101; 
			}else{
				PORTC = 0b00000100; //RC2=1;
			}
			delay1();
			PORTC=0;//turn off the lights
		}
		if(sampleT4()){
			T0IF = 0;
			TMR0 = 0;
			checkT0T2();
			if(td02<midThresh && td04<midThresh && td24<midThresh){
				// this means that the sound originated from above or below
				PORTC=0b00001100;
			}else if(td04<midThresh){
				PORTC = 0b00001010; //RC5=1;
			}else if(td24<midThresh){
				PORTC = 0b00000110; //RC3=1;
			}else if(td24<td04){
				PORTC = 0b00000111; 
			}else if(td24>td04){
				PORTC = 0b00001001; 
			}else{
				PORTC = 0b00001000; //RC4=1;
			}
			delay1();
			PORTC=00001111;//turn off the lights
		}
    }//elihw
}//end main

//CHECKING TRASDUCERS 2 and 4
void checkT2T4(){
	while(!T0IF){
		if(sampleT2() && !T0IF){
			td02=TMR0;
			//wait for T4 to catch the signal now
			while(!T0IF){
				if(sampleT4()){
					td04=TMR0;
					td24=td04-td02;
					return;
				}
			}
		}
		if(sampleT4() && !T0IF){
			td04=TMR0;
			//wait for T2 to catch the signal now
			while(!T0IF){
				if(sampleT2()){
					td02=TMR0;
					td24=td02-td04;
					return;
				}
			}
		}
	}
	PORTC=0b00111111;//signifies that the timer ran out and the signal wasn't caught on all the transducers
}//end checkT2T4

//CHECKING TRASDUCERS 0 and 4
void checkT0T4(){
	while(!T0IF){
		if(sampleT0() && !T0IF){
			td02=TMR0;
			while(!T0IF){
				if(sampleT4()){
					td24=TMR0;
					td04=td24-td02;
					return;
				}
			}
		}
		if(sampleT4() && !T0IF){
			td24=TMR0;
			while(!T0IF){
				if(sampleT2()){
					td02=TMR0;
					td04=td02-td24;
					return;
				}
			}
		}
	}
	PORTC=0b00111111;//signifies that the timer ran out and the signal wasn't caught on all the transducers
}//end checkT0T4

//CHECKING TRASDUCERS 0 and 2
void checkT0T2(){
	while(!T0IF){
		if(sampleT0() && !T0IF){
			td04=TMR0;
			while(!T0IF){
				if(sampleT2()){
					td24=TMR0;
					td02=td24-td04;
					return;
				}
			}
		}
		if(sampleT2() && !T0IF){
			td24=TMR0;
			while(!T0IF){
				if(sampleT0()){
					td04=TMR0;
					td02=td04-td24;
					return;
				}
			}
		}
	}
	PORTC=0b00111111;//signifies that the timer ran out and the signal wasn't caught on all the transducers
}//end checkT0T2


//SAMPLING CODE SAMPLING CODE SAMPLING CODE


//SAMPLE TRANSDUCER 0
bit sampleT0(){
////////////////////////////////////////////////////////////
//Start Sampling Transducer 0
////////////////////////////////////////////////////////////
	//ADCON0: ADFM VCFG CHS3 CHS2 CHS1 CHS0 GO/DONE ADON
    ADCON0 &= 0b11000011; //select channel 0
	ADCON0 |= 0b00000000;

	NOP();	// wait 5us for A2D amp to settle and capacitor to charge.
	NOP();	// wait 4us
	NOP();	// wait 3us
	NOP();	// wait 2us
	NOP();	// wait 1us
	GODONE = 1; //start conversion
	while(GODONE){}; //wait for conversion to complete
	if(ADRESH<thresholdVal0){
		// no signal detected
		return 0;
	}else{
		//signal detected
		return 1;
	}//fi
}

//SAMPLE TRANSDUCER 2
bit sampleT2(){
////////////////////////////////////////////////////////////
//Start Sampling Transducer 1
////////////////////////////////////////////////////////////
	//ADCON0: ADFM VCFG CHS3 CHS2 CHS1 CHS0 GO/DONE ADON
    ADCON0 &= 0b11000011; //select channel 1
	ADCON0 |= 0b00000100;

	NOP();	// wait 5us for A2D amp to settle and capacitor to charge.
	NOP();	// wait 4us
	NOP();	// wait 3us
	NOP();	// wait 2us
	NOP();	// wait 1us
	GODONE = 1; //start conversion
	while(GODONE){}; //wait for conversion to complete
	if(ADRESH<thresholdVal2){
		// no signal detected
		return 0;
	}else{
		//signal detected
		return 1;
	}//fi

}

//SAMPLE TRANSDUCER 4
bit sampleT4(){
////////////////////////////////////////////////////////////
//Start Sampling Transducer 4
////////////////////////////////////////////////////////////
	//ADCON0: ADFM VCFG CHS3 CHS2 CHS1 CHS0 GO/DONE ADON
    ADCON0 &= 0b11000011; //select channel 2
	ADCON0 |= 0b00001000;

	NOP();	// wait 5us for A2D amp to settle and capacitor to charge.
	NOP();	// wait 4us
	NOP();	// wait 3us
	NOP();	// wait 2us
	NOP();	// wait 1us
	GODONE = 1; //start conversion
	while(GODONE){}; //wait for conversion to complete
	if(ADRESH<thresholdVal4){
		// no signal detected
		return 0;
	}else{
		//signal detected
		return 1;
	}//fi
}

void delay1(){ //function actually wastes cycles
// 750,000 instructions will take 0.75 seconds at 4MHz
	for(delayNum=0; delayNum<65535; delayNum++); //655ms
	//for(delayNum=0; delayNum<34200; delayNum++); add for almost exactly one second
}
