#include <outcompare.h>

#ifdef _OC2IF

/*************************************************************************
*    Function Name:  SetPulseOC2                                         *
*    Description:    This routine writes the pulse_start to Main register*
*                    and pulse_stop to Secondary register                *
*    Parameters:     unsigned int pulse_start,unsigned int pulse_stop    *
*    Return Value:   None                                                *
*************************************************************************/

void SetPulseOC2(unsigned int pulse_start, unsigned int pulse_stop)
{   

    /* check if OC is in NON PWM Mode. */
    if( (OC2CONbits.OCM & 0x06) != 0x06) 
    {
        OC2R = pulse_start; /* assign pulse_start to Main Register */
        OC2RS = pulse_stop; /* assign pulse_stop to Secondary Register */
    }
}

#else 
#warning "Does not build on this target"
#endif
