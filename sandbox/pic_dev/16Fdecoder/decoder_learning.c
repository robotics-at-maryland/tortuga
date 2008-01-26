/* Author: Scott Watson

---modified by Peter for a learning experiece---
for original functionalty please see the decoder_orig.c file


Description: This code accepts binary coded input on 4 pins
and uses it to determine which of the other pins to drive high

The functionality is as a BCD to Decimal decoder for a light display

*/

#include <pic.h>
 __CONFIG(FCMDIS & IESODIS & BORDIS & UNPROTECT & 
    MCLRDIS & PWRTEN & WDTDIS & INTIO);

main(){    
	//disabling the analog functions on all pins
	ANSEL = 0b00000000;
	ANSELH = 0b00000000;

    TRISA = 0b00000000;	//enable digital output on port A
	TRISB = 0b11110000; //port B enabled for digital input
	TRISC = 0b00000000; //enable digital output on port C

	PORTA=0;
	PORTC=0;

	//Main part of program begins here...
    while (1 == 1){

		//RA0=1;
		//RA1=1;
		//RA2=1;
		RC6=1;
		/*if(PORTB==0b00000000){ //0
			RA0=1;
		}
		if(PORTB==0b00010000){ //1
			RA1=1;
		}
		if(PORTB==0b00100000){ //2
			RA2=1;
		}
		if(PORTB==0b00110000){ //3
			RC7=1; //changed from RA3 because it can only be an input
		}
		if(PORTB==0b01000000){ //4
			RA4=1;
		}
		if(PORTB==0b01010000){ //5
			RA5=1;
		}
		if(PORTB==0b01100000){ //6
			RC0=1;
		}
		if(PORTB==0b01110000){ //7
			RC1=1;
		}
		if(PORTB==0b10000000){ //8
			RC2=1;
		}
		if(PORTB==0b10010000){ //9
			RC3=1;
		}
		if(PORTB==0b10100000){ //10
			RC4=1;
		}
		if(PORTB==0b10110000){ //11
			RC5=1;
		}
		if(PORTB==0b11000000){ //12
			RC6=1;
		}
/////////////////////////////////////////
		if(PORTB==0b11000000){ //13
			RC6=0;
		}
		if(PORTB==0b11000000){ //14
			RC6=0;
		}
		if(PORTB==0b11000000){ //15
			RC6=0;
		}
		PORTA=0;//turn off the lights
		PORTC=0;//turn off the lights*/
	
    }//elihw
}//end main


