#ifndef _scottcommon_h
#define _scottcommon_h

typedef unsigned char byte;

#define SAMPLE_LENGTH 100
#define _ISR __attribute__((interrupt))
#define _ISRFAST __attribute__((interrupt, shadow))

//from main
void init_IO(void);
void init_OC2(void);
void init_Uart(byte enabled);
void init_Timer2and3(void);
void sendByte(byte i);
void sendString( char * s);
void sendNum(unsigned int i);
void delay(int microseconds);
void startup_flashes_text(void);
void _ISRFAST _T2Interrupt(void);
void _ISRFAST _T3Interrupt(void);

//ADC specific
void ADC_Init(void);
void __attribute__((__interrupt__)) _ADCInterrupt(void);

#endif
