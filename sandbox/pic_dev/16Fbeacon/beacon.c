/* Author: Scott Watson

Description: 

*/

#include <pic.h>
 __CONFIG(FCMDIS & IESODIS & BORDIS & UNPROTECT & 
    MCLRDIS & PWRTEN & WDTDIS & INTIO);


void delay1();


unsigned int delayNum; //delay loop


main(){
    
	//disabling the analog functions on port C pins
	ANSEL = 0x7; //0b00000111
	ANSELH = 0xc; //0b00001100

	//enable digital output on port C
    TRISC = 0; //0b00000000

	//enable port A pins 0,1,2 for analog input
	TRISA = 0b00000111;

	PORTC = 0; //turn off all of port C

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

	//Main part of program begins here...
/* 3000^-1 / 4000000^-1 = 1333.3 
   6000^-1 / 4000000^-1 = 666.7
   20000^-1 / 4000000^-1 = 200

*/
    while (1 == 1){
		TMR0=0;
		while(TMR0<166){

		}
		RC0=RC0^1;
    }//elihw
}//end main

void delay1(){ //function actually wastes cycles
// 750,000 instructions will take 0.75 seconds at 4MHz
	for(delayNum=0; delayNum<65535; delayNum++); //655ms
	//for(delayNum=0; delayNum<34200; delayNum++); add for almost exactly one second
}
