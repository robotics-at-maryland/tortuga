/*header file for main.c and has code-bits that I use often
 *Scott Watson April 20, 2008
 *include this file whenever using main.c functions in another file
 */

//please please please! be careful not to make this too large...the stack needs space too!!!!!
#define SAMPLE_LENGTH 1 
//WAIT... pay attention to this number SAMPLE_LENGTH

#define byte unsigned char
#define _ISR __attribute__((interrupt))
#define _ISRFAST __attribute__((interrupt, shadow))

//from main
void _ISRFAST _U1RXInterrupt(void);
void init_IO(void);
void init_OutputCompare(void);
void startup_flashes_text(void);
void _ISRFAST _T2Interrupt(void);
void _ISRFAST _T3Interrupt(void);
