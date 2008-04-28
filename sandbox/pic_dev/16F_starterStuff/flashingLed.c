#include <pic.h>
 __CONFIG(FCMDIS & IESODIS & BORDIS & UNPROTECT & MCLRDIS & PWRTEN &
 WDTDIS & INTIO);

//author: Peter Enns

 int main(void)
 {
	long x;
	 PORTA=0;
	 //CMCON0 = 7;
	 ANSEL=0;
	 TRISA4=0;
	 TRISA5=0;
     TRISA0=0;
	while (1){
		RA0=1;
		for (x=0;x<20000;x++);
		RA0=0;
		for (x=0;x<20000;x++);
	}
	 
	return 0;
}