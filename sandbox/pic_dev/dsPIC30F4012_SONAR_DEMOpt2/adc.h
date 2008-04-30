/*header file for adc.c 
 *Scott Watson April 20, 2008
 *include this file whenever using adc.c functions in another file
 */

void ADC_Init(void);
void __attribute__((__interrupt__)) _ADCInterrupt(void);
