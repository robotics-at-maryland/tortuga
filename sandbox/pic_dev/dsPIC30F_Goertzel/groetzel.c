//#include <stdio.h>
//#include "p30f1010.h"

/* Goertzel implementation */

#include <stdio.h>
#include <math.h>

#define BYTE	unsigned char

#define SAMPLING_RATE	500000.0	//1MHz  //8kHz
//#define TARGET_FREQUENCY	30000.0	//20kHz //941 Hz
#define N	500	//Block size

float TARGET_FREQUENCY = 20000;
float coeff;
float Q1;	float Q2;
float sine;	float cosine;

BYTE testData[N];

//function prototypes
void InitGoertzel(void);
void ProcessSample(BYTE sample);
float GetMagnitudeSquared(void);
void GenerateAndTest(float frequency);
void GenerateAndTest2(float frequency);

/* Call this routine before every "block" (size=N) of samples. */
void ResetGoertzel(void){
  Q2 = 0;	Q1 = 0;
}

/* Call this routine before every "block" (size=N) of samples. */
/* Call this once, to precompute the constants. */
void InitGoertzel(){
  int	k;
  float	floatN;
  float	omega;

  floatN = (float) N;
  k = (int) (0.5 + ((floatN * TARGET_FREQUENCY) / SAMPLING_RATE));
  omega = (2.0 * M_PI * k) / floatN;
  sine = sin(omega);
  cosine = cos(omega);
  coeff = 2.0 * cosine;

  printf("For SAMPLING_RATE = %f", SAMPLING_RATE);
  printf(" N = %d", N);
  printf(" and FREQUENCY = %f,\n", TARGET_FREQUENCY);
  printf("k = %d and coeff = %f\n\n", k, coeff);

  Q2 = 0; Q1 = 0;
}

/* Call this routine for every sample. */
void ProcessSample(BYTE sample){
  float Q0;
  Q0 = coeff * Q1 - Q2 + (float) sample;
  Q2 = Q1;
  Q1 = Q0;
}

/* Optimized Goertzel */
/* Call this after every block to get the RELATIVE magnitude squared. */
float GetMagnitudeSquared(void){
  return (Q1 * Q1 + Q2 * Q2 - Q1 * Q2 * coeff);
}


/*** End of Goertzel-specific code, the remainder is test code. */

/* Synthesize some test data at a given frequency. */
void Generate(float frequency){
  int	index;
  float	step;

  step = frequency * ((2.0 * M_PI) / SAMPLING_RATE);

  /* Generate the test data */
  for (index = 0; index < N; index++){
    testData[index] = (BYTE) (500.0 * sin(index * step) + 500.0);
  }
}

/* Demo 1 */
void GenerateAndTest(float frequency){
  int	index;
  float	magnitudeSquared;
  float	magnitude;

  printf("Freq=%7.1f   ", frequency);
  Generate(frequency);

  /* Process the samples. */
  for (index = 0; index < N; index++){
    ProcessSample(testData[index]);
  }

  /* Do the "optimized Goertzel" processing. */
  magnitudeSquared = GetMagnitudeSquared();
  magnitude = sqrt(magnitudeSquared);

  printf("rel mag^2=%16.5f   ", magnitudeSquared);
  printf("rel mag=%12.5f\n", magnitude);

  ResetGoertzel();
}

int main(void){
  float freq;

  for(TARGET_FREQUENCY=20000;TARGET_FREQUENCY<30001;TARGET_FREQUENCY+=1000){
  InitGoertzel();

  /* Demo 1 */
  //for (freq = TARGET_FREQUENCY - 2000; freq <= TARGET_FREQUENCY + 2000; freq += 1000){
  for (freq = 20000; freq <= 30000; freq += 1000){
    GenerateAndTest(freq);
  }

  printf("\n\n");
  }
  return 0;
}

/* Device configuration register macros for building the hex file */
//_FOSC(EC & HS);          
//_FOSCSEL(PRIOSC_PLL);
//_FWDT ( WDT_OFF );
////_FPOR(0xFFF7) //turning off MCLR pin?


/*#define byte unsigned char

void initUart()
{
    U1MODE = 0x0000;
    U1BRG = 832;
    U1MODEbits.ALTIO = 1;   // Use alternate IO
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;   // Enable transmit
}


void sendByte(byte i)
{
 //   U1STAbits.OERR = 0;
 //   U1STAbits.FERR = 0;
  //  U1STAbits.PERR = 0;
  //  U1STAbits.URXDA = 0;
    while(U1STAbits.UTXBF);
    U1TXREG = i;
    while(U1STAbits.UTXBF);
}

void sendString(unsigned char str[])
{
    byte i=0;
    for(i=0; str[i]!=0; i++)
        sendByte(str[i]);
}

int main(void)
{
    long i=0, j=0, t=0, b=0;
    initUart();

    while(1)
    {
        sendString("\n\revil wombat says hello");
        for(j=0; j<100000; j++);
    }

    while(1);
}
*/





/*
void main()
{
	ADPCFG = 0xFFFF;
	TRISB = 0;
	LATB = 0xFFFF;

	long i=0;
	while(1)
	{
		for(i=0; i<65000; i++);
		for(i=0; i<65000; i++);
		for(i=0; i<65000; i++);
		LATB ^= 0xFFFF;
	}

}
*/
