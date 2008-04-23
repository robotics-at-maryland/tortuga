#include <p30fxxxx.h>

//  Configure sliding DFT for 3 kHz, 3 channels
#define M_SLIDING_DFT_k 4
#define M_SLIDING_DFT_N 32
#define M_SLIDING_DFT_nchannels 3
#include <dft_singleton_c30.h>
sliding_dft_t *dft;


/**
 * Stop the car.
 */
void allStop()
{
    //  TODO
}


/**
 * Drive the car in the direction (x,y) where (x,y) is a (not necessarily normalized)
 * 2-dimensional vector relative to the car.
 */
void steerToward(float x, float y)
{
    //  TODO
}


void _ISR _ADCInterrupt(void)
{
    int i, thresholdMet = 1, threshold = 10000;    //  May need to adjust threshold
    adcdata_t rawData[M_SLIDING_DFT_nchannels];
    
    rawData[0] = ADCBUF1;                          //  Copy ADC data
    rawData[1] = ADCBUF2;                          //  Copy ADC data
    rawData[2] = ADCBUF3;                          //  Copy ADC data
    dft_update(dft, rawData);                      //  Propagate the DFT
    
    for (i = 0; i < M_SLIDING_DFT_nchannels; i ++) //  For all channels:
        thresholdMet &= (dft->mag[i] > threshold); //  Check for threshold
    
    if (thresholdMet) {                            //  If we are hearing a ping:
        float d10 = dft_relativePhase(dft, 0, 1);  //  TDOA 0/1
        float d20 = dft_relativePhase(dft, 0, 2);  //  TDOA 0/2
        steerToward(d10, d20);                     //  Steer car
    } else
        allStop();                                 //  Otherwise, halt car
    
    IFS0bits.ADIF = 0;                             //  Clear interrupt bit
}


int main(void)
{
    dft = M_DFT_INIT();  //  Initialize the sliding DFT
    
    //  TODO
    //  Initialize ADCs for:
    //    - simultaneous sampling and sequential convesion
    //    - four channels
    //    - automatic, timed sampling
    //    - interrupt on completion
    //    - signed integer output
    
    while (1);          // run indefinitely
}
