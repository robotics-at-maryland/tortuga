/* Author: Scott Watson

Description: This code is meant to toggle an output pin on a 
PIC16F690 for the purpose of generating tones for the passive
sonar system to detect.

*/

#include <pic.h>
 __CONFIG(FCMDIS & IESODIS & BORDIS & UNPROTECT & 
    MCLRDIS & PWRTEN & WDTDIS & INTIO);

main(){
	//disabling the analog functions on all pins
	ANSEL = 0;
	ANSELH = 0;

	//enable digital output on all pins
    TRISC = 0; 
	TRISB=0; 
	TRISA=0;

	PORTC = 0; //turn off all of port C
	PORTB=0;
	PORTA=0; //and all the rest

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

/* how long we need to wait to get different frequencies

	3000^-1 / 4000000^-1 = 1333.3 
	6000^-1 / 4000000^-1 = 666.7
	20000^-1 / 4000000^-1 = 200
*/

    while (1 == 1){
		TMR0=0;
		while(TMR0<100){
			//do nothing
		}
		RB6=RB6^1;
    }//elihw

}//end main

