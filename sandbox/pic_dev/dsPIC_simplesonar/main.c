#include <p30f4012.h>
#include <stdio.h>

//For the dsPIC30F4012
_FOSC(CSW_FSCM_OFF & XT_PLL8); //to get 30MIPS with 15MHz clock
//_FOSC(CSW_FSCM_OFF & XT_PLL16); //to get 30MIPS with 7.37MHz clock
_FOSCSEL(PRIOSC_PLL);
_FWDT(WDT_OFF);	
_FGS(CODE_PROT_OFF);

void initUart(void)
{
    U1MODE = 0x0000;
    U1BRG = 15;  // 25 for baud of 38400 //7 for baud of 230400 pll8
    U1MODEbits.ALTIO = 1;   // Use alternate IO
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;   // Enable transmit
}

void main(void)
{
	//initUart();
	printf("Hello, world!\n");
}
