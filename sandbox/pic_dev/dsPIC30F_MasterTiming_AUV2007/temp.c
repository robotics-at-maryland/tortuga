//source
//http://forum.microchip.com/tm.aspx?m=261838&mpage=1&key=

#include "p30fxxxx.h"

// Configuration Bit Settings 
//_FOSCSEL(FRC_PLL)
//_FOSC(CSW_FSCM_OFF & FRC_HI_RANGE & OSC2_CLKO)


#define    TIMER_PERIOD 0x0064         // Set the timer period for 3.43 usec 

int main2(void){
	// Set up the ADC Module 

	ADCONbits.ADSIDL    = 0;        // Operate in Idle Mode 
	ADCONbits.FORM      = 0;        // Output in Integer Format    
	ADCONbits.EIE       = 1;        // Early Interrupt 
	ADCONbits.ORDER     = 0;        // Even channel first 
	ADCONbits.SEQSAMP   = 1;        // Sequential Sampling Enabled 
	ADCONbits.ADCS      = 5;        // Clock Divider is set up for Fadc/14 
	
	
	ADPCFG = 0b1111111111011111;    // AN4 and AN5 are analog inputs 
	ADSTAT = 0;                     // Clear the ADSTAT register 
	ADCPC0bits.TRGSRC0     = 0xC;   // Trigger conversion on TMR1 Prd Match 
	ADCPC0bits.IRQEN0    = 1;       // Enable the interrupt 
	
	// Set up Timer1 
	
	T1CON     = 0;                  // Timer with 0 prescale 
	TMR1    = 0;                    // Clear the Timer counter 
	PR1        = TIMER_PERIOD;      // Load the period register 
	
	// Set up the Interrupts 
	
	IFS0bits.ADIF     = 0;           // Clear AD Interrupt Flag 
	IFS0bits.T1IF    = 0;            // Clear Timer1 Interrupt Flag 
	IEC0bits.T1IE    = 0;            // Timer Interrupt is not needed 
	IPC2bits.ADIP    = 4;            // Set ADC Interrupt Priority 
	IEC0bits.ADIE    = 1;            // Enable the ADC Interrupt 
	
	// Enable ADC and Timer 
	ADCONbits.ADON     = 1;           // Start the ADC module       
	T1CONbits.TON     = 1;            // Start the Timer 
	
	while(1);     
	   
}

void __attribute__ ((__interrupt__)) _ADCInterrupt(void){
	// AD Conversion complete interrupt handler 
	
	int channel0Result, channel1Result;
	
	IFS0bits.ADIF   = 0;            // Clear ADC Interrupt Flag
	channel0Result  = ADCBUF4;      // Get the conversion result
	channel1Result  = ADCBUF5;
	ADSTATbits.P0RDY = 0;           // Clear the ADSTAT bits
}

