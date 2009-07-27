/********************************************************************/
//              Header for OutCompare module library functions      */
/********************************************************************/

#if defined(__dsPIC30F__)
#include <p30fxxxx.h>
#elif defined(__dsPIC33F__)
#include <p33Fxxxx.h>
#elif defined(__PIC24H__)
#include <p24Hxxxx.h>
#endif

#ifndef __OutCompare_H
#define __OutCompare_H

/* List of SFRs for Output compare modules */

#define OC1RS_VALUE                0x0000
#define OC1R_VALUE                 0x0000
#define OC1CON_VALUE               0x0000

#ifdef _OC2IF

#define OC2RS_VALUE                0x0000
#define OC2R_VALUE                 0x0000
#define OC2CON_VALUE               0x0000

#endif

#ifdef _OC3IF

#define OC3RS_VALUE                0x0000
#define OC3R_VALUE                 0x0000
#define OC3CON_VALUE               0x0000

#endif

#ifdef _OC4IF

#define OC4RS_VALUE                0x0000
#define OC4R_VALUE                 0x0000
#define OC4CON_VALUE               0x0000

#endif

#ifdef _OC5IF

#define OC5RS_VALUE                0x0000
#define OC5R_VALUE                 0x0000
#define OC5CON_VALUE               0x0000

#endif

#ifdef _OC6IF

#define OC6RS_VALUE                0x0000
#define OC6R_VALUE                 0x0000
#define OC6CON_VALUE               0x0000

#endif

#ifdef _OC7IF

#define OC7RS_VALUE                0x0000
#define OC7R_VALUE                 0x0000
#define OC7CON_VALUE               0x0000

#endif

#ifdef _OC8IF

#define OC8RS_VALUE                0x0000
#define OC8R_VALUE                 0x0000
#define OC8CON_VALUE               0x0000

#endif

/* Output Compare Stop in Idle mode Bit defines */
#define OC_IDLE_STOP               0xffff   /* stop in idle mode */
#define OC_IDLE_CON                0xdfff   /* continue operation in idle mode */

/* Output Compare timer select Bit Defines */
#define OC_TIMER2_SRC               0xfff7  /* Timer2 is the clock source 
                                                    for OutputCompare1 */
#define OC_TIMER3_SRC               0xffff  /* Timer3 is the clock source
                                                     for OutputCompare1 */

/* PWM Mode on OCx, fault pin enabled, (TxIF bit is set for PWM, OCxIF is set for fault)*/
#define OC_PWM_FAULT_PIN_ENABLE     0xffff

/* PWM Mode on OCx, fault pin disabled */
#define OC_PWM_FAULT_PIN_DISABLE    0xfffe
   
/* Generates Continuous Output pulse on OCx Pin */
#define OC_CONTINUE_PULSE           0xfffd
  
/* Generates Single Output pulse on OCx Pin */
#define OC_SINGLE_PULSE             0xfffc  
#define OC_TOGGLE_PULSE             0xfffb  /* Compare1 toggels  OCx pin*/
#define OC_HIGH_LOW                 0xfffa  /* Compare1 forces   OCx pin Low*/
#define OC_LOW_HIGH                 0xfff9  /* Compare1 forces  OCx pin High*/
#define OC_OFF                      0xfff8  /* OutputCompare x Off*/


/* Interrupt bit definitions */
#define OC_INT_ON                   0xffff  /* OutputCompare Enable  */
#define OC_INT_OFF                  0xfff7  /* OutputCompare Disable */
    
#define OC_INT_PRIOR_0              0xfff8  /* OutputCompare PriorityLevel 0 */
#define OC_INT_PRIOR_1              0xfff9  /* OutputCompare PriorityLevel 1 */
#define OC_INT_PRIOR_2              0xfffa  /* OutputCompare PriorityLevel 2 */
#define OC_INT_PRIOR_3              0xfffb  /* OutputCompare PriorityLevel 3 */
#define OC_INT_PRIOR_4              0xfffc  /* OutputCompare PriorityLevel 4 */
#define OC_INT_PRIOR_5              0xfffd  /* OutputCompare PriorityLevel 5 */
#define OC_INT_PRIOR_6              0xfffe  /* OutputCompare PriorityLevel 6 */
#define OC_INT_PRIOR_7              0xffff  /* OutputCompare PriorityLevel 7 */

#define Start                       0x01
#define Stop                        0x00

#define EnableIntOC1                    _OC1IE = 1
#define DisableIntOC1                   _OC1IE = 0
#define SetPriorityIntOC1(priority)     _OC1IP = priority

/*  CloseOC1    */
void CloseOC1() __attribute__((section (".libperi")));

/* ConfigIntOC1  */
void ConfigIntOC1(unsigned int config) __attribute__ ((section (".libperi")));

/*  OpenOC1   */
void OpenOC1(unsigned int config,unsigned int value1,
                 unsigned int value2) __attribute__ ((section (".libperi")));

/*  ReadDCOC1PWM    */
unsigned int ReadDCOC1PWM() __attribute__((section (".libperi")));

/*  ReadRegOC1    */
unsigned int ReadRegOC1(char reg) __attribute__((section (".libperi")));

/*  SetDCOC1PWM  */
void SetDCOC1PWM(unsigned int dutycycle) __attribute__((section (".libperi")));

/*  SetPulseOC1    */
void SetPulseOC1(unsigned int pulse_start,unsigned int pulse_stop) __attribute__((section (".libperi")));

#ifdef _OC2IF

#define EnableIntOC2                    _OC2IE = 1
#define DisableIntOC2                   _OC2IE = 0
#define SetPriorityIntOC2(priority)     _OC2IP = priority

/*  CloseOC2    */
void CloseOC2() __attribute__((section (".libperi")));

/* ConfigIntOC2  */
void ConfigIntOC2(unsigned int config) __attribute__ ((section (".libperi")));

/*  OpenOC2   */
void OpenOC2(unsigned int config,unsigned int value1,
                 unsigned int value2) __attribute__ ((section (".libperi")));

/*  ReadDCOC2PWM    */
unsigned int ReadDCOC2PWM() __attribute__((section (".libperi")));

/*  ReadRegOC2    */
unsigned int ReadRegOC2(char reg) __attribute__((section (".libperi")));

/*  SetDCOC2PWM  */
void SetDCOC2PWM(unsigned int dutycycle) __attribute__((section (".libperi")));

/*  SetPulseOC2    */
void SetPulseOC2(unsigned int pulse_start,unsigned int pulse_stop) __attribute__((section (".libperi")));
    
#endif

#ifdef _OC3IF

#define EnableIntOC3                    _OC3IE = 1
#define DisableIntOC3                   _OC3IE = 0
#define SetPriorityIntOC3(priority)     _OC3IP = priority

/*  CloseOC3    */
void CloseOC3() __attribute__ ((section (".libperi")));

/* ConfigIntOC3  */
void ConfigIntOC3(unsigned int config) __attribute__((section(".libperi")));

/*  OpenOC3   */
void OpenOC3(unsigned int config,unsigned int value1,
             unsigned int value2) __attribute__ ((section(".libperi")));

/*  ReadDCOC3    */
unsigned int ReadDCOC3PWM() __attribute__ ((section (".libperi")));

/*  ReadRegOC3    */
unsigned int ReadRegOC3(char reg) __attribute__ ((section(".libperi")));

/*  SetDCOC3PWM  */
void SetDCOC3PWM(unsigned int dutycycle) __attribute__ ((section(".libperi")));

/*  SetPulseOC3    */
void SetPulseOC3(unsigned int pulse_start,unsigned int pulse_stop) __attribute__ 
((section (".libperi")));

#endif    

#ifdef _OC4IF

#define EnableIntOC4                    _OC4IE = 1
#define DisableIntOC4                   _OC4IE = 0
#define SetPriorityIntOC4(priority)     _OC4IP = priority

/*  CloseOC4    */
void CloseOC4() __attribute__ ((section (".libperi")));

/* ConfigIntOC4  */
void ConfigIntOC4(unsigned int config) __attribute__((section(".libperi")));

/*  OpenOC4   */
void OpenOC4(unsigned int config,unsigned int value1,
                 unsigned int value2) __attribute__ ((section(".libperi")));

/*  ReadDCOC4    */
unsigned int ReadDCOC4PWM() __attribute__ ((section (".libperi")));

/*  ReadRegOC4     */
unsigned int ReadRegOC4(char reg) __attribute__ ((section(".libperi")));

/*  SetDCOC4PWM  */
void SetDCOC4PWM(unsigned int dutycycle) __attribute__ ((section(".libperi")));

/*  SetPulseOC4    */
void SetPulseOC4(unsigned int pulse_start,unsigned int pulse_stop) __attribute__ 
((section (".libperi")));

#endif

#ifdef _OC5IF

#define EnableIntOC5                    _OC5IE = 1
#define DisableIntOC5                   _OC5IE = 0
#define SetPriorityIntOC5(priority)     _OC5IP = priority

/*  CloseOC5    */
void CloseOC5() __attribute__ ((section (".libperi")));

/* ConfigIntOC5  */
void ConfigIntOC5(unsigned int config) __attribute__ ((section(".libperi")));

/*  OpenOC5   */
void OpenOC5(unsigned int config,unsigned int value1,
                  unsigned int value2) __attribute__ ((section(".libperi")));

/*  ReadDCOC5PWM    */
unsigned int ReadDCOC5PWM() __attribute__ ((section (".libperi")));

/*  ReadRegOC5    */
unsigned int ReadRegOC5(char reg) __attribute__ ((section(".libperi")));

/*  SetDCOC5PWM  */
void SetDCOC5PWM(unsigned int dutycycle) __attribute__ ((section(".libperi")));

/*  SetPulseOC5    */
void SetPulseOC5(unsigned int pulse_start,unsigned int pulse_stop) __attribute__ 
((section (".libperi")));

#endif

#ifdef _OC6IF

#define EnableIntOC6                    _OC6IE = 1
#define DisableIntOC6                   _OC6IE = 0
#define SetPriorityIntOC6(priority)     _OC6IP = priority

/*  CloseOC6    */
void CloseOC6() __attribute__ ((section (".libperi")));

/* ConfigIntOC6  */
void ConfigIntOC6(unsigned int config) __attribute__ ((section(".libperi")));

/*  OpenOC6   */
void OpenOC6(unsigned int config,unsigned int value1,
                  unsigned int value2) __attribute__ ((section(".libperi")));

/*  ReadDCOC6PWM    */
unsigned int ReadDCOC6PWM() __attribute__ ((section (".libperi")));

/*  ReadRegOC6    */
unsigned int ReadRegOC6(char reg) __attribute__ ((section(".libperi")));

/*  SetDCOC6PWM  */
void SetDCOC6PWM(unsigned int dutycycle) __attribute__ ((section(".libperi")));

/*  SetPulseOC6    */
void SetPulseOC6(unsigned int pulse_start,unsigned int pulse_stop) __attribute__ 
((section (".libperi")));

#endif

#ifdef _OC7IF

#define EnableIntOC7                    _OC7IE = 1
#define DisableIntOC7                   _OC7IE = 0
#define SetPriorityIntOC7(priority)     _OC7IP = priority

/*  CloseOC7    */
void CloseOC7() __attribute__ ((section (".libperi")));

/* ConfigIntOC7  */
void ConfigIntOC7(unsigned int config) __attribute__ ((section(".libperi")));

/*  OpenOC7   */
void OpenOC7(unsigned int config,unsigned int value1,
                 unsigned int value2) __attribute__ ((section(".libperi")));

/*  ReadDCOC7PWM    */
unsigned int ReadDCOC7PWM() __attribute__ ((section (".libperi")));

/*  ReadRegOC7    */
unsigned int ReadRegOC7(char reg) __attribute__ ((section(".libperi")));

/*  SetDCOC7PWM  */
void SetDCOC7PWM(unsigned int dutycycle) __attribute__ ((section(".libperi")));

/*  SetPulseOC7    */
void SetPulseOC7(unsigned int pulse_start,unsigned int pulse_stop) __attribute__ 
((section (".libperi")));

#endif

#ifdef _IC8IF

#define EnableIntOC8                    _OC8IE = 1
#define DisableIntOC8                   _OC8IE = 0
#define SetPriorityIntOC8(priority)     _OC8IP = priority

/*  CloseOC8    */
void CloseOC8() __attribute__ ((section (".libperi")));

/* ConfigIntOC8  */
void ConfigIntOC8(unsigned int config) __attribute__ ((section(".libperi")));

/*  ReadDCOC8PWM    */
unsigned int ReadDCOC8PWM() __attribute__ ((section (".libperi")));

/*  ReadRegOC8    */
unsigned int ReadRegOC8(char reg) __attribute__ ((section(".libperi")));

/*  OpenOC8   */
void OpenOC8(unsigned int config,unsigned int value1,
                 unsigned int value2) __attribute__ ((section(".libperi")));

/*  SetDCOC8PWM  */
void SetDCOC8PWM(unsigned int dutycycle) __attribute__ ((section(".libperi")));

/*  SetPulseOC8    */
void SetPulseOC8(unsigned int pulse_start,unsigned int pulse_stop) __attribute__ 
((section (".libperi")));

#endif

#endif
