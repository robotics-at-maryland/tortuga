
#include "p30fxxxx.h"

//Macros for Configuration Fuse Registers:
//Invoke macros to set up  device configuration fuse registers.
//The fuses will select the oscillator source, power-up timers, watch-dog
//timers, BOR characteristics etc. The macros are defined within the device
//header files. The configuration fuse registers reside in Flash memory.
//_FOSC(CSW_FSCM_OFF & XT_PLL16);  //Run this project using an external crystal
                                //routed via the PLL in 16x multiplier mode
                                //For the 7.3728 MHz crystal we will derive a
                                //throughput of 7.3728e+6*16/4 = 29.4 MIPS(Fcy)
                                //,~33.9 nanoseconds instruction cycle time(Tcy).
//_FWDT(WDT_OFF);                 //Turn off the Watch-Dog Timer.
//_FBORPOR(MCLR_EN & PWRT_OFF);   //Enable MCLR reset pin and turn off the
                                //power-up timers.
_FGS(CODE_PROT_OFF);            //Disable Code Protection

//from our other code
//_FOSC(EC & HS);          
_FOSCSEL(PRIOSC_PLL);
_FWDT ( WDT_OFF );

//for the dsPIC30F4012
_FOSC(CSW_FSCM_OFF & XT_PLL8);


void sendString(unsigned char * s);

int main (void)
{
		long i=0, j=0, t=0, b=0;
    	initUart();
        sendString("\n\rRunning...\n\n\r");
		sendString("\n\rCircuit designed built and programmed by Scott and Steve\n\n\r");

		//Delay so that start up message can be observed
		for(i=0;i<100000;i++);for(i=0;i<100000;i++);
		for(i=0;i<100000;i++);for(i=0;i<100000;i++);
		for(i=0;i<100000;i++);for(i=0;i<100000;i++);
		for(i=0;i<100000;i++);for(i=0;i<100000;i++);
		for(i=0;i<100000;i++);for(i=0;i<100000;i++);
		i=0;

		ADC_Init();             //Initialize the A/D converter

        while (1);              //Loop Endlessly - Execution is interrupt driven
                                //from this point on.
        return 0;
}






