#include <outcompare.h>

#ifdef _OC4IF

/*************************************************************************
*    Function Name:  SetPulseOC4                                         *
*    Description:    This routine writes the pulse_start to Main register*
*                    and pulse_stop to Secondary register                *
*    Parameters:     unsigned int pulse_start,unsigned int pulse_stop    *
*    Return Value:   None                                                *
*************************************************************************/

void SetPulseOC4(unsigned int pulse_start, unsigned int pulse_stop)
{   
    /* check if OC is in NON PWM Mode. */
    if( (OC4CONbits.OCM & 0x06) != 0x06) 
    {
        OC4R = pulse_start; /* assign pulse_start to Main Register */
        OC4RS = pulse_stop; /* assign pulse_stop to Secondary Register */
    }
}

#else
#warning "Does not build on this target"
#endif
