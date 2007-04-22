#include "p30fxxxx.h"
/*Run this project using an external crystal
routed via the PLL in 16x multiplier mode
For the 7.3728 MHz crystal we will derive a
throughput of 7.3728e+6*16/4 = 29.4 MIPS(Fcy)
,~33.9 nanoseconds instruction cycle time(Tcy).
_FOSC(CSW_FSCM_OFF & XT_PLL16); //this one from the sample code as is the above comment  */

_FOSC(CSW_FSCM_OFF & XT_PLL16);

//For the dsPIC30F4012
//_FOSC(CSW_FSCM_OFF & XT_PLL8);
_FOSCSEL(PRIOSC_PLL);
_FWDT ( WDT_OFF );				//Turn off the Watch-Dog Timer.
_FGS(CODE_PROT_OFF);			//Disable Code Protection

/*//for the dsPIC30F1010, 2020, etc...
_FOSC(EC & HS); 
_FOSCSEL(PRIOSC_PLL);
_FWDT ( WDT_OFF );
_FGS(CODE_PROT_OFF);
*/

void initUart(void);
void ADC_Init(void);
void sendString(unsigned char * s);
void toggleinfinite(void);

int main (void){
		//long i=0, j=0, t=0, b=0;
    	initUart();
        //sendString("\n\rRunning...\n\n\r");
		//sendString("\n\rCircuit designed built and programmed by Scott and Steve\n\n\r");

		ADC_Init();             //Initialize the A/D converter

        while (1);              //Loop Endlessly - Execution is interrupt driven
                                //from this point on.
        return 0;
}


//This function toggles the port B pins infinitely (no stoppping!)
void toggleinfinite(){
	ADPCFG = 0xFFFF;
	TRISB = 0;
	LATB = 0xFFFF;

	long i=0;
	while(1){
		for(i=0; i<65000; i++);
		for(i=0; i<65000; i++);
		for(i=0; i<65000; i++);
		LATB ^= 0xFFFF;
	}
}





